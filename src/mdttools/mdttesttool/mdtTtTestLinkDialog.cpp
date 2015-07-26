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
#include "mdtTtTestLinkDialog.h"
#include "mdtClLinkData.h"
#include "mdtClUnit.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtTtTestNodeUnitData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClPathGraph.h"
#include "mdtClLinkedUnitConnectionInfoDialog.h"
#include <QMessageBox>
#include <QSqlRecord>
#include <QList>
#include <QStringList>

#include <QDebug>

mdtTtTestLinkDialog::mdtTtTestLinkDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;
  // Setup test link data
  if(!pvTestLinkData.setup(pvDatabase)){
    QMessageBox msgBox;
    msgBox.setText(pvTestLinkData.lastError().text());
    msgBox.setDetailedText(pvTestLinkData.lastError().systemText());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Test link data setup error"));
    msgBox.exec();
    reject();
    return;
  }
  // Setup test link data
  /**
  if(!pvTestCableLinkData.setup(pvDatabase)){
    QMessageBox msgBox;
    msgBox.setText(pvTestCableLinkData.lastError().text());
    msgBox.setDetailedText(pvTestCableLinkData.lastError().systemText());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Test cable link data setup error"));
    msgBox.exec();
    reject();
    return;
  }
  */
  // Setup UI
  setupUi(this);
  connect(pbSelectTestCableLink, SIGNAL(clicked()), this, SLOT(selectTestCableLink()));
  connect(pbSelectTestUnit, SIGNAL(clicked()), this, SLOT(selectTestNodeUnit()));
  connect(pbSelectDutUnit, SIGNAL(clicked()), this, SLOT(selectDutUnit()));
  connect(pbSelectTestConnection, SIGNAL(clicked()), this, SLOT(selectTestConnection()));
  connect(pbSelectDutConnection, SIGNAL(clicked()), this, SLOT(selectDutConnection()));
  connect(pbLinkedConnectionsInfo, SIGNAL(clicked()), this, SLOT(viewDutLinkedConnections()));
  displayTestCable();
  displayTestCableLink();
  displayTestNodeUnit();
  displayDutUnit();
  displayTestConnection();
  displayDutConnection();
}

mdtTtTestLinkDialog::~mdtTtTestLinkDialog() 
{
}

void mdtTtTestLinkDialog::setTestCableLink(const QVariant& unitConnectionStartId, const QVariant& unitConnectionEndId)
{
  /**
  pvTestCableLinkData.setValue("UnitConnectionStart_Id_FK", unitConnectionStartId);
  pvTestCableLinkData.setValue("UnitConnectionEnd_Id_FK", unitConnectionEndId);
  */
  pvTestLinkData.setValue("TestCableUnitConnectionStart_Id_FK", unitConnectionStartId);
  pvTestLinkData.setValue("TestCableUnitConnectionEnd_Id_FK", unitConnectionEndId);
  displayTestCableLink();
}

