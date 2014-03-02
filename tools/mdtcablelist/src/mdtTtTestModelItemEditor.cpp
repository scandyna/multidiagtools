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
#include "mdtTtTestModelItemEditor.h"
#include "ui_mdtTtTestModelItemEditor.h"
#include "mdtTtTestModelItem.h"
///#include "mdtSqlFormOld.h"
#include "mdtSqlFormWidget.h"
///#include "mdtSqlWindowOld.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QVariant>
#include <QString>
#include <QPushButton>
#include <QSqlError>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>
#include <QList>

#include <QDebug>

/**
mdtTtTestModelItemEditor::mdtTtTestModelItemEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}
*/

mdtTtTestModelItemEditor::mdtTtTestModelItemEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestModelItemEditor::setupTables()
{
  if(!setupTestItemTable()){
    return false;
  }
  if(!setupTestLinkTable()){
    return false;
  }
  if(!setupTestNodeUnitSetupTable()){
    return false;
  }
  return true;
}

void mdtTtTestModelItemEditor::setTestLink()
{
  mdtTtTestModelItem tmi(this, database());
  QVariant testItemId;
  QVariant testLinkBusAId;
  QVariant testLinkBusBId;

  // Get current test ID
  testItemId = currentData("TestModelItem_tbl", "Id_PK");
  if(testItemId.isNull()){
    return;
  }
  // Select test link to use on bus A
  testLinkBusAId = selectTestLink("BUSA");
  if(testLinkBusAId.isNull()){
    return;
  }
  // Select test link to use on bus B
  testLinkBusBId = selectTestLink("BUSB");
  if(testLinkBusBId.isNull()){
    return;
  }
  // Add to db
  if(!tmi.setTestLink(testItemId, testLinkBusAId, testLinkBusBId)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  select("TestModelItem_view");
  // Update setup
  generateTestNodeUnitSetup();
}

void mdtTtTestModelItemEditor::generateTestNodeUnitSetup()
{
  mdtTtTestModelItem tmi(this, database());
  QVariant testItemId;
  QList<QVariant> setupIdList;

  // Get current test ID
  qDebug() << "Get testItemId ...";
  testItemId = currentData("TestModelItem_tbl", "Id_PK");
  if(testItemId.isNull()){
    return;
  }
  qDebug() << "-> testItemId: " << testItemId;
  qDebug() << "Get setupIdList ...";
  // Warn user if some setup allready exists
  setupIdList = tmi.getTestNodeUnitSetupIdList(testItemId);
  qDebug() << "-> setupIdList: " << setupIdList;
  if(!setupIdList.isEmpty()){
    QString text;
    QMessageBox msgBox;
    text = tr("Setups will be generated for current test item. ");
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
  qDebug() << "Generate ...";
  if(!tmi.generateTestNodeUnitSetup(testItemId)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  qDebug() << "-> DONE";
  // Update GUI
  select("TestModelItemNodeUnitSetup_view");
}

void mdtTtTestModelItemEditor::removeTestNodeUnitSetup()
{
  mdtSqlTableWidget *widget;
  mdtTtTestModelItem tmi(this, database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = sqlTableWidget("TestModelItemNodeUnitSetup_view");
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
  if(!tmi.removeTestNodeUnitSetups(indexes)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  select("TestModelItemNodeUnitSetup_view");
}

QVariant mdtTtTestModelItemEditor::selectTestLink(const QString & bus)
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM TestLink_view WHERE Bus = '" + bus + "'";
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of test links."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItemEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select link for bus '") + bus + tr("'"));
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
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

bool mdtTtTestModelItemEditor::setupTestItemTable() 
{
  Ui::mdtTtTestModelItemEditor tie;

  // Setup main form widget
  tie.setupUi(mainSqlWidget());
  // Setup form
  if(!setMainTable("TestModelItem_tbl", "Test item", database())){
    return false;
  }
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestModelItemEditor::setupTestLinkTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbSetTestLink;

  if(!addChildTable("TestModelItem_view", tr("Link"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestModelItem_view", "Id_PK")){
    return false;
  }
  widget = sqlTableWidget("TestModelItem_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestModel_Id_FK", true);
  widget->setColumnHidden("TestLinkBusA_Id_FK", true);
  widget->setColumnHidden("TestLinkBusB_Id_FK", true);
  widget->setColumnHidden("SequenceNumber", true);
  widget->setColumnHidden("ExpectedValue", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("ExpectedValue", tr("Value\nExpected"));
  widget->setHeaderData("TestConnectorNameBusA", tr("Test\nConnector\nBus A"));
  widget->setHeaderData("TestConnectorNameBusB", tr("Test\nConnector\nBus B"));
  widget->setHeaderData("TestContactNameBusA", tr("Test\nContact\nBus A"));
  widget->setHeaderData("TestContactNameBusB", tr("Test\nContact\nBus B"));
  widget->setHeaderData("DutUnitSchemaPositionBusA", tr("DUT BUS A"));
  widget->setHeaderData("DutUnitSchemaPositionBusB", tr("DUT BUS B"));
  widget->setHeaderData("DutConnectorNameBusA", tr("DUT\nConnector\nBus A"));
  widget->setHeaderData("DutConnectorNameBusB", tr("DUT\nConnector\nBus B"));
  widget->setHeaderData("DutContactNameBusA", tr("DUT\nContact\nBus A"));
  widget->setHeaderData("DutContactNameBusB", tr("DUT\nContact\nBus B"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbSetTestLink = new QPushButton(tr("Set test link ..."));
  connect(pbSetTestLink, SIGNAL(clicked()), this, SLOT(setTestLink()));
  widget->addWidgetToLocalBar(pbSetTestLink);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestModelItemEditor::setupTestNodeUnitSetupTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbGenerateTestNodeUnitSetup;
  QPushButton *pbRemoveTestNodeUnitSetup;

  if(!addChildTable("TestModelItemNodeUnitSetup_view", tr("Node unit setup"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestModelItemNodeUnitSetup_view", "TestModelItem_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestModelItemNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbGenerateTestNodeUnitSetup = new QPushButton(tr("Generate setup"));
  connect(pbGenerateTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(generateTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbGenerateTestNodeUnitSetup);
  pbRemoveTestNodeUnitSetup = new QPushButton(tr("Remove setup item"));
  connect(pbRemoveTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(removeTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbRemoveTestNodeUnitSetup);
  widget->addStretchToLocalBar();

  return true;
}

