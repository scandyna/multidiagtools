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
#include "mdtTtTestLinkDialog.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtClPathGraph.h"
#include "mdtClLinkedUnitConnectionInfoDialog.h"
#include <QMessageBox>
#include <QSqlRecord>
#include <QList>
#include <QStringList>

mdtTtTestLinkDialog::mdtTtTestLinkDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;
  // Setup link data
  if(!pvLinkData.setup(pvDatabase)){
    QMessageBox msgBox;
    msgBox.setText(pvLinkData.lastError().text());
    msgBox.setDetailedText(pvLinkData.lastError().systemText());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Test link data setup error"));
    msgBox.exec();
    reject();
    return;
  }
  // Setup UI
  setupUi(this);
  connect(pbSelectTestUnit, SIGNAL(clicked()), this, SLOT(selectTestNode()));
  connect(pbSelectDutUnit, SIGNAL(clicked()), this, SLOT(selectDutUnit()));
  connect(pbSelectTestConnection, SIGNAL(clicked()), this, SLOT(selectTestConnection()));
  connect(pbSelectDutConnection, SIGNAL(clicked()), this, SLOT(selectDutConnection()));
  connect(pbLinkedConnectionsInfo, SIGNAL(clicked()), this, SLOT(viewDutLinkedConnections()));
  displayTestNode();
  displayDutUnit();
  displayTestConnection();
  displayDutConnection();
}

mdtTtTestLinkDialog::~mdtTtTestLinkDialog() 
{
}

void mdtTtTestLinkDialog::setTestNode(const QVariant & nodeId) 
{
  pvTestNodeId = nodeId;
  displayTestNode();
}

void mdtTtTestLinkDialog::setDutUnit(const QVariant & unitId) 
{
  pvDutUnitId = unitId;
  displayDutUnit();
}

void mdtTtTestLinkDialog::setTestConnection(const QVariant & unitConnectionId) 
{
  pvLinkData.setValue("TestConnection_Id_FK", unitConnectionId);
  displayTestConnection();
}

void mdtTtTestLinkDialog::setDutConnection(const QVariant & unitConnectionId) 
{
  pvLinkData.setValue("DutConnection_Id_FK", unitConnectionId);
  displayDutConnection();
}

void mdtTtTestLinkDialog::setLinkData(const mdtTtTestLinkData & data)
{
  pvLinkData = data;
  displayTestConnection();
  displayDutConnection();
}