/**
  relationInfo.setChildTableName("UnitLink_view");
  relationInfo.addRelation("Id_PK", "StartUnit_Id_FK", false);
  relationInfo.addRelation("Id_PK", "EndUnit_Id_FK", false, "OR");
*/
void mdtTtTestLinkDialog::selectTestCableLink()
{
  mdtClUnit unit(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;
  QVariant physicalTestCableId;
  QVariant logicalTestCableId;
  QList<QVariant> dataList;
  QStringList fields;
  bool ok;

  // Get logical cable ID
  logicalTestCableId = pvTestLinkData.value("LogicalTestCable_Id_FK");
  if(logicalTestCableId.isNull()){
    return;
  }
  // Get physical cable ID
  sql = "SELECT Unit_Id_FK_PK FROM TestCable_tbl TC JOIN LogicalTestCable_tbl LTC ON LTC.TestCable_Id_FK = TC.Unit_Id_FK_PK";
  sql += " WHERE LTC.Id_PK = " + logicalTestCableId.toString();
  dataList = unit.getDataList<QVariant>(sql, ok);
  if(!ok){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Getting physical cable ID failed."));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  physicalTestCableId = dataList.at(0);
  // Build SQL query
  sql = "SELECT * FROM UnitLink_view";
  sql += " WHERE StartUnit_Id_FK = " + physicalTestCableId.toString();
  sql += " OR EndUnit_Id_FK = " + physicalTestCableId.toString();
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select test cable link:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("LinkBeam_Id_FK", true);
  selectionDialog.setColumnHidden("StartSchemaPosition", true);
  selectionDialog.setColumnHidden("StartAlias", true);
  selectionDialog.setColumnHidden("EndSchemaPosition", true);
  selectionDialog.setColumnHidden("EndAlias", true);
  selectionDialog.setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  selectionDialog.setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  selectionDialog.setHeaderData("EndUnitConnectorName", tr("End\nconnector"));
  selectionDialog.setHeaderData("EndUnitContactName", tr("End\ncontact"));
  selectionDialog.addColumnToSortOrder("StartUnitConnectorName", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("StartUnitContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 300);
  selectionDialog.setWindowTitle(tr("Test cable selection link"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  s = selectionDialog.selection(fields);
  Q_ASSERT(s.rowCount() == 1);
  // Update data
  pvTestLinkData.setValue("TestCableUnitConnectionStart_Id_FK", s.data(0, "UnitConnectionStart_Id_FK"));
  pvTestLinkData.setValue("TestCableUnitConnectionEnd_Id_FK", s.data(0, "UnitConnectionEnd_Id_FK"));
  /**
  pvTestCableLinkData.setValue("UnitConnectionStart_Id_FK", s.data(0, "UnitConnectionStart_Id_FK"));
  pvTestCableLinkData.setValue("UnitConnectionEnd_Id_FK", s.data(0, "UnitConnectionEnd_Id_FK"));
  */
  displayTestCableLink();
}

void mdtTtTestLinkDialog::setTestNodeUnit(const QVariant & nodeUnitId) 
{
  pvTestNodeUnitId = nodeUnitId;
  displayTestNodeUnit();
}

void mdtTtTestLinkDialog::setTestNodeUnitSelectionList(const QList< QVariant >& idList)
{
  pvTestNodeUnitSelectionIdList = idList;
}

void mdtTtTestLinkDialog::setDutUnit(const QVariant & unitId) 
{
  pvDutUnitId = unitId;
  displayDutUnit();
}

void mdtTtTestLinkDialog::setDutUnitSelectionList(const QList< QVariant >& idList)
{
  pvDutUnitSelectionIdList = idList;
}

void mdtTtTestLinkDialog::setTestConnection(const QVariant & unitConnectionId) 
{
  pvTestLinkData.setValue("TestConnection_Id_FK", unitConnectionId);
  displayTestConnection();
}

void mdtTtTestLinkDialog::setDutConnection(const QVariant & unitConnectionId) 
{
  pvTestLinkData.setValue("DutConnection_Id_FK", unitConnectionId);
  displayDutConnection();
}

void mdtTtTestLinkDialog::setLinkData(const mdtTtTestLinkData & data)
{
  pvTestLinkData = data;
  
  qDebug() << "setLinkData() - pvTestLinkData: " << pvTestLinkData;
  
  ///leIdentification->setText(pvTestLinkData.value("Identification").toString());
  displayTestCable();
  displayTestCableLink();
  displayTestConnection();
  displayDutConnection();
}

void mdtTtTestLinkDialog::selectTestNodeUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  mdtSqlTableSelection s;
  int i;

  // Setup SQL statement
  sql = "SELECT * FROM TestNodeUnit_view";
  if(pvTestNodeUnitSelectionIdList.size() > 0){
    sql += " WHERE (Unit_Id_FK_PK = " + pvTestNodeUnitSelectionIdList.at(0).toString();
    for(i = 0; i < pvTestNodeUnitSelectionIdList.size(); ++i){
      sql += " OR Unit_Id_FK_PK = " + pvTestNodeUnitSelectionIdList.at(i).toString();
    }
    sql += ")";
  }
  if(!pvTestNodeUnitId.isNull()){
    if(pvTestNodeUnitSelectionIdList.isEmpty()){
      sql += " WHERE (";
    }else{
      sql += " AND (";
    }
    sql += "Unit_Id_FK_PK <> " + pvTestNodeUnitId.toString();
    sql += ")";
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select test node unit:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Unit_Id_FK_PK", true);
  selectionDialog.setHeaderData("", tr(""));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_FK_PK");
  Q_ASSERT(s.rowCount() == 1);
  // Store unit and update GUI
  setTestConnection(QVariant());
  setTestNodeUnit(s.data(0, "Unit_Id_FK_PK"));
}

void mdtTtTestLinkDialog::selectDutUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  mdtSqlTableSelection s;
  int i;

  // Setup SQL statement
  sql = "SELECT * FROM Unit_view ";
  if(pvDutUnitSelectionIdList.size() > 0){
    sql += " WHERE (Unit_Id_PK = " + pvDutUnitSelectionIdList.at(0).toString();
    for(i = 0; i < pvDutUnitSelectionIdList.size(); ++i){
      sql += " OR Unit_Id_PK = " + pvDutUnitSelectionIdList.at(i).toString();
    }
    sql += ")";
  }
  if(!pvDutUnitId.isNull()){
    if(pvDutUnitSelectionIdList.isEmpty()){
      sql += " WHERE (";
    }else{
      sql += " AND (";
    }
    sql += "Unit_Id_PK <> " + pvDutUnitId.toString();
    sql += ")";
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
  selectionDialog.resize(800, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  // Store unit and update GUI
  setDutConnection(QVariant());
  setDutUnit(s.data(0, "Unit_Id_PK"));
}

void mdtTtTestLinkDialog::selectTestConnection() 
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  if(pvTestNodeUnitId.isNull()){
    return;
  }
  // Setup SQL statement
  sql = "SELECT * FROM UnitConnection_view WHERE Unit_Id_FK = " + pvTestNodeUnitId.toString();
  sql += " AND UnitConnection_Id_PK NOT IN (";
  sql += "  SELECT TestConnection_Id_FK FROM TestLink_tbl WHERE LogicalTestCable_Id_FK = " + pvTestLinkData.value("LogicalTestCable_Id_FK").toString();
  sql += ")";
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
  selectionDialog.addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("UnitConnection_Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  pvTestLinkData.setValue("TestConnection_Id_FK", s.data(0, "UnitConnection_Id_PK"));
  displayTestConnection();
}

void mdtTtTestLinkDialog::selectDutConnection() 
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  mdtClUnit unit(this, pvDatabase);
  QStringList fields;
  mdtSqlTableSelection s;

  if(pvDutUnitId.isNull()){
    return;
  }
  // Setup SQL statement
  sql = "SELECT * FROM UnitConnection_view WHERE Unit_Id_FK = " + pvDutUnitId.toString();
  if(cbHideAffectedDutConnections->isChecked()){
    sql += " AND UnitConnection_Id_PK NOT IN (";
    sql += "  SELECT DutConnection_Id_FK FROM TestLink_tbl WHERE LogicalTestCable_Id_FK = " + pvTestLinkData.value("LogicalTestCable_Id_FK").toString();
    sql += ")";
  }
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
  selectionDialog.addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  fields << "UnitConnection_Id_PK" << "UnitFunctionEN";
  s = selectionDialog.selection(fields);
  Q_ASSERT(s.rowCount() == 1);
  // Get connection data and update
  pvTestLinkData.setValue("DutConnection_Id_FK", s.data(0, "UnitConnection_Id_PK"));
  /**
  if(leIdentification->text().trimmed().isEmpty()){
    leIdentification->setText(s.data(0, "UnitFunctionEN").toString());
  }
  */
  displayDutConnection();
}

void mdtTtTestLinkDialog::viewDutLinkedConnections()
{
  mdtClLinkedUnitConnectionInfoDialog dialog(this, pvDatabase);
  mdtClPathGraph graph(pvDatabase);
  QVariant connectionId;
  QList<QVariant> linkedConnectionsIdList;
  bool ok;

  // Get current unit connection ID
  connectionId = pvTestLinkData.value("DutConnection_Id_FK");
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
  linkedConnectionsIdList = graph.getLinkedConnectionIdList(connectionId, ok);
  if(!ok){
    /** \todo Error message
    pvLastError = graph.lastError();
    displayLastError();
    */
    return;
  }
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
  if(pvTestLinkData.value("TestConnection_Id_FK").isNull()){
    missingFields << " - Test connection";
  }
  if(pvTestLinkData.value("DutConnection_Id_FK").isNull()){
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
  /**
  pvTestLinkData.setValue("Identification", leIdentification->text());
  pvTestLinkData.setValue("Value", sbValue->value());
  */
  QDialog::accept();
}

void mdtTtTestLinkDialog::reject() 
{
  pvTestLinkData.clearValues();
  QDialog::reject();
}

void mdtTtTestLinkDialog::displayTestCable()
{
  mdtClUnit unit(0, pvDatabase);
  QList<QVariant> dataList;
  QVariant id;
  QString sql;
  bool ok;

  // Clear all widgets
  lbTestCableIdentification->clear();
  lbTestCableKey->clear();
  // Get logical test cable ID
  id = pvTestLinkData.value("LogicalTestCable_Id_FK");
  if(id.isNull()){
    return;
  }
  // Get physical cable Identification
  sql = "SELECT Identification FROM TestCable_tbl TC JOIN LogicalTestCable_tbl LTC ON LTC.TestCable_Id_FK = TC.Unit_Id_FK_PK";
  sql += " WHERE LTC.Id_PK = " + id.toString();
  dataList = unit.getDataList<QVariant>(sql, ok);
  if(!ok){
    lbTestCableIdentification->setText("<Error>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  lbTestCableIdentification->setText(dataList.at(0).toString());
  // Get logical cable key
  sql = "SELECT Key FROM LogicalTestCable_tbl WHERE Id_PK = " + id.toString();
  dataList = unit.getDataList<QVariant>(sql, ok);
  if(!ok){
    lbTestCableKey->setText("<Error>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  lbTestCableKey->setText(dataList.at(0).toString());
}

/// \todo Supprimer pvTestCableLinkData et reprendre infos de pvTestLinkData (éviter de devoir gérer 36 membres ...)
void mdtTtTestLinkDialog::displayTestCableLink()
{
  mdtClLink lnk(0, pvDatabase);
  mdtClUnit unit(0, pvDatabase);
  QVariant var;
  QVariant id;
  QString sql;
  QList<QVariant> dataList;
  bool ok;

  // Clear all widgets
  lbTestCableLinkIdentification->clear();
  lbTestCableLinkStartConnector->clear();
  lbTestCableLinkStartContact->clear();
  lbTestCableLinkEndConnector->clear();
  lbTestCableLinkEndContact->clear();
  // Get link data
  if(pvTestLinkData.value("TestCableUnitConnectionStart_Id_FK").isNull()){
    return;
  }
  if(pvTestLinkData.value("TestCableUnitConnectionEnd_Id_FK").isNull()){
    return;
  }
  // Get Test cable link identification
  /**
  sql = "SELECT TC.Identification FROM TestCable_tbl TC";
  sql += " JOIN LogicalTestCable_tbl LTC ON LTC.TestCable_Id_FK = TC.Unit_Id_FK_PK";
  sql += " JOIN TestLink_tbl TL ON TL.LogicalTestCable_Id_FK = LTC.Id_PK";
  */
  sql = "SELECT Identification FROM Link_tbl L";
  sql += " WHERE L.UnitConnectionStart_Id_FK = " + pvTestLinkData.value("TestCableUnitConnectionStart_Id_FK").toString();
  sql += " AND L.UnitConnectionEnd_Id_FK = " + pvTestLinkData.value("TestCableUnitConnectionEnd_Id_FK").toString();
  
  
  qDebug() << "displayTestCableLink() - SQL: " << sql;
  

  
  dataList = lnk.getDataList<QVariant>(sql, ok);
  if(!ok){
    lbTestCableLinkIdentification->setText("<Error>");
    return;
  }
  
  qDebug() << "dataList.size(): " << dataList.size();
  
  Q_ASSERT(dataList.size() == 1);
  lbTestCableLinkIdentification->setText(dataList.at(0).toString());
  /**
  pvTestCableLinkData = lnk.getLinkData(pvTestCableLinkData.value("UnitConnectionStart_Id_FK"), pvTestCableLinkData.value("UnitConnectionEnd_Id_FK"), false, false, &ok);
  if(!ok){
    pvTestCableLinkData.clearValues();
    lbIdentification->setText("<Error>");
    return;
  }
  lbIdentification->setText(pvTestCableLinkData.value("").toString());
  */
  // Get start connectors name
  ///id = pvTestCableLinkData.value("UnitConnectionStart_Id_FK");
  id = pvTestLinkData.value("TestCableUnitConnectionStart_Id_FK");
  ///var = unit.getConnectorNameOfConnectionId(id, ok);
  if(!ok){
    lbTestCableLinkStartConnector->setText("<Error>");
    return;
  }
  lbTestCableLinkStartConnector->setText(var.toString());
  // Get start contact name
 /// var = unit.getContactName(id, ok);
  if(!ok){
    lbTestCableLinkStartContact->setText("<Error>");
    return;
  }
  lbTestCableLinkStartContact->setText(var.toString());
  // Get end connector name
  ///id = pvTestCableLinkData.value("UnitConnectionEnd_Id_FK");
  id = pvTestLinkData.value("TestCableUnitConnectionEnd_Id_FK");
  ///var = unit.getConnectorNameOfConnectionId(id, ok);
  if(!ok){
     lbTestCableLinkEndConnector->setText("<Error>");
     return;
  }
  lbTestCableLinkEndConnector->setText(var.toString());
  // Get end contact name
  ///var = unit.getContactName(id, ok);
  if(!ok){
    lbTestCableLinkEndContact->setText("<Error>");
    return;
  }
  lbTestCableLinkEndContact->setText(var.toString());
}

void mdtTtTestLinkDialog::displayTestNodeUnit()
{
  mdtTtTestNodeUnit tnu(0, pvDatabase);
  mdtTtTestNodeUnitData data;
  bool ok;

  if(pvTestNodeUnitId.isNull()){
    lbTnuSchemaPosition->clear();
    lbTnuAlias->clear();
  }else{
    // Get test node unit data
    data = tnu.getData(pvTestNodeUnitId, &ok, false);
    if(!ok){
      lbTnuSchemaPosition->setText("<Error!>");
      lbTnuAlias->setText("<Error!>");
      return;
    }
    lbTnuSchemaPosition->setText(data.unitData().value("SchemaPosition").toString());
    lbTnuAlias->setText(data.unitData().value("Alias").toString());
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
    dataList = unit.getDataList<QSqlRecord>(sql, ok);
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

/// \todo adapt
void mdtTtTestLinkDialog::displayTestConnection() 
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectionData connectionData;
  mdtClUnitConnectorData connectorData;
  bool ok;

  if(pvTestLinkData.value("TestConnection_Id_FK").isNull()){
    lbTestConnector->clear();
    lbTestContact->clear();
  }else{
    ///connectionData = unit.getConnectionData(pvTestLinkData.value("TestConnection_Id_FK"), false, &ok);
    if(!ok){
      lbTestContact->setText("<Error!>");
    }else{
      ///lbTestContact->setText(connectionData.value("UnitContactName").toString());
    }
    // Set connector name
    /**
    if(connectionData.value("UnitConnector_Id_FK").isNull()){
      lbTestConnector->setText("");
      return;
    }
    */
    ///connectorData = unit.getConnectorData(connectionData.value("UnitConnector_Id_FK"), &ok, false, false, false);
    if(!ok){
      lbTestConnector->setText("<Error!>");
    }
    ///lbTestConnector->setText(connectorData.value("Name").toString());
  }
}

/// \todo adapt
void mdtTtTestLinkDialog::displayDutConnection() 
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectionData connectionData;
  mdtClUnitConnectorData connectorData;
  bool ok;

  if(pvTestLinkData.value("DutConnection_Id_FK").isNull()){
    lbDutConnector->clear();
    lbDutContact->clear();
  }else{
    ///connectionData = unit.getConnectionData(pvTestLinkData.value("DutConnection_Id_FK"), false, &ok);
    if(!ok){
      lbDutContact->setText("<Error!>");
    }else{
      ///lbDutContact->setText(connectionData.value("UnitContactName").toString());
    }
    // Set connector name
    /**
    if(connectionData.value("UnitConnector_Id_FK").isNull()){
      lbDutConnector->setText("");
      return;
    }
    */
    ///connectorData = unit.getConnectorData(connectionData.value("UnitConnector_Id_FK"), &ok, false, false, false);
    if(!ok){
      lbDutConnector->setText("<Error!>");
    }
    ///lbDutConnector->setText(connectorData.value("Name").toString());
  }
}
