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
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtTtTestNodeUnitSetupDialog.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QVariant>
#include <QString>
#include <QPushButton>
#include <QIcon>
#include <QSqlError>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>
#include <QList>
#include <QStringList>

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

void mdtTtTestModelItemEditor::setTestModelId(const QVariant & id)
{
  if(!setCurrentData("TestModelItem_tbl", "TestModel_Id_FK", id, false)){
    displayLastError();
  }
}

void mdtTtTestModelItemEditor::setSequenceNumber(const QVariant & seqNumber)
{
  if(!setCurrentData("TestModelItem_tbl", "SequenceNumber", seqNumber, false)){
    displayLastError();
  }
}

void mdtTtTestModelItemEditor::addTestLink()
{
  mdtTtTestModelItem tmi(0, database());
  QVariant testModelItemId;
  QVariant testLinkId;
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  // Get test model item ID
  testModelItemId = currentData("TestModelItem_tbl", "Id_PK");
  if(testModelItemId.isNull()){
    return;
  }
  // Setup and show dialog for test link selection
  sql = tmi.sqlForTestLinkSelection(testModelItemId);
  selectionDialog.setMessage(tr("Please select a test link to use."));
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("TestCable_Id_FK", true);
  selectionDialog.setColumnHidden("TestConnection_Id_FK", true);
  selectionDialog.setColumnHidden("DutConnection_Id_FK", true);
  selectionDialog.setColumnHidden("TestLinkValue", true);
  selectionDialog.setColumnHidden("Unit_Id_FK_PK", true);
  selectionDialog.setColumnHidden("IoPosition", true);
  selectionDialog.setColumnHidden("Bus", true);
  selectionDialog.setColumnHidden("VehicleType_Id_FK_PK", true);
  selectionDialog.setHeaderData("TestLinkIdentification", tr("Identification"));
  selectionDialog.setHeaderData("TestNodeUnitSchemaPosition", tr("Test node\nunit\nschema pos."));
  selectionDialog.setHeaderData("TestConnectorName", tr("Test\nconnector"));
  selectionDialog.setHeaderData("TestContactName", tr("Test\ncontact"));
  selectionDialog.setHeaderData("DutUnitSchemaPosition", tr("DUT\nSchema pos."));
  selectionDialog.setHeaderData("DutUnitAlias", tr("DUT\nalias"));
  selectionDialog.setHeaderData("DutConnectorName", tr("DUT\nconnector"));
  selectionDialog.setHeaderData("DutContactName", tr("DUT\ncontact"));
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  testLinkId = s.data(0, "Id_PK");
  // Add to db
  if(!tmi.addTestLink(testModelItemId, testLinkId)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestModelItem_TestLink_view");
}

void mdtTtTestModelItemEditor::removeTestLinks()
{
  mdtTtTestModelItem tmi(0, database());
  QStringList fields;
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;

  // Get widget and selection
  widget = sqlTableWidget("TestModelItem_TestLink_view");
  Q_ASSERT(widget != 0);
  fields << "TestModelItem_Id_FK" << "TestLink_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove test links from current test item."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!tmi.removeTestLinks(s)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestModelItem_TestLink_view");
}

void mdtTtTestModelItemEditor::addNodeUnit()
{
  mdtTtTestModelItem tmi(0, database());
  QVariant testModelItemId;
  mdtTtTestNodeUnitSetupData data;
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  // Get test model item ID
  testModelItemId = currentData("TestModelItem_tbl", "Id_PK");
  if(testModelItemId.isNull()){
    return;
  }
  // Setup data
  if(!data.setup(database())){
    pvLastError = data.lastError();
    displayLastError();
    return;
  }
  data.setValue("TestModelItem_Id_FK", testModelItemId);
  // Setup and show dialog for test link selection
  sql = tmi.sqlForTestNodeUnitSelection(testModelItemId);
  selectionDialog.setMessage(tr("Please select a test node unit to use."));
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Unit_Id_FK_PK", true);
  selectionDialog.setColumnHidden("TestNode_Id_FK", true);
  selectionDialog.setColumnHidden("Type_Code_FK", true);
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema\npos."));
  selectionDialog.setHeaderData("NameEN", tr("Type"));
  selectionDialog.setHeaderData("NameFR", tr("Type"));
  selectionDialog.setHeaderData("NameDE", tr("Type"));
  selectionDialog.setHeaderData("NameIT", tr("Type"));
  selectionDialog.setHeaderData("Type", tr("Test system"));
  selectionDialog.setHeaderData("SubType", tr("Test node"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_FK_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  data.setValue("TestNodeUnit_Id_FK", s.data(0, "Unit_Id_FK_PK"));
  // Add to db
  if(!tmi.addTestNodeUnitSetup(data)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestNodeUnitSetup_view");
}

void mdtTtTestModelItemEditor::setupNodeUnit()
{
  mdtTtTestModelItem tmi(0, database());
  mdtTtTestNodeUnitSetupDialog dialog(this, database());
  mdtTtTestNodeUnitSetupData data;
  QVariant testModelItemId;
  QVariant testNodeUnitId;
  bool ok;

  // Get test model item ID
  testModelItemId = currentData("TestModelItem_tbl", "Id_PK");
  if(testModelItemId.isNull()){
    return;
  }
  // Get test node unit ID
  testNodeUnitId = currentData("TestNodeUnitSetup_view", "TestNodeUnit_Id_FK");
  if(testNodeUnitId.isNull()){
    return;
  }
  // Get data
  data = tmi.getNodeUnitSetupData(testModelItemId, testNodeUnitId, &ok);
  if(!ok){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  qDebug() << "Data: " << data;
  // Setup and show dialog
  dialog.setData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update in db
  data = dialog.data();
  if(!tmi.updateNodeUnitData(testModelItemId, testNodeUnitId, data)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestNodeUnitSetup_view");
}

void mdtTtTestModelItemEditor::removeNodeUnits()
{
  mdtTtTestModelItem tmi(0, database());
  QStringList fields;
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;

  // Get widget and selection
  widget = sqlTableWidget("TestNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  fields << "TestModelItem_Id_FK" << "TestNodeUnit_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove test node units from current setup list."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!tmi.removeTestNodeUnitSetups(s)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestNodeUnitSetup_view");
}


/**
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
*/

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
  ///mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestModelItemEditor::setupTestLinkTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddTestLink;
  QPushButton *pbRemoveTestLinks;

  if(!addChildTable("TestModelItem_TestLink_view", tr("Test links"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestModelItem_TestLink_view", "TestModelItem_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestModelItem_TestLink_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestModelItem_Id_FK", true);
  widget->setColumnHidden("TestLink_Id_FK", true);
  widget->setColumnHidden("TestConnection_Id_FK", true);
  widget->setColumnHidden("DutConnection_Id_FK", true);
  widget->setColumnHidden("Unit_Id_FK_PK", true);
  widget->setColumnHidden("DutUnitId", true);
  widget->setColumnHidden("VehicleType_Id_FK_PK", true);
  // Rename visible fields to something human friendly
  widget->setHeaderData("IoPosition", tr("I/O\nPos."));
  widget->setHeaderData("TestNodeUnitSchemaPosition", tr("Test node\nunit\nschema pos."));
  widget->setHeaderData("TestConnectorName", tr("Test\nconnector"));
  widget->setHeaderData("TestContactName", tr("Test\ncontact"));
  widget->setHeaderData("DutUnitSchemaPosition", tr("DUT\nSchema pos."));
  widget->setHeaderData("DutUnitAlias", tr("DUT\nalias"));
  widget->setHeaderData("DutConnectorName", tr("DUT\nconnector"));
  widget->setHeaderData("DutContactName", tr("DUT\ncontact"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add link button
  pbAddTestLink = new QPushButton(tr("Add..."));
  pbAddTestLink->setIcon(QIcon::fromTheme("list-add"));
  connect(pbAddTestLink, SIGNAL(clicked()), this, SLOT(addTestLink()));
  widget->addWidgetToLocalBar(pbAddTestLink);
  // Remove links button
  pbRemoveTestLinks = new QPushButton(tr("Remove..."));
  pbRemoveTestLinks->setIcon(QIcon::fromTheme("list-remove"));
  connect(pbRemoveTestLinks, SIGNAL(clicked()), this, SLOT(removeTestLinks()));
  widget->addWidgetToLocalBar(pbRemoveTestLinks);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestModelItemEditor::setupTestNodeUnitSetupTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddNodeUnit;
  QPushButton *pbSetupNodeUnit;
  QPushButton *pbRemoveNodeUnits;
  ///QPushButton *pbGenerateTestNodeUnitSetup;
  ///QPushButton *pbRemoveTestNodeUnitSetup;

  if(!addChildTable("TestNodeUnitSetup_view", tr("Node unit setup"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestNodeUnitSetup_view", "TestModelItem_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestModelItem_Id_FK", true);
  widget->setColumnHidden("TestNodeUnit_Id_FK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  widget->setColumnHidden("IoPosition", true);
  widget->setColumnHidden("Type_Code_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("SchemaPosition", tr("Schema\npos."));
  widget->setHeaderData("NameEN", tr("Type (English)"));
  widget->setHeaderData("NameFR", tr("Type (Frensh)"));
  widget->setHeaderData("NameDE", tr("Type (German)"));
  widget->setHeaderData("NameIT", tr("Type (Italian)"));
  // Set some attributes on table view
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->sort();
  widget->tableView()->resizeColumnsToContents();
  // Add node unit button
  pbAddNodeUnit = new QPushButton(tr("Add unit ..."));
  pbAddNodeUnit->setIcon(QIcon::fromTheme("list-add"));
  connect(pbAddNodeUnit, SIGNAL(clicked()), this, SLOT(addNodeUnit()));
  widget->addWidgetToLocalBar(pbAddNodeUnit);
  // Setup unit button
  pbSetupNodeUnit = new QPushButton(tr("Setup unit ..."));
  connect(pbSetupNodeUnit, SIGNAL(clicked()), this, SLOT(setupNodeUnit()));
  widget->addWidgetToLocalBar(pbSetupNodeUnit);
  // Remove units button
  pbRemoveNodeUnits = new QPushButton(tr("Remove units ..."));
  pbRemoveNodeUnits->setIcon(QIcon::fromTheme("list-remove"));
  connect(pbRemoveNodeUnits, SIGNAL(clicked()), this, SLOT(removeNodeUnits()));
  widget->addWidgetToLocalBar(pbRemoveNodeUnits);
  widget->addStretchToLocalBar();

  
  // Add buttons
  /**
  pbGenerateTestNodeUnitSetup = new QPushButton(tr("Generate setup"));
  connect(pbGenerateTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(generateTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbGenerateTestNodeUnitSetup);
  pbRemoveTestNodeUnitSetup = new QPushButton(tr("Remove setup item"));
  connect(pbRemoveTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(removeTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbRemoveTestNodeUnitSetup);
  widget->addStretchToLocalBar();
  */

  return true;
}

