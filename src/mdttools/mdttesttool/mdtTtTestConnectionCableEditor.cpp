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
#include "mdtTtTestConnectionCableEditor.h"
#include "ui_mdtTtTestConnectionCableEditor.h"
#include "mdtTtTestConnectionCable.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtTtTestNode.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include <QSqlQueryModel>
#include <QTableView>
#include <QSqlError>
#include <QString>
#include <QPushButton>
#include <QModelIndex>

#include <QDebug>

mdtTtTestConnectionCableEditor::mdtTtTestConnectionCableEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestConnectionCableEditor::setupTables()
{
  if(!setupTestCableTable()){
    return false;
  }
  if(!setupTestLinkTable()){
    return false;
  }
  return true;
}

void mdtTtTestConnectionCableEditor::generateLinks()
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtTtTestNode tn(this, database());
  QVariant cableId;
  QVariant dutVehicleId;
  QVariant dutUnitId;
  QVariant testNodeId;
  QVariant dutStartConnectorId;
  QList<QVariant> dutStartConnectionIdList;
  QList<QVariant> dutLinkedConnectorIdList;
  QList<QVariant> dutEndConnectorIdList;
  QList<QVariant> dutEndConnectionIdList;
  QList<QVariant> busAtestConnectionIdList;
  QList<QVariant> busBtestConnectionIdList;
  QList<QVariant> idList;
  int i, k;

  // Get cable ID
  cableId = currentData("TestCable_tbl", "Id_PK");
  if(cableId.isNull()){
    return;
  }
  // Let user choose DUT unit and test node
  dutVehicleId = selectDutVehicleId();
  if(dutVehicleId.isNull()){
    return;
  }
  dutUnitId = selectDutUnitId(dutVehicleId);
  if(dutUnitId.isNull()){
    return;
  }
  testNodeId = selectTestNode();
  if(testNodeId.isNull()){
    return;
  }
  // Get test node's BUS channel test connections
  busAtestConnectionIdList = tn.getChannelTestConnectionIdList(testNodeId, "BUSA");
  if(busAtestConnectionIdList.isEmpty()){
    qDebug() << "There is no channel test connection available on BUSA";
    return;
  }
  busBtestConnectionIdList = tn.getChannelTestConnectionIdList(testNodeId, "BUSB");
  if(busBtestConnectionIdList.isEmpty()){
    qDebug() << "There is no channel test connection available on BUSB";
    return;
  }
  qDebug() << "BUSA cnn: " << busAtestConnectionIdList;
  qDebug() << "BUSB cnn: " << busBtestConnectionIdList;
  // Select a start connector
  dutStartConnectorId = selectStartConnectorId(dutUnitId);
  if(dutStartConnectorId.isNull()){
    return;
  }
  qDebug() << "Selected DUT: " << dutUnitId << " , Test node: " << testNodeId;
  // Load link list
  if(!tcc.loadLinkList()){
    pvLastError = tcc.lastError();
    return;
  }
  // Get start unit connections for selected connector
  dutStartConnectionIdList = tcc.getToUnitConnectorRelatedUnitConnectionIdList(dutStartConnectorId);
  qDebug() << "Start connections: " << dutStartConnectionIdList;
  for(i = 0; i < dutStartConnectionIdList.size(); ++i){
    // Get list of unit connectors that is linked to selected start unit connection
    idList = tcc.getToUnitConnectionLinkedUnitConnectorIdList(dutStartConnectionIdList.at(i));
    for(k = 0; k < idList.size(); ++k){
      if(!dutLinkedConnectorIdList.contains(idList.at(k))){
        dutLinkedConnectorIdList.append(idList.at(k));
      }
    }
  }
  // Select end connector(s)
  dutEndConnectorIdList = selectEndConnectorIdList(dutLinkedConnectorIdList);
  qDebug() << "End connectors: " << dutEndConnectorIdList;
  // Get end connections that are part of selected end connectors
  dutEndConnectionIdList = tcc.getToUnitConnectionIdListLinkedUnitConnectionIdListPartOfUnitConnectorList(dutStartConnectionIdList, dutEndConnectorIdList);
  qDebug() << "Start connections: " << dutStartConnectionIdList;
  qDebug() << "End connections: " << dutEndConnectionIdList;
  // Check that test node has needed amount of busses lines
  if(busAtestConnectionIdList.size() < dutStartConnectionIdList.size()){
    qDebug() << "Test node has to less BUSA test connections";
    return;
  }
  if(busBtestConnectionIdList.size() < dutEndConnectionIdList.size()){
    qDebug() << "Test node has to less BUSB test connections";
    return;
  }
  // Add links to link table
  /**
  if(!tcc.createTestCable(testNodeId, busAtestConnectionIdList, dutStartConnectionIdList, busBtestConnectionIdList, dutEndConnectionIdList)){
    return;
  }
  */
  if(!tcc.addLinks(testNodeId, cableId, busAtestConnectionIdList, dutStartConnectionIdList)){
    pvLastError = tcc.lastError();
    return;
  }
  if(!tcc.addLinks(testNodeId, cableId, busBtestConnectionIdList, dutEndConnectionIdList)){
    pvLastError = tcc.lastError();
    return;
  }
  // Update GUI
  select("TestLink_view");
}

void mdtTtTestConnectionCableEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtTtTestConnectionCable tcc(this, database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = sqlTableWidget("TestLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove selected links."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!tcc.removeData("TestLink_tbl", "Id_PK", indexes)){
    pvLastError = tcc.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  select("TestLink_view");
}

/**
void mdtTtTestConnectionCableEditor::connectTestCable()
{
  mdtTtTestConnectionCable tcc(this, database());
  QVariant testCableId;
  QVariant dutVehicleId;
  QVariant testNodeId;

  // Let user choose a test cable to use
  testCableId = selectTestCable();
  if(testCableId.isNull()){
    return;
  }
  qDebug() << "Selected test cable: " << testCableId;
  // Let user choose test node
  testNodeId = selectTestNode();
  if(testNodeId.isNull()){
    return;
  }
  // Let user choose DUT vehiclke type
  dutVehicleId = selectDutVehicleId();
  if(dutVehicleId.isNull()){
    return;
  }
  // Connect test cable
  if(!tcc.connectTestCable(testCableId, testNodeId, dutVehicleId)){
    return;
  }
}
*/

/**
void mdtTtTestConnectionCableEditor::disconnectTestCable()
{
  mdtTtTestConnectionCable tcc(this, database());
  QVariant testCableId;

  // Let user choose a test cable to disconnect
  testCableId = selectTestCable();
  if(testCableId.isNull()){
    return;
  }
  // Connect test cable
  if(!tcc.disconnectTestCable(testCableId)){
    return;
  }
}
*/

QVariant mdtTtTestConnectionCableEditor::selectDutVehicleId()
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available connectors
  sql = "SELECT * FROM VehicleType_tbl ";
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get vhicle type list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select vehicle type that contains unit to test.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setColumnHidden("VehicleType_Id_FK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(750, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectDutUnitId(const QVariant & vehicleId)
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available connectors
  sql = "SELECT * FROM VehicleType_Unit_view WHERE VehicleType_Id_FK = " + vehicleId.toString();
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get unit list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select unit to test.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("VehicleType_Id_FK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Unit_Id_FK");
  selectionDialog.resize(750, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectTestNode()
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available test nodes
  ///sql = "SELECT * FROM TestNode_tbl ";
  sql = tcc.sqlForTestNodeSelection();
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get test node list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select test node to use.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("VehicleType_Id_FK_PK", true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setHeaderData("SubType", tr("Variant"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("VehicleType_Id_FK_PK");
  selectionDialog.resize(600, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectTestCable()
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available test nodes
  sql = tcc.sqlForTestCableSelection();
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get test cable list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select test cable to use.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("", tr(""));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(600, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QVariant mdtTtTestConnectionCableEditor::selectStartConnectorId(const QVariant & dutUnitId) 
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model to show available connectors
  sql = tcc.sqlForStartConnectorSelection(dutUnitId);
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get unit connector list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select a connector to test.");
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

QList<QVariant> mdtTtTestConnectionCableEditor::selectEndConnectorIdList(const QList<QVariant> & unitConnectorIdList) 
{
  mdtTtTestConnectionCable tcc(this, database());
  mdtSqlSelectionDialog selectionDialog;
  QModelIndexList selectedItems;
  QList<QVariant> idList;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;
  int i;

  // Setup model to show available connectors
  sql = tcc.sqlForUnitConnectorSelectionFromUnitConnectorIdList(unitConnectorIdList);
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get unit connector list."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCableEditor");
    pvLastError.commit();
    ///displayLastError();
    return idList;
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage("Please select end connector(s) to test.");
  ///selectionDialog.setModel(&model, true);
  selectionDialog.setQuery(sql, database(), true);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return idList;
  }
  selectedItems = selectionDialog.selectionResults();
  for(i = 0; i < selectedItems.size(); ++i){
    idList.append(selectedItems.at(i).data());
  }

  return idList;  
}

bool mdtTtTestConnectionCableEditor::setupTestCableTable()
{
  Ui::mdtTtTestConnectionCableEditor tcce;

  // Setup main form widget
  tcce.setupUi(mainSqlWidget());
  // Setup form
  if(!setMainTable("TestCable_tbl", "Test cable", database())){
    return false;
  }
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestConnectionCableEditor::setupTestLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbGenerateLinks;
  QPushButton *pbRemoveLinks;

  if(!addChildTable("TestLink_view", tr("Links"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "TestLink_view", "TestCable_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestLink_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestCable_Id_FK", true);
  widget->setColumnHidden("TestConnection_Id_FK", true);
  widget->setColumnHidden("DutConnection_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("TestNodeType", tr("Test node\nType"));
  widget->setHeaderData("TestNodeSubType", tr("Test node\nSub type"));
  widget->setHeaderData("TestNodeSeriesNumber", tr("Test node\nSerie"));
  widget->setHeaderData("NodeId", tr("Test node\nNode ID"));
  widget->setHeaderData("TestNodeUnitSchemaPosition", tr("Test node\nUnit"));
  widget->setHeaderData("TestConnectorName", tr("Test\nConnector"));
  widget->setHeaderData("TestContactName", tr("Test\nContact"));
  widget->setHeaderData("TestLinkIdentification", tr("Identification"));
  widget->setHeaderData("TestLinkValue", tr("R [Ohm]"));
  widget->setHeaderData("DutUnitSchemaPosition", tr("DUT"));
  widget->setHeaderData("DutConnectorName", tr("DUT\nConnector"));
  widget->setHeaderData("DutContactName", tr("DUT\nContact"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbGenerateLinks = new QPushButton(tr("Generate links ..."));
  connect(pbGenerateLinks, SIGNAL(clicked()), this, SLOT(generateLinks()));
  widget->addWidgetToLocalBar(pbGenerateLinks);
  pbRemoveLinks = new QPushButton(tr("Remove links ..."));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
  widget->addWidgetToLocalBar(pbRemoveLinks);
  widget->addStretchToLocalBar();

  return true;
}
