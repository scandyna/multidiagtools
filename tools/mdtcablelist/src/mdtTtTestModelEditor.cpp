/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtTtTestModelEditor.h"
#include "ui_mdtTtTestModelEditor.h"
#include "mdtTtTestModel.h"
#include "mdtTtTestModelItem.h"
#include "mdtTtTestModelItemEditor.h"
#include "mdtSqlFormOld.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlWindowOld.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
///#include <QDataWidgetMapper>
#include <QVariant>
#include <QString>
#include <QPushButton>
#include <QSqlError>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>
#include <QList>

#include <QDebug>

mdtTtTestModelEditor::mdtTtTestModelEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

void mdtTtTestModelEditor::addTestItem() 
{
  mdtTtTestModel t(database());
  QVariant testId;
  QVariant testCableId;
  QVariant testLinkBusAId;
  QVariant testLinkBusBId;
  QVariant expectedValue;

  // Get current test ID
  testId = form()->currentData("TestModel_tbl", "Id_PK");
  if(testId.isNull()){
    return;
  }
  // Select test cable
  testCableId = selectTestCable();
  if(testCableId.isNull()){
    return;
  }
  // Select test link to use on bus A
  testLinkBusAId = selectTestLink(testCableId, "BUSA");
  if(testLinkBusAId.isNull()){
    return;
  }
  // Select test link to use on bus B
  testLinkBusBId = selectTestLink(testCableId, "BUSB");
  if(testLinkBusBId.isNull()){
    return;
  }
  // Set a expectedValue
  ///expectedValue = 1.2;
  // Add to db
  if(!t.addTestItem(testId, testLinkBusAId, testLinkBusBId, expectedValue)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  form()->select("TestModelItem_view");
  form()->select("TestModelItemNode_view");
  // Generate setup
  generateTestNodeUnitSetupList();
}

void mdtTtTestModelEditor::editTestItem()
{
  mdtTtTestModelItemEditor *tie;
  mdtSqlDialog dialog;
  QVariant currentTestItemId;

  currentTestItemId = form()->currentData("TestModelItem_view", "Id_PK");
  if(currentTestItemId.isNull()){
    return;
  }
  tie = new mdtTtTestModelItemEditor(0, database());
  if(!tie->setupTables()){
    pvLastError = tie->lastError();
    displayLastError();
    return;
  }
  tie->setMainTableFilter("Id_PK", currentTestItemId);
  dialog.setSqlForm(tie);
  dialog.resize(700, 400);
  dialog.enableEdition();
  dialog.exec();
  // Update connections table
  form()->select("TestModelItem_view");
  form()->select("TestModelItemNode_view");
  form()->select("TestModelItemNodeUnitSetup_view");
}

void mdtTtTestModelEditor::removeTestItem() 
{
  mdtSqlTableWidget *widget;
  mdtTtTestModel t(database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = form()->sqlTableWidget("TestModelItem_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove selected test items."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  ///if(!t.removeTestItems(indexes)){
  if(!t.removeData("TestModelItem_tbl", "Id_PK", indexes)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("TestModelItem_view");
  form()->select("TestModelItemNode_view");
}

void mdtTtTestModelEditor::generateTestNodeUnitSetupList()
{
  mdtTtTestModel t(database());
  QVariant testId;
  QList<QVariant> setupIdList;

  // Get current test ID
  testId = form()->currentData("TestModel_tbl", "Id_PK");
  if(testId.isNull()){
    return;
  }
  // Warn user if some setup allready exists
  setupIdList = t.getTestNodeUnitSetupIdList(testId);
  if(!setupIdList.isEmpty()){
    QString text;
    QMessageBox msgBox;
    text = tr("Setups will be generated for current test. ");
    text += tr("Some setups allready exists, and they will be deleted if you continue.");
    msgBox.setText(text);
    msgBox.setInformativeText(tr("Do you want to continue ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  // Generate setup
  if(!t.generateTestNodeUnitSetup(testId)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  form()->select("TestModelItemNodeUnitSetup_view");
}

void mdtTtTestModelEditor::removeTestNodeUnitSetup()
{
  mdtSqlTableWidget *widget;
  mdtTtTestModelItem ti(database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = form()->sqlTableWidget("TestModelItemNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove selected setup items."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!ti.removeTestNodeUnitSetups(indexes)){
    pvLastError = ti.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("TestModelItemNodeUnitSetup_view");
}

QVariant mdtTtTestModelEditor::selectTestCable()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM TestCable_tbl";
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of test cables."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select test cable to use."));
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(600, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestModelEditor::selectTestLink(const QVariant & cableId, const QString & bus)
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM TestLink_view WHERE TestCable_Id_FK = " + cableId.toString() + " AND Bus = '" + bus + "'";
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of test links."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select link for bus '") + bus + tr("'"));
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("TestConnection_Id_FK", true);
  selectionDialog.setColumnHidden("DutConnection_Id_FK", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

bool mdtTtTestModelEditor::setupTables() 
{
  if(!setupTestTable()){
    return false;
  }
  if(!setupTestItemTable()){
    return false;
  }
  if(!setupTestNodeUnitSetupTable()){
    return false;
  }
  if(!setupTestNodeTable()){
    return false;
  }
  return true;
}

bool mdtTtTestModelEditor::setupTestTable()
{
  Ui::mdtTtTestModelEditor te;

  // Setup main form widget
  te.setupUi(form()->mainSqlWidget());
  connect(te.pbGenerateNodeUnitSetup, SIGNAL(clicked()), this, SLOT(generateTestNodeUnitSetupList()));
  // Setup form
  if(!form()->setTable("TestModel_tbl", "Test", database())){
    return false;
  }
  sqlWindow()->enableNavigation();
  sqlWindow()->enableEdition();
  sqlWindow()->resize(800, 500);
  sqlWindow()->setWindowTitle(tr("Test edition"));
  // Force a update
  form()->mainSqlWidget()->setCurrentIndex(form()->mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestModelEditor::setupTestItemTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddTestItem;
  QPushButton *pbEditTestItem;
  QPushButton *pbRemoveTestItem;

  if(!form()->addChildTable("TestModelItem_view", tr("Test items"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestModelItem_view", "TestModel_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("TestModelItem_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestModel_Id_FK", true);
  widget->setColumnHidden("TestLinkBusA_Id_FK", true);
  widget->setColumnHidden("TestLinkBusB_Id_FK", true);
  widget->setColumnHidden("TestConnectorNameBusA", true);
  widget->setColumnHidden("TestContactNameBusA", true);
  widget->setColumnHidden("TestConnectorNameBusB", true);
  widget->setColumnHidden("TestContactNameBusB", true);
  // Set fields a user friendly name
  widget->setHeaderData("SequenceNumber", tr("Seq. #"));
  widget->setHeaderData("ExpectedValue", tr("Value\nExpected"));
  ///widget->setHeaderData("TestConnectorNameBusA", tr("Test\nConnector\nBus A"));
  ///widget->setHeaderData("TestConnectorNameBusB", tr("Test\nConnector\nBus B"));
  ///widget->setHeaderData("TestContactNameBusA", tr("Test\nContact\nBus A"));
  ///widget->setHeaderData("TestContactNameBusB", tr("Test\nContact\nBus B"));
  widget->setHeaderData("DutUnitSchemaPositionBusA", tr("DUT BUS A"));
  widget->setHeaderData("DutUnitSchemaPositionBusB", tr("DUT BUS B"));
  widget->setHeaderData("DutConnectorNameBusA", tr("DUT\nConnector\nBus A"));
  widget->setHeaderData("DutConnectorNameBusB", tr("DUT\nConnector\nBus B"));
  widget->setHeaderData("DutContactNameBusA", tr("DUT\nContact\nBus A"));
  widget->setHeaderData("DutContactNameBusB", tr("DUT\nContact\nBus B"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbAddTestItem = new QPushButton(tr("Add item"));
  connect(pbAddTestItem, SIGNAL(clicked()), this, SLOT(addTestItem()));
  widget->addWidgetToLocalBar(pbAddTestItem);
  pbEditTestItem = new QPushButton(tr("Edit item"));
  connect(pbEditTestItem, SIGNAL(clicked()), this, SLOT(editTestItem()));
  widget->addWidgetToLocalBar(pbEditTestItem);
  pbRemoveTestItem = new QPushButton(tr("Remove items ..."));
  connect(pbRemoveTestItem, SIGNAL(clicked()), this, SLOT(removeTestItem()));
  widget->addWidgetToLocalBar(pbRemoveTestItem);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestModelEditor::setupTestNodeUnitSetupTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbRemoveTestNodeUnitSetup;

  if(!form()->addChildTable("TestModelItemNodeUnitSetup_view", tr("Node unit setup"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestModelItemNodeUnitSetup_view", "TestModel_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("TestModelItemNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestModel_Id_FK", true);
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestModelItem_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("SequenceNumber", tr("Seq. #"));
  widget->setHeaderData("SchemaPosition", tr("Node unit"));
  widget->setHeaderData("NodeId", tr("Node ID"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbRemoveTestNodeUnitSetup = new QPushButton(tr("Remove setup item"));
  connect(pbRemoveTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(removeTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbRemoveTestNodeUnitSetup);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestModelEditor::setupTestNodeTable()
{
  mdtSqlTableWidget *widget;

  if(!form()->addChildTable("TestModelItemNode_view", tr("Used nodes"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestModelItemNode_view", "TestModel_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("TestModelItemNode_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons

  return true;
}
