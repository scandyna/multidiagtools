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
#include "mdtSqlFormWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtSqlDialog.h"
#include "mdtClPathGraph.h"
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

mdtTtTestModelEditor::mdtTtTestModelEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestModelEditor::setupTables() 
{
  if(!setupTestTable()){
    return false;
  }
  if(!setupTestNodeTable()){
    return false;
  }
  if(!setupTestItemTable()){
    return false;
  }
  /**
  if(!setupTestNodeUnitSetupTable()){
    return false;
  }
  */
  return true;
}

void mdtTtTestModelEditor::addTestNode()
{
  mdtTtTestModel tm(0, database());
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
  QVariant testModelId;
  QVariant testNodeId;
  QString sql;

  // Get current test model ID
  testModelId = currentData("TestModel_tbl", "Id_PK");
  if(testModelId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = tm.sqlForTestNodeSelection(testModelId);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select a test node:"));
  selectionDialog.resize(800, 600);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("VehicleType_Id_FK_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  testNodeId = s.data(0, "VehicleType_Id_FK_PK");
  // Add test node
  if(!tm.addTestNode(testNodeId, testModelId)){
    pvLastError = tm.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestModel_TestNode_view");
}

void mdtTtTestModelEditor::removeTestNodes()
{
  mdtTtTestModel tm(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;
  QStringList fields;

  // Get widget and selection
  widget = sqlTableWidget("TestModel_TestNode_view");
  Q_ASSERT(widget != 0);
  fields << "TestNode_Id_FK" << "TestModel_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove assignations between selected test node and current test model."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected nodes
  if(!tm.removeTestNodes(s)){
    pvLastError = tm.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestModel_TestNode_view");
}

/**
void mdtTtTestModelEditor::addTestItem() 
{
  mdtTtTestModel tm(this, database());
  QVariant testId;
  QVariant testCableId;
  QVariant testLinkBusAId;
  QVariant testLinkBusBId;
  QVariant expectedValue;

  // Get current test ID
  testId = currentData("TestModel_tbl", "Id_PK");
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
  if(!tm.addTestItem(testId, testLinkBusAId, testLinkBusBId, expectedValue)){
    pvLastError = tm.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  select("TestModelItem_tbl");
  select("TestModelItemNode_view");
  // Generate setup
  generateTestNodeUnitSetupList();
}
*/


/**
void mdtTtTestModelEditor::addTestItem()
{
  mdtTtTestModelItemDialog dialog(0, database());
  
  if(!dialog.setTestModelItemId(QVariant())){
    return;
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
}
*/

void mdtTtTestModelEditor::addTestItem()
{
  mdtTtTestModelItemEditor *tie;
  mdtTtTestModel tm(0, database());
  mdtSqlDialog dialog;
  QVariant testModelId;
  QVariant seqNumber;

  // Get current test model ID
  testModelId = currentData("TestModel_tbl", "Id_PK");
  if(testModelId.isNull()){
    return;
  }
  // Get next sequence number
  seqNumber = tm.getNextSequenceNumber(testModelId);
  if(seqNumber.isNull()){
    return;
  }
  // Create and setup item editors and dialog
  tie = new mdtTtTestModelItemEditor(0, database());
  if(!tie->setupTables()){
    pvLastError = tie->lastError();
    displayLastError();
    return;
  }
  dialog.setSqlForm(tie);
  dialog.resize(900, 500);
  dialog.enableEdition();
  dialog.setWindowTitle(tr("Test item edition"));
  tie->insert();
  tie->setTestModelId(testModelId);
  tie->setSequenceNumber(seqNumber);
  dialog.exec();
  // Update views
  select("TestModelItem_tbl");
}

void mdtTtTestModelEditor::editTestItem()
{
  mdtTtTestModelItemEditor *tie;
  mdtSqlDialog dialog;
  QVariant currentTestItemId;

  currentTestItemId = currentData("TestModelItem_tbl", "Id_PK");
  if(currentTestItemId.isNull()){
    return;
  }
  tie = new mdtTtTestModelItemEditor(0, database());
  if(!tie->setupTables()){
    pvLastError = tie->lastError();
    displayLastError();
    return;
  }
  if(!tie->setMainTableFilter("Id_PK", currentTestItemId)){
    pvLastError = tie->lastError();
    displayLastError();
    delete tie;
    return;
  }
  dialog.setSqlForm(tie);
  dialog.resize(900, 500);
  dialog.enableEdition();
  dialog.setWindowTitle(tr("Test item edition"));
  dialog.exec();
  // Update connections table
  select("TestModelItem_tbl");
  ///select("TestModelItemNode_view");
  ///select("TestModelItemNodeUnitSetup_view");
}

void mdtTtTestModelEditor::removeTestItem() 
{
  mdtSqlTableWidget *widget;
  mdtTtTestModel tm(this, database());
  QMessageBox msgBox;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("TestModelItem_tbl");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
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
  if(!tm.removeTestModelItems(s, true, true)){
  ///if(!tm.removeTestItems(indexes)){
  ///if(!tm.removeData("TestModelItem_tbl", "Id_PK", indexes)){
    pvLastError = tm.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  select("TestModelItem_tbl");
  ///select("TestModelItemNode_view");
}

void mdtTtTestModelEditor::generateContinuityTest()
{
  mdtTtTestModel tm(this, database());
  mdtClPathGraph graph(database());
  QVariant testModelId;
  QVariant testCableId;
  QVariant testNodeId;
  QVariant measureConnexionIdA;
  QVariant measureConnexionIdB;
  QList<QVariant> relaysToIgnore;
  QString sql;
  mdtSqlSelectionDialog dialog(this);
  mdtSqlTableSelection s;
  int i;

  // Get current test ID
  testModelId = currentData("TestModel_tbl", "Id_PK");
  if(testModelId.isNull()){
    return;
  }
  // Load link list
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    displayLastError();
    return;
  }
  // Select a cable
  testCableId = selectTestCable();
  if(testCableId.isNull()){
    return;
  }
  // Get current test node
  /// \todo Not good..
  testNodeId = currentData("TestModel_TestNode_view", "TestNode_Id_FK");
  if(testNodeId.isNull()){
    return;
  }
  // Select measure connections
  measureConnexionIdA = selectMeasureConnection(testNodeId);
  if(measureConnexionIdA.isNull()){
    return;
  }
  measureConnexionIdB = selectMeasureConnection(testNodeId);
  if(measureConnexionIdB.isNull()){
    return;
  }
  // Select relays to not use
  sql = "SELECT * FROM TestNodeUnit_view";
  sql += " WHERE TestNode_Id_FK = " + testNodeId.toString();
  sql += " AND (Type_Code_FK = 'BUSCPLREL')";
  dialog.setQuery(sql, database(), true);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  s = dialog.selection("Unit_Id_FK_PK");
  for(i = 0; i < s.rowCount(); ++i){
    relaysToIgnore.append(s.data(i, "Unit_Id_FK_PK"));
  }

  ///if(!tm.generateContinuityTest(testModelId, testCableId, testNodeId, measureConnexionIdA, measureConnexionIdB, graph)){
  if(!tm.generateShortDetectionTest(testModelId, testCableId, testNodeId, measureConnexionIdA, measureConnexionIdB, graph, relaysToIgnore)){
    pvLastError = tm.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestModelItem_tbl");
}

void mdtTtTestModelEditor::generateTestNodeUnitSetupList()
{
  mdtTtTestModel tm(this, database());
  QVariant testId;
  QList<QVariant> setupIdList;

  // Get current test ID
  testId = currentData("TestModel_tbl", "Id_PK");
  if(testId.isNull()){
    return;
  }
  // Warn user if some setup allready exists
  setupIdList = tm.getTestNodeUnitSetupIdList(testId);
  if(!setupIdList.isEmpty()){
    QString text;
    QMessageBox msgBox;
    text = tr("Setups will be generated for current testm. ");
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
  if(!tm.generateTestNodeUnitSetup(testId)){
    pvLastError = tm.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  select("TestModelItemNodeUnitSetup_view");
}

void mdtTtTestModelEditor::removeTestNodeUnitSetup()
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

QVariant mdtTtTestModelEditor::selectTestCable()
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM TestCable_tbl";
  /**
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
  */
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select test cable to use."));
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
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

QVariant mdtTtTestModelEditor::selectMeasureConnection(const QVariant & testNodeId)
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  mdtSqlTableSelection s;
 
  sql = "SELECT UCNX.Id_PK, UCNX.UnitContactName ";
  sql += "FROM TestNodeUnit_tbl TNU JOIN Unit_tbl U ON U.Id_PK = TNU.Unit_Id_FK_PK ";
  sql += " JOIN UnitConnection_tbl UCNX ON UCNX.Unit_Id_FK = TNU.Unit_Id_FK_PK ";
  sql += " WHERE TNU.Type_Code_FK = 'MEASCONNECTOR' ";
  sql += " AND TNU.TestNode_Id_FK = " + testNodeId.toString();
  selectionDialog.setMessage(tr("Please select a measure connection to use:"));
  selectionDialog.setQuery(sql, database(), false);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  s = selectionDialog.selection("Id_PK");
  if(s.isEmpty()){
    return QVariant();
  }
  Q_ASSERT(s.rowCount() == 1);

  return s.data(0, "Id_PK");
}

QVariant mdtTtTestModelEditor::selectTestLink(const QVariant & cableId, const QString & bus)
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM TestLink_view WHERE TestCable_Id_FK = " + cableId.toString() + " AND Bus = '" + bus + "'";
  /**
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

bool mdtTtTestModelEditor::setupTestTable()
{
  Ui::mdtTtTestModelEditor te;

  // Setup main form widget
  te.setupUi(mainSqlWidget());
  ///connect(te.pbGenerateNodeUnitSetup, SIGNAL(clicked()), this, SLOT(generateTestNodeUnitSetupList()));
  connect(te.pbGenerateNodeUnitSetup, SIGNAL(clicked()), this, SLOT(generateContinuityTest()));
  // Setup form
  if(!setMainTable("TestModel_tbl", "Test", database())){
    return false;
  }
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestModelEditor::setupTestNodeTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddTestNode;
  QPushButton *pbRemodeTestNode;

  if(!addChildTable("TestModel_TestNode_view", tr("Test nodes"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestModel_TestNode_view", "TestModel_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestModel_TestNode_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbAddTestNode = new QPushButton(tr("Add ..."));
  pbAddTestNode->setIcon(QIcon::fromTheme("list-add"));
  widget->addWidgetToLocalBar(pbAddTestNode);
  connect(pbAddTestNode, SIGNAL(clicked()), this, SLOT(addTestNode()));
  pbRemodeTestNode = new QPushButton(tr("Remove ..."));
  pbRemodeTestNode->setIcon(QIcon::fromTheme("list-remove"));
  widget->addWidgetToLocalBar(pbRemodeTestNode);
  connect(pbRemodeTestNode, SIGNAL(clicked()), this, SLOT(removeTestNodes()));
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestModelEditor::setupTestItemTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddTestItem;
  QPushButton *pbEditTestItem;
  QPushButton *pbRemoveTestItem;

  if(!addChildTable("TestModelItem_tbl", tr("Test items"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestModelItem_tbl", "TestModel_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestModelItem_tbl");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestModel_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("SequenceNumber", tr("Seq. #"));
  widget->setHeaderData("ExpectedValue", tr("Value\nExpected"));
  // Set some attributes on table view
  widget->addColumnToSortOrder("SequenceNumber", Qt::AscendingOrder);
  widget->sort();
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

/**
bool mdtTtTestModelEditor::setupTestNodeUnitSetupTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbRemoveTestNodeUnitSetup;

  if(!addChildTable("TestModelItemNodeUnitSetup_view", tr("Node unit setup"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestModelItemNodeUnitSetup_view", "TestModel_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestModelItemNodeUnitSetup_view");
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
*/