void mdtTtTestLinkDialog::selectTestNode()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QList<QVariant> result;

  // Setup and run query
  /**
  sql = "SELECT * FROM Unit_view ";
  if(!pvTestNodeId.isNull()){
    sql += " WHERE Unit_Id_PK <> " + pvTestNodeId.toString();
  }
  */
  sql = "SELECT VehicleType_Id_FK_PK, Type, SubType, SeriesNumber, NodeId";
  sql += " FROM TestNode_tbl";
  sql += " JOIN VehicleType_tbl";
  sql += "  ON VehicleType_tbl.Id_PK = TestNode_tbl.VehicleType_Id_FK_PK";
  if(!pvTestNodeId.isNull()){
    sql += " WHERE VehicleType_Id_FK_PK <> " + pvTestNodeId.toString();
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select test node:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("VehicleType_Id_FK_PK", true);
  selectionDialog.setHeaderData("Type", tr("Type"));
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("NodeId", tr("Node ID"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("VehicleType_Id_FK_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 1);
  // Store unit and update GUI
  setTestConnection(QVariant());
  setTestNode(result.at(0));
}

void mdtTtTestLinkDialog::selectDutUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QList<QVariant> result;

  // Setup and run query
  sql = "SELECT * FROM Unit_view ";
  if(!pvDutUnitId.isNull()){
    sql += " WHERE Unit_Id_PK <> " + pvDutUnitId.toString();
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select DUT unit:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setHeaderData("Vehicle", tr("Type"));
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article code"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation (ENG)"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Unit_Id_PK");
  selectionDialog.resize(800, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 1);
  // Store unit and update GUI
  setDutConnection(QVariant());
  setDutUnit(result.at(0));
}

void mdtTtTestLinkDialog::selectTestConnection() 
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QList<QVariant> result;
  mdtClUnit unit(this, pvDatabase);

  /**
  if(pvTestUnitId.isNull()){
    return;
  }
  */
  // Setup and run query
  sql = "SELECT TestNode_Id_FK, TestConnection_Id_FK, UnitConnectorName, UnitContactName, Bus, SchemaPosition, NameEN, NameFR, NameDE, NameIT, IoPosition";
  sql += " FROM TestNodeUnit_view";
  
  // Setup and show dialog
  selectionDialog.setMessage("Please select test box connection:");
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("TestNode_Id_FK", true);
  selectionDialog.setColumnHidden("TestConnection_Id_FK", true);
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema\nposition"));
  selectionDialog.setHeaderData("UnitConnectorName", tr("Connector"));
  selectionDialog.setHeaderData("UnitContactName", tr("Contact"));
  selectionDialog.setHeaderData("NameEN", tr("Node unit type\n(English)"));
  selectionDialog.setHeaderData("NameFR", tr("Node unit type\n(Frensh)"));
  selectionDialog.setHeaderData("NameDE", tr("Node unit type\n(German)"));
  selectionDialog.setHeaderData("NameIT", tr("Node unit type\n(Italian)"));
  selectionDialog.addColumnToSortOrder("Bus", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("TestConnection_Id_FK");
  selectionDialog.resize(700, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 1);
  // Get connection data and update
  pvLinkData.setValue("TestConnection_Id_FK", result.at(0));
  displayTestConnection();
}

void mdtTtTestLinkDialog::selectDutConnection() 
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QList<QVariant> result;
  mdtClUnit unit(this, pvDatabase);

  if(pvDutUnitId.isNull()){
    return;
  }
  // Setup and run query
  sql = "SELECT * FROM UnitConnection_view WHERE Unit_Id_FK = " + pvDutUnitId.toString();
  // Setup and show dialog
  selectionDialog.setMessage("Please select DUT connection:");
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("UnitConnection_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnection_Id_FK", true);
  selectionDialog.setHeaderData("SchemaPage", tr("Schema\npage"));
  selectionDialog.setHeaderData("UnitFunctionEN", tr("Unit\nfunction (ENG)"));
  selectionDialog.setHeaderData("SignalName", tr("Signal name"));
  selectionDialog.setHeaderData("SwAddress", tr("SW address"));
  selectionDialog.setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  selectionDialog.setHeaderData("UnitContactName", tr("Unit\ncontact"));
  selectionDialog.setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  selectionDialog.setHeaderData("ArticleContactName", tr("Article\ncontact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("ArticleFunctionEN", tr("Article\nfunction (ENG)"));
  
  selectionDialog.addSelectionResultColumn("UnitConnection_Id_PK");
  selectionDialog.resize(700, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 1);
  // Get connection data and update
  pvLinkData.setValue("DutConnection_Id_FK", result.at(0));
  displayDutConnection();
}

void mdtTtTestLinkDialog::viewDutLinkedConnections()
{
  mdtClLinkedUnitConnectionInfoDialog dialog(this, pvDatabase);
  mdtClPathGraph graph(pvDatabase);
  QVariant connectionId;
  QList<QVariant> linkedConnectionsIdList;

  // Get current unit connection ID
  connectionId = pvLinkData.value("DutConnection_Id_FK");
  if(connectionId.isNull()){
    return;
  }
  // Load link list and get linked connections
  if(!graph.loadLinkList()){
    /** \todo Error message
    pvLastError = graph.lastError();
    displayLastError();
    */
    return;
  }
  linkedConnectionsIdList = graph.getLinkedConnectionIdList(connectionId);
  // Setup and show dialog
  dialog.setConnections(connectionId, linkedConnectionsIdList);
  dialog.exec();
}

void mdtTtTestLinkDialog::accept() 
{
  QString msg;
  QStringList missingFields;
  int i;

  // Check required data
  if(pvLinkData.value("TestConnection_Id_FK").isNull()){
    missingFields << " - Test connection";
  }
  if(pvLinkData.value("DutConnection_Id_FK").isNull()){
    missingFields << " - DUT connection";
  }
  if(!missingFields.isEmpty()){
    msg = tr("Some data are missing. Please check:") + "\n";
    for(i = 0; i < missingFields.size(); ++i){
      msg += missingFields.at(i) + "\n";
    }
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Test link data missing"));
    msgBox.exec();
    return;
  }
  // Store other data
  pvLinkData.setValue("Identification", leIdentification->text());
  pvLinkData.setValue("Value", sbValue->value());
  QDialog::accept();
}

void mdtTtTestLinkDialog::reject() 
{
  pvLinkData.clearValues();
  QDialog::reject();
}

void mdtTtTestLinkDialog::displayTestNode()
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;

  if(pvTestNodeId.isNull()){
    lbTestNodeType->clear();
    lbTestNodeSubType->clear();
    lbTestNodeNodeId->clear();
  }else{
    // Get test node data
    ///sql = "SELECT SchemaPosition, Alias, Cabinet FROM Unit_tbl WHERE Id_PK = " + pvTestNodeId.toString();
    sql = "SELECT VehicleType_Id_FK_PK, Type, SubType, SeriesNumber, NodeId";
    sql += " FROM TestNode_tbl";
    sql += " JOIN VehicleType_tbl";
    sql += "  ON VehicleType_tbl.Id_PK = TestNode_tbl.VehicleType_Id_FK_PK";
    sql += " WHERE VehicleType_Id_FK_PK = " + pvTestNodeId.toString();
    dataList = unit.getData(sql, &ok);
    if(!ok){
      lbTestNodeType->setText("<Error!>");
      lbTestNodeSubType->setText("<Error!>");
      lbTestNodeNodeId->setText("<Error!>");
      return;
    }
    Q_ASSERT(dataList.size() == 1);
    data = dataList.at(0);
    lbTestNodeType->setText(data.value("Type").toString());
    lbTestNodeSubType->setText(data.value("SubType").toString());
    lbTestNodeNodeId->setText(data.value("NodeId").toString());
  }
}

void mdtTtTestLinkDialog::displayDutUnit()
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;

  if(pvDutUnitId.isNull()){
    lbDutSchemaPosition->clear();
    lbDutAlias->clear();
    lbDutCabinet->clear();
  }else{
    // Get unit data
    sql = "SELECT SchemaPosition, Alias, Cabinet FROM Unit_tbl WHERE Id_PK = " + pvDutUnitId.toString();
    dataList = unit.getData(sql, &ok);
    if(!ok){
      lbDutSchemaPosition->setText("<Error!>");
      lbDutAlias->setText("<Error!>");
      lbDutCabinet->setText("<Error!>");
      return;
    }
    Q_ASSERT(dataList.size() == 1);
    data = dataList.at(0);
    lbDutSchemaPosition->setText(data.value("SchemaPosition").toString());
    lbDutAlias->setText(data.value("Alias").toString());
    lbDutCabinet->setText(data.value("Cabinet").toString());
  }
}

void mdtTtTestLinkDialog::displayTestConnection() 
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectionData connectionData;
  mdtClUnitConnectorData connectorData;
  bool ok;

  if(pvLinkData.value("TestConnection_Id_FK").isNull()){
    lbTestConnector->clear();
    lbTestContact->clear();
  }else{
    connectionData = unit.getConnectionData(pvLinkData.value("TestConnection_Id_FK"), false, &ok);
    if(!ok){
      lbTestContact->setText("<Error!>");
    }else{
      lbTestContact->setText(connectionData.value("UnitContactName").toString());
    }
    // Set connector name
    if(connectionData.value("UnitConnector_Id_FK").isNull()){
      lbTestConnector->setText("");
      return;
    }
    connectorData = unit.getConnectorData(connectionData.value("UnitConnector_Id_FK"), &ok, false, false, false);
    if(!ok){
      lbTestConnector->setText("<Error!>");
    }
    lbTestConnector->setText(connectorData.value("Name").toString());
  }
}

void mdtTtTestLinkDialog::displayDutConnection() 
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectionData connectionData;
  mdtClUnitConnectorData connectorData;
  bool ok;

  if(pvLinkData.value("DutConnection_Id_FK").isNull()){
    lbDutConnector->clear();
    lbDutContact->clear();
  }else{
    connectionData = unit.getConnectionData(pvLinkData.value("DutConnection_Id_FK"), false, &ok);
    if(!ok){
      lbDutContact->setText("<Error!>");
    }else{
      lbDutContact->setText(connectionData.value("UnitContactName").toString());
    }
    // Set connector name
    if(connectionData.value("UnitConnector_Id_FK").isNull()){
      lbDutConnector->setText("");
      return;
    }
    connectorData = unit.getConnectorData(connectionData.value("UnitConnector_Id_FK"), &ok, false, false, false);
    if(!ok){
      lbDutConnector->setText("<Error!>");
    }
    lbDutConnector->setText(connectorData.value("Name").toString());
  }
}

