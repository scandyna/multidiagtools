/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
///#include "mdtTtRelayPathDialog.h"
#include "mdtClPathGraph.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtValueEditionDialog.h"
#include "mdtDoubleEdit.h"
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
#include <QLocale>

#include <QDebug>

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
  if(!setupTestItemRouteTable()){
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

void mdtTtTestModelItemEditor::editExpectedValueLimits()
{
  mdtTtValueEditionDialog dialog(this);
  auto tc = tableController<mdtAbstractSqlTableController>("TestModelItem_tbl");
  Q_ASSERT(tc);
  QVariant value;
  mdtTtValueLimits limits;
  QString unit;

  // Get current data
  value = currentExpectedValue();
  limits = currentValueLimits();
  unit = tc->currentData("ValueUnit").toString();
  // Setup and show dialog
  dialog.setValueVar(value);
  dialog.setLimits(limits);
  dialog.setUnit(unit);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update data
  tc->setCurrentData("ExpectedValue", dialog.valueVar(), false);
  limits = dialog.limits();
  tc->setCurrentData("LeftBottomLimit", limits.leftBottomLimitVar(), false);
  tc->setCurrentData("LeftTopLimit", limits.leftTopLimitVar(), false);
  tc->setCurrentData("RightBottomLimit", limits.rightBottomLimitVar(), false);
  tc->setCurrentData("RightTopLimit", limits.rightTopLimitVar(), false);
}

void mdtTtTestModelItemEditor::addTestLink()
{
  mdtTtTestModelItem tmi(0, database());
  QVariant testModelItemId;
  QVariant testModelId;
  QVariant testLinkId;
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  // Get test model item ID
  testModelItemId = currentData("TestModelItem_tbl", "Id_PK");
  if(testModelItemId.isNull()){
    return;
  }
  // Get test model ID
  testModelId = currentData("TestModelItem_tbl", "TestModel_Id_FK");
  if(testModelId.isNull()){
    return;
  }
  // Setup and show dialog for test link selection
  sql = tmi.sqlForTestLinkSelection(testModelItemId, testModelId);
  selectionDialog.setMessage(tr("Please select a test link to use."));
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("LogicalTestCable_Id_FK", true);
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
  selectionDialog.setWindowTitle(tr("Test link selection"));
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

void mdtTtTestModelItemEditor::addRoute()
{
//   mdtTtTestModelItem tmi(0, database());
//   mdtClPathGraph graph(database());
//   mdtTtRelayPathDialog dialog(database(), &graph, this);
//   mdtSqlSelectionDialog selectionDialog(this);
//   mdtSqlTableSelection s;
//   QVariant testModelId;
//   QVariant testModelItemId;
//   QVariant testLinkId;
//   QVariant measureCnxId;
//   QVariant testNodeId;
//   mdtTtTestNodeUnitSetupData setupData;
//   QList<mdtTtTestNodeUnitSetupData> setupDataList;
//   QList<QVariant> nodeUnitIdList;
//   QString sql;
//   int i;
// 
//   // Get test model ID
//   testModelId = currentData("TestModelItem_tbl", "TestModel_Id_FK");
//   if(testModelId.isNull()){
//     return;
//   }
//   // Get test model item ID
//   testModelItemId = currentData("TestModelItem_tbl", "Id_PK");
//   if(testModelItemId.isNull()){
//     return;
//   }
//   // Setup test node unit setup data
//   if(!setupData.setup(database())){
//     pvLastError = setupData.lastError();
//     displayLastError();
//     return;
//   }
//   setupData.setValue("TestModelItem_Id_FK", testModelItemId);
//   // Setup and show dialog for test node selection
//   sql = tmi.sqlForTestNodeSelection(testModelId);
//   selectionDialog.setMessage(tr("Please select a test node to use:"));
//   selectionDialog.setQuery(sql, database(), false);
//   selectionDialog.setColumnHidden("TestNode_Id_FK", true);
//   selectionDialog.setHeaderData("Type", tr("Test system"));
//   selectionDialog.setHeaderData("SubType", tr("Test node"));
//   selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
//   selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
//   selectionDialog.addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
//   selectionDialog.sort();
//   selectionDialog.setWindowTitle(tr("Test node selection"));
//   selectionDialog.resize(600, 300);
//   if(selectionDialog.exec() != QDialog::Accepted){
//     return;
//   }
//   s = selectionDialog.selection("TestNode_Id_FK");
//   if(s.isEmpty()){
//     return;
//   }
//   Q_ASSERT(s.rowCount() == 1);
//   testNodeId = s.data(0, "TestNode_Id_FK");
//   // Setup and show dialog
//   ///dialog.setTestNodeId(testNodeId);
//   dialog.setData(testModelItemId, testNodeId);
//   if(dialog.exec() != QDialog::Accepted){
//     return;
//   }
//   nodeUnitIdList = dialog.idListOfRelaysToEnable();
//   testLinkId = getTestLinkIdForTestConnectionId(testModelItemId, dialog.selectedTestConnection());
//   if(testLinkId.isNull()){
//     return;
//   }
//   measureCnxId = dialog.selectedMeasureConnection();
//   if(measureCnxId.isNull()){
//     return;
//   }
//   for(i = 0; i < nodeUnitIdList.size(); ++i){
//     setupData.setValue("TestNodeUnit_Id_FK", nodeUnitIdList.at(i));
//     setupData.setValue("StepNumber", 0);
//     setupData.setValue("State", true);
//     setupDataList.append(setupData);
//   }
//   ///testLinkId = ???
//   ///measureCnxId = ???
//   // Add to db
//   if(!tmi.addRoute(testModelItemId, testLinkId, measureCnxId, setupDataList, true)){
//     pvLastError = tmi.lastError();
//     displayLastError();
//     return;
//   }
//   // Update views
//   select("TestNodeUnitSetup_view");
//   select("TestModelItemRoute_view");
}

void mdtTtTestModelItemEditor::removeRoutes()
{
  mdtTtTestModelItem tmi(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;

  // Get widget and selection
  widget = sqlTableWidget("TestModelItemRoute_view");
  Q_ASSERT(widget != 0);
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove routes from current test item."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!tmi.removeRoutes(s)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestNodeUnitSetup_view");
  select("TestModelItemRoute_view");
}

void mdtTtTestModelItemEditor::addNodeUnit()
{
  mdtTtTestModelItem tmi(0, database());
  QVariant testModelId;
  QVariant testModelItemId;
  mdtTtTestNodeUnitSetupData data;
  mdtSqlSelectionDialog selectionDialog(this);
  QStringList fields;
  mdtSqlTableSelection s;
  QString sql;
  QString type;

  // Get test model ID
  testModelId = currentData("TestModelItem_tbl", "TestModel_Id_FK");
  if(testModelId.isNull()){
    return;
  }
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
  // Setup and show dialog for test node unit selection
  sql = tmi.sqlForTestNodeUnitSelection(testModelItemId, testModelId);
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
  selectionDialog.setWindowTitle(tr("Test node unit selection"));
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  fields << "Unit_Id_FK_PK" << "Type_Code_FK";
  s = selectionDialog.selection(fields);
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  data.setValue("TestNodeUnit_Id_FK", s.data(0, "Unit_Id_FK_PK"));
  data.setValue("StepNumber", 0);
  // Set state to ON if unit is a digital output
  type = s.data(0, "Type_Code_FK").toString();
  if((type == "BUSCPLREL")||(type == "CHANELREL")||(type == "DO")){
    data.setValue("State", true);
  }
  // Add to db
  if(!tmi.addOrUpdateTestNodeUnitSetup(data)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestNodeUnitSetup_view");
  select("TestModelItemRoute_view");
}

void mdtTtTestModelItemEditor::editRelayPath()
{
//   mdtTtTestModelItem tmi(0, database());
//   mdtClPathGraph graph(database());
//   mdtTtRelayPathDialog dialog(database(), &graph, this);
//   mdtSqlSelectionDialog selectionDialog(this);
//   mdtSqlTableSelection s;
//   QVariant testModelId;
//   QVariant testModelItemId;
//   QVariant testNodeId;
//   mdtTtTestNodeUnitSetupData setupData;
//   QList<mdtTtTestNodeUnitSetupData> setupDataList;
//   QList<QVariant> nodeUnitIdList;
//   QString sql;
//   int i;
// 
//   // Get test model ID
//   testModelId = currentData("TestModelItem_tbl", "TestModel_Id_FK");
//   if(testModelId.isNull()){
//     return;
//   }
//   // Get test model item ID
//   testModelItemId = currentData("TestModelItem_tbl", "Id_PK");
//   if(testModelItemId.isNull()){
//     return;
//   }
//   // Setup data
//   if(!setupData.setup(database())){
//     pvLastError = setupData.lastError();
//     displayLastError();
//     return;
//   }
//   setupData.setValue("TestModelItem_Id_FK", testModelItemId);
//   // Setup and show dialog for test node selection
//   sql = tmi.sqlForTestNodeSelection(testModelId);
//   selectionDialog.setMessage(tr("Please select a test node to use:"));
//   selectionDialog.setQuery(sql, database(), false);
//   selectionDialog.setColumnHidden("TestNode_Id_FK", true);
//   selectionDialog.setHeaderData("Type", tr("Test system"));
//   selectionDialog.setHeaderData("SubType", tr("Test node"));
//   selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
//   selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
//   selectionDialog.addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
//   selectionDialog.sort();
//   selectionDialog.setWindowTitle(tr("Test node selection"));
//   selectionDialog.resize(600, 300);
//   if(selectionDialog.exec() != QDialog::Accepted){
//     return;
//   }
//   s = selectionDialog.selection("TestNode_Id_FK");
//   if(s.isEmpty()){
//     return;
//   }
//   Q_ASSERT(s.rowCount() == 1);
//   testNodeId = s.data(0, "TestNode_Id_FK");
//   // Setup and show dialog
//   dialog.setTestNodeId(testNodeId);
//   if(dialog.exec() != QDialog::Accepted){
//     return;
//   }
//   nodeUnitIdList = dialog.idListOfRelaysToEnable();
//   for(i = 0; i < nodeUnitIdList.size(); ++i){
//     setupData.setValue("TestNodeUnit_Id_FK", nodeUnitIdList.at(i));
//     setupData.setValue("StepNumber", 0);
//     setupData.setValue("State", true);
//     setupDataList.append(setupData);
//   }
//   // Add to db
//   if(!tmi.addOrUpdateTestNodeUnitSetupList(setupDataList)){
//     pvLastError = tmi.lastError();
//     displayLastError();
//     return;
//   }
//   // Update views
//   select("TestNodeUnitSetup_view");
//   select("TestModelItemRoute_view");
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
  data = tmi.getTestNodeUnitSetupData(testModelItemId, testNodeUnitId, &ok);
  if(!ok){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  qDebug() << "Data: " << data;
  qDebug() << "I/O type: " << data.ioType();
  qDebug() << "I/O pos: " << data.ioPosition();
  qDebug() << "schema pos: " << data.schemaPosition();
  // Setup and show dialog
  dialog.setData(data);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update in db
  data = dialog.data();
  if(!tmi.updateTestNodeUnitData(testModelItemId, testNodeUnitId, data)){
    pvLastError = tmi.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("TestNodeUnitSetup_view");
  select("TestModelItemRoute_view");
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
  select("TestModelItemRoute_view");
}

QVariant mdtTtTestModelItemEditor::getTestLinkIdForTestConnectionId(const QVariant & testModelItemId, const QVariant & testConnectionId)
{
  mdtTtTestModelItem tmi(0, database());
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;

  if((testModelItemId.isNull())||(testConnectionId.isNull())){
    return QVariant();
  }
  sql = "SELECT Id_PK FROM TestLink_tbl TL JOIN TestModelItem_TestLink_tbl TMITL ON TMITL.TestLink_Id_FK = TL.Id_PK";
  sql += " WHERE TMITL.TestModelItem_Id_FK = " + testModelItemId.toString();
  sql += " AND TL.TestConnection_Id_FK = " + testConnectionId.toString();
  dataList = tmi.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = tmi.lastError();
    displayLastError();
    return QVariant();
  }
  if(dataList.isEmpty()){
    return QVariant();
  }
  qDebug() << "Test connection ID: " << testConnectionId << " , test link ID: " << dataList;
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0).value(0);
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

/**
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
*/

void mdtTtTestModelItemEditor::removeTestNodeUnitSetup()
{
  mdtSqlTableWidget *widget;
  mdtTtTestModelItem tmi(this, database());
  QMessageBox msgBox;
  ///QModelIndexList indexes;
  mdtSqlTableSelection s;

  widget = sqlTableWidget("TestModelItemNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  s = widget->currentSelection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  /**
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  */
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
  if(!tmi.removeTestNodeUnitSetups(s)){
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

QVariant mdtTtTestModelItemEditor::currentExpectedValue()
{
  ///return fld_ExpectedValue->value();
  return currentData("TestModelItem_tbl", "ExpectedValue");
}

mdtTtValueLimits mdtTtTestModelItemEditor::currentValueLimits()
{
  mdtTtValueLimits limits;
  auto tc = tableController<mdtAbstractSqlTableController>("TestModelItem_tbl");
  Q_ASSERT(tc);
  QVariant v;

  v = tc->currentData("LeftBottomLimit");
  limits.setLeftBottomLimitVar(v);
  v = tc->currentData("LeftTopLimit");
  limits.setLeftTopLimitVar(v);
  v = tc->currentData("RightBottomLimit");
  limits.setRightBottomLimitVar(v);
  v = tc->currentData("RightTopLimit");
  limits.setRightTopLimitVar(v);

  return limits;
}

bool mdtTtTestModelItemEditor::setupTestItemTable() 
{
  Ui::mdtTtTestModelItemEditor tie;

  // Setup main form widget
  ///tie.setupUi(mainSqlWidget());
  ///setMainTableUi<Ui::mdtTtTestModelItemEditor>();
  setMainTableUi(tie);
  // Setup form
  if(!setMainTable("TestModelItem_tbl", "Test item", database())){
    return false;
  }
  connect(tie.pbEditLimits, SIGNAL(clicked()), this, SLOT(editExpectedValueLimits()));

  return true;
}

bool mdtTtTestModelItemEditor::setupTestLinkTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddTestLink;
  QPushButton *pbRemoveTestLinks;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("TestModelItem_TestLink_view");
  relationInfo.addRelation("Id_PK", "TestModelItem_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Test links"))){
    return false;
  }
  widget = sqlTableWidget("TestModelItem_TestLink_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestModelItem_Id_FK", true);
  widget->setColumnHidden("TestNodeUnit_Id_FK", true);
  widget->setColumnHidden("TestLink_Id_FK", true);
  widget->setColumnHidden("TestConnection_Id_FK", true);
  widget->setColumnHidden("DutConnection_Id_FK", true);
  widget->setColumnHidden("Unit_Id_FK_PK", true);
  widget->setColumnHidden("DutUnitId", true);
  widget->setColumnHidden("VehicleType_Id_FK_PK", true);
  widget->setColumnHidden("TestNodeUnitSchemaPosition", true);
  widget->setColumnHidden("Value", true);
  widget->setColumnHidden("IoPosition", true);
  widget->setColumnHidden("TestNodeSeriesNumber", true);
  // Rename visible fields to something human friendly
  widget->setHeaderData("TestLinkIdentification", tr("Test link\nIdentification"));
  widget->setHeaderData("TestConnectorName", tr("Test\nconnector"));
  widget->setHeaderData("TestContactName", tr("Test\ncontact"));
  widget->setHeaderData("Alias", tr("Test system\nalias"));
  widget->setHeaderData("AddressString", tr("Test system\naddress string"));
  widget->setHeaderData("DutUnitSchemaPosition", tr("DUT\nSchema pos."));
  widget->setHeaderData("DutUnitAlias", tr("DUT\n   alias   "));
  widget->setHeaderData("DutConnectorName", tr("DUT\nconnector"));
  widget->setHeaderData("DutContactName", tr("DUT\ncontact"));
  widget->setHeaderData("TestNodeType", tr("Test\nsystem"));
  widget->setHeaderData("TestNodeSubType", tr("Test\nnode"));
  widget->setHeaderData("NodeIdentification", tr("Test\nnode\nidentification"));
  widget->setHeaderData("Type", tr("DUT\ntype"));
  widget->setHeaderData("SubType", tr("DUT\nsub type"));
  widget->setHeaderData("SeriesNumber", tr("DUT\ntype serie"));
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

bool mdtTtTestModelItemEditor::setupTestItemRouteTable()
{
  mdtSqlTableWidget *widget;
  QLocale locale;
  QPushButton *pb;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("TestModelItemRoute_view");
  relationInfo.addRelation("Id_PK", "TestModelItem_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Test node internal routes"))){
    return false;
  }
  widget = sqlTableWidget("TestModelItemRoute_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestModelItem_Id_FK", true);
  widget->setColumnHidden("TestLink_Id_FK", true);
  widget->setColumnHidden("MeasureTestNodeUnitConnection_Id_FK", true);
  widget->setColumnHidden("TestAlias", true);
  widget->setColumnHidden("TestConnection_Id_FK", true);
  widget->setColumnHidden("DutConnection_Id_FK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  // Choose MeasureContactFunction field regarding language
  switch(locale.language()){
    case QLocale::French:
      widget->setColumnHidden("MeasureContactFunctionEN", true);
      widget->setColumnHidden("MeasureContactFunctionDE", true);
      widget->setColumnHidden("MeasureContactFunctionIT", true);
      widget->setHeaderData("MeasureContactFunctionFR", tr("Measure\ncontact\nFunction"));
      break;
    case QLocale::German:
      widget->setColumnHidden("MeasureContactFunctionEN", true);
      widget->setColumnHidden("MeasureContactFunctionFR", true);
      widget->setColumnHidden("MeasureContactFunctionIT", true);
      widget->setHeaderData("MeasureContactFunctionDE", tr("Measure\ncontact\nFunction"));
      break;
    case QLocale::Italian:
      widget->setColumnHidden("MeasureContactFunctionEN", true);
      widget->setColumnHidden("MeasureContactFunctionDE", true);
      widget->setColumnHidden("MeasureContactFunctionFR", true);
      widget->setHeaderData("MeasureContactFunctionIT", tr("Measure\ncontact\nFunction"));
      break;
    default:
      widget->setColumnHidden("MeasureContactFunctionEN", true);
      widget->setColumnHidden("MeasureContactFunctionDE", true);
      widget->setColumnHidden("MeasureContactFunctionIT", true);
      widget->setHeaderData("MeasureContactFunctionEN", tr("Measure\ncontact\nFunction"));
  }
  // Choose MeasureContactFunction field regarding language
  switch(locale.language()){
    case QLocale::French:
      widget->setColumnHidden("TestContactFunctionDE", true);
      widget->setColumnHidden("TestContactFunctionEN", true);
      widget->setColumnHidden("TestContactFunctionIT", true);
      widget->setHeaderData("TestContactFunctionFR", tr("Test\ncontact\nFunction"));
      break;
    case QLocale::German:
      widget->setColumnHidden("TestContactFunctionFR", true);
      widget->setColumnHidden("TestContactFunctionEN", true);
      widget->setColumnHidden("TestContactFunctionIT", true);
      widget->setHeaderData("TestContactFunctionDE", tr("Test\ncontact\nFunction"));
      break;
    case QLocale::Italian:
      widget->setColumnHidden("TestContactFunctionDE", true);
      widget->setColumnHidden("TestContactFunctionEN", true);
      widget->setColumnHidden("TestContactFunctionFR", true);
      widget->setHeaderData("TestContactFunctionIT", tr("Test\ncontact\nFunction"));
      break;
    default:
      widget->setColumnHidden("TestContactFunctionDE", true);
      widget->setColumnHidden("TestContactFunctionFR", true);
      widget->setColumnHidden("TestContactFunctionIT", true);
      widget->setHeaderData("TestContactFunctionEN", tr("Test\ncontact\nFunction"));
  }
  // Rename other visible fields to something human friendly
  widget->setHeaderData("Identification", tr("Test link\nidentification"));
  widget->setHeaderData("TestSchemaPosition", tr("Test\nConnector"));
  widget->setHeaderData("TestContactName", tr("Test\nContact"));
  widget->setHeaderData("RouteTestNodeUnits", tr("Route\nrelays"));
  widget->setHeaderData("RouteResistance", tr("Route\nresistance"));
  widget->setHeaderData("MeasureSchemaPosition", tr("Measure\nunit"));
  widget->setHeaderData("MeasureAlias", tr("Measure\nalias"));
  widget->setHeaderData("MeasureContactName", tr("Measure\ncontact"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add route button
  pb = new QPushButton(tr("Add route ..."));
  pb->setIcon(QIcon::fromTheme("list-add"));
  connect(pb, SIGNAL(clicked()), this, SLOT(addRoute()));
  widget->addWidgetToLocalBar(pb);
  // Remove route button
  pb = new QPushButton(tr("Remove routes ..."));
  pb->setIcon(QIcon::fromTheme("list-remove"));
  connect(pb, SIGNAL(clicked()), this, SLOT(removeRoutes()));
  widget->addWidgetToLocalBar(pb);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestModelItemEditor::setupTestNodeUnitSetupTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddNodeUnit;
  QPushButton *pbEditRelayPath;
  QPushButton *pbSetupNodeUnit;
  QPushButton *pbRemoveNodeUnits;
  QLocale locale;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("TestNodeUnitSetup_view");
  relationInfo.addRelation("Id_PK", "TestModelItem_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Node unit setup"))){
    return false;
  }
  widget = sqlTableWidget("TestNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestModelItem_Id_FK", true);
  widget->setColumnHidden("TestModelItemRoute_Id_FK", true);
  widget->setColumnHidden("TestNodeUnit_Id_FK", true);
  widget->setColumnHidden("TestNode_Id_FK", true);
  widget->setColumnHidden("IoPosition", true);
  widget->setColumnHidden("Type_Code_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("StepNumber", tr("Step #"));
  widget->setHeaderData("SchemaPosition", tr("Test node unit\nschema pos."));
  widget->setHeaderData("Alias", tr("Test node unit\nalias"));
  widget->setHeaderData("Type", tr("Test node unit\ntype"));
  // Choose Name field regarding language
  switch(locale.language()){
    case QLocale::French:
      widget->setColumnHidden("NameEN", true);
      widget->setColumnHidden("NameDE", true);
      widget->setColumnHidden("NameIT", true);
      widget->setHeaderData("NameFR", tr("                 Type                 "));
      break;
    case QLocale::German:
      widget->setColumnHidden("NameEN", true);
      widget->setColumnHidden("NameFR", true);
      widget->setColumnHidden("NameIT", true);
      widget->setHeaderData("NameDE", tr("               Type               "));
      break;
    case QLocale::Italian:
      widget->setColumnHidden("NameEN", true);
      widget->setColumnHidden("NameDE", true);
      widget->setColumnHidden("NameFR", true);
      widget->setHeaderData("NameIT", tr("               Type               "));
      break;
    default:
      widget->setColumnHidden("NameFR", true);
      widget->setColumnHidden("NameDE", true);
      widget->setColumnHidden("NameIT", true);
      widget->setHeaderData("NameEN", tr("               Type               "));
  }
  // Set some attributes on table view
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->sort();
  widget->tableView()->resizeColumnsToContents();
  // Add node unit button
  pbAddNodeUnit = new QPushButton(tr("Add unit ..."));
  pbAddNodeUnit->setIcon(QIcon::fromTheme("list-add"));
  connect(pbAddNodeUnit, SIGNAL(clicked()), this, SLOT(addNodeUnit()));
  widget->addWidgetToLocalBar(pbAddNodeUnit);
  // Edit relay path button
  pbEditRelayPath = new QPushButton(tr("Edit relay path ..."));
  ///pbEditRelayPath->setIcon(QIcon::fromTheme("list-add"));
  connect(pbEditRelayPath, SIGNAL(clicked()), this, SLOT(editRelayPath()));
  widget->addWidgetToLocalBar(pbEditRelayPath);
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

  return true;
}

