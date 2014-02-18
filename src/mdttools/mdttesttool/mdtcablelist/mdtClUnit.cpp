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
#include "mdtClUnit.h"
#include "mdtClArticle.h"
#include "mdtClLink.h"
#include <QSqlQuery>
#include <QSqlField>
#include <QPair>

#include <QDebug>

mdtClUnit::mdtClUnit(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
  ///pvUnitLinkModel = 0;
}

mdtClUnit::~mdtClUnit()
{
  ///delete pvUnitLinkModel;
}

QString mdtClUnit::sqlForComponentSelection(const QVariant& unitId) const
{
  QString sql;

  sql =  "SELECT Id_PK, SchemaPosition, Cabinet, Coordinate "\
         "FROM Unit_tbl "\
         "WHERE ( Id_PK <> " + unitId.toString() + " "\
         "AND Composite_Id_FK IS NULL )";

  return sql;
}

QString mdtClUnit::sqlForConnectorContactSelection(const QVariant & connectorId) const
{
  QString sql;

  sql = "SELECT * FROM ConnectorContact_tbl WHERE Connector_Id_FK = " + connectorId.toString();

  return sql;
}

QString mdtClUnit::sqlForArticleConnectorSelection(const QVariant & articleId, const QVariant & unitId) const
{
  QString sql;

  sql = "SELECT DISTINCT "\
        " Id_PK, Name "\
        "FROM ArticleConnector_tbl "\
        "WHERE Article_Id_FK = " + articleId.toString();
  sql += " AND Id_PK NOT IN ( "\
         " SELECT ArticleConnector_Id_FK "\
         " FROM UnitConnector_tbl "\
         " WHERE ( Unit_Id_FK = " + unitId.toString() + " ) "\
         " AND ( ArticleConnector_Id_FK IS NOT NULL ) )";

  return sql;
}

QString mdtClUnit::sqlForFreeArticleConnectionSelection(const QVariant& articleId, const QVariant& unitId) const
{
  QString sql;

  sql = "SELECT * FROM ArticleConnection_tbl ";
  sql += "WHERE Article_Id_FK = " + articleId.toString();
  sql += " AND ArticleConnector_Id_FK IS NULL ";
  sql += " AND Id_PK NOT IN ("\
         "  SELECT ArticleConnection_Id_FK"\
         "  FROM UnitConnection_tbl"\
         "  WHERE Unit_Id_FK = " + unitId.toString();
  sql += "  AND ArticleConnection_Id_FK IS NOT NULL)";

  return sql;
}

QString mdtClUnit::sqlForArticleConnectionLinkedToArticleConnectorSelection(const QVariant & articleConnectorId, const QVariant & unitId) const
{
  QString sql;

  sql = "SELECT * FROM ArticleConnection_tbl ";
  sql += "WHERE ArticleConnector_Id_FK = " + articleConnectorId.toString();
  sql += " AND Id_PK NOT IN ("\
         "  SELECT ArticleConnection_Id_FK"\
         "  FROM UnitConnection_tbl"\
         "  WHERE Unit_Id_FK = " + unitId.toString();
  sql += "  AND ArticleConnection_Id_FK IS NOT NULL)";

  return sql;
}

mdtClUnitConnectionData mdtClUnit::getConnectionData(const QVariant & unitConnectionId, bool includeArticleConnectionData, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;

  if(unitConnectionId.isNull()){
    pvLastError.setError("Trying to get unit connection data for a NULL unit connection ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    *ok = false;
    return mdtClUnitConnectionData();
  }
  sql = "SELECT * FROM UnitConnection_tbl WHERE Id_PK = " + unitConnectionId.toString();

  return getConnectionDataPv(sql, includeArticleConnectionData, ok);
}

mdtClUnitConnectorData mdtClUnit::getConnectorData(const QVariant& unitConnectorId, bool* ok, bool includeConnectionData, bool includeArticleConnectorData, bool includeBaseConnectorData)
{
  Q_ASSERT(ok != 0);

  QString sql;

  if(unitConnectorId.isNull()){
    pvLastError.setError("Trying to get unit connector data for a NULL unit connector ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    *ok = false;
    return mdtClUnitConnectorData();
  }
  sql = "SELECT * FROM UnitConnector_tbl WHERE Id_PK = " + unitConnectorId.toString();

  return getConnectorDataPv(sql, ok, includeConnectionData, includeArticleConnectorData, includeBaseConnectorData);
}

bool mdtClUnit::addConnectionDataListFromConnectorContactIdList(mdtClUnitConnectorData& data, const QList< QVariant >& connectorContactIdList)
{
  mdtClArticle art(0, database());
  QList<QSqlRecord> connectorContactDataList;
  mdtClUnitConnectionData unitConnectionData;
  int i;
  bool ok;

  // Get connector contact data
  connectorContactDataList = art.getConnectorContactDataList(connectorContactIdList, &ok);
  if(!ok){
    return false;
  }
  // Create unit contact data list
  if(!unitConnectionData.setup(database(), false)){
    pvLastError = unitConnectionData.lastError();
    return false;
  }
  for(i = 0; i < connectorContactDataList.size(); ++i){
    unitConnectionData.clearValues();
    unitConnectionData.setValue("UnitContactName", connectorContactDataList.at(i).value("Name"));
    unitConnectionData.setValue("Unit_Id_FK", data.value("Unit_Id_FK"));
    unitConnectionData.setValue("UnitConnector_Id_FK", data.value("Id_PK"));
    data.addConnectionData(unitConnectionData);
  }

  return true;
}

bool mdtClUnit::addArticleConnectorData(mdtClUnitConnectorData & data, const QVariant & articleConnectorId, bool copyConnectorName)
{
  mdtClArticle art(0, database());
  mdtClArticleConnectorData articleConnectorData;
  bool ok;

  // Get article connector data
  articleConnectorData = art.getConnectorData(articleConnectorId, &ok, false, true);
  if(!ok){
    pvLastError = art.lastError();
    return false;
  }
  // Update unit connector data
  data.setArticleConnectorData(articleConnectorData);
  if(copyConnectorName){
    data.setValue("Name", articleConnectorData.value("Name"));
  }

  return true;
}

bool mdtClUnit::addConnectionDataListFromArticleConnectionIdList(mdtClUnitConnectorData & data, const QList<QVariant> & articleConnectionIdList, bool copyContactName)
{
  mdtClArticle art(0, database());
  mdtClArticleConnectionData articleConnectionData;
  QList<mdtClArticleConnectionData> articleConnectionDataList;
  mdtClUnitConnectionData unitConnectionData;
  bool ok;
  int i;

  // Get article connection data list
  articleConnectionDataList = art.getConnectionDataListFromConnectionIdList(articleConnectionIdList, &ok);
  if(!ok){
    pvLastError = art.lastError();
    return false;
  }
  // Create unit connections
  if(!unitConnectionData.setup(database(), false)){
    pvLastError = unitConnectionData.lastError();
    return false;
  }
  for(i = 0; i < articleConnectionDataList.size(); ++i){
    mdtClArticleConnectionData articleConnectionData = articleConnectionDataList.at(i);
    unitConnectionData.clearValues();
    unitConnectionData.setValue("Unit_Id_FK", data.value("Unit_Id_FK"));
    unitConnectionData.setValue("UnitConnector_Id_FK", data.value("Id_PK"));
    unitConnectionData.setArticleConnectionData(articleConnectionData);
    if(copyContactName){
      unitConnectionData.setValue("UnitContactName", articleConnectionData.value("ArticleContactName"));
    }
    data.addConnectionData(unitConnectionData);
  }

  return true;
}

bool mdtClUnit::addComponent(const QVariant &unitId, const QVariant &componentId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "UPDATE Unit_tbl "\
        "SET Composite_Id_FK = :Composite_Id_FK "\
        "WHERE Id_PK = " + componentId.toString();
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for component assignation", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Composite_Id_FK", unitId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component assignation", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::removeComponent(const QVariant &componentId)
{
  QList<QVariant> idList;

  idList.append(componentId);

  return removeComponents(idList);
}

bool mdtClUnit::removeComponents(const QList<QVariant> &componentIdList)
{
  int i;
  QString sql;
  QSqlError sqlError;

  if(componentIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "UPDATE Unit_tbl SET Composite_Id_FK = NULL ";
  for(i = 0; i < componentIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += " Id_PK = " + componentIdList.at(i).toString();
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component assignation removal", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::removeComponents(const QModelIndexList & indexListOfSelectedRows)
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return removeComponents(idList);
}

int mdtClUnit::toUnitRelatedArticleConnectionCount(const QVariant & unitId)
{
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;

  // Build and run query
  sql = "SELECT COUNT(*) FROM UnitConnection_tbl \
         JOIN ArticleConnection_tbl \
         ON UnitConnection_tbl.ArticleConnection_Id_FK = ArticleConnection_tbl.Id_PK \
         WHERE UnitConnection_tbl.Unit_Id_FK = " + unitId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to count units related to article connections", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return -1;
  }
  // Get value
  if(query.next()){
    Q_ASSERT(query.value(0).isValid());
    return query.value(0).toInt();
  }

  // Should not happen
  return -1;
}

/**
QSqlQueryModel *mdtClUnit::toUnitRelatedLinksModel(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList)
{
  QString sql;
  QSqlError sqlError;
  int i;

  if(pvUnitLinkModel == 0){
    pvUnitLinkModel = new QSqlQueryModel;
  }
  sql = "SELECT * FROM UnitLink_view "\
        "WHERE ( StartUnit_Id_FK = " + unitId.toString() + " "\
        "OR EndUnit_Id_FK = " + unitId.toString() + " ) ";
  if(unitConnectionIdList.size() > 0){
    sql += " AND ( ";
  }
  for(i = 0; i < unitConnectionIdList.size(); ++i){
    if(i > 0){
      sql += " OR ";
    }
    sql += " UnitConnectionStart_Id_FK = " + unitConnectionIdList.at(i).toString();
    sql += " OR UnitConnectionEnd_Id_FK = " + unitConnectionIdList.at(i).toString();
  }
  if(unitConnectionIdList.size() > 0){
    sql += " ) ";
  }
  pvUnitLinkModel->setQuery(sql, database());
  sqlError = pvUnitLinkModel->lastError();
  if(sqlError.isValid()){
    pvLastError.setError("Cannot execute query to get links related to unit", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
  }

  return pvUnitLinkModel;
}
*/

/**
QStringList mdtClUnit::toUnitRelatedLinksList(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList)
{
  int row, col;
  QSqlQueryModel *model;
  QModelIndex index;
  QString link;
  QStringList linksList;

  model = toUnitRelatedLinksModel(unitId, unitConnectionIdList);
  Q_ASSERT(model != 0);
  for(row = 0; row < model->rowCount(); ++row){
    col = model->record().indexOf("Identification");
    index = model->index(row, col);
    link = model->data(index).toString();
    link += "-(";
    col = model->record().indexOf("StartUnitConnectorName");
    index = model->index(row, col);
    link += model->data(index).toString();
    link += ";";
    col = model->record().indexOf("StartUnitContactName");
    index = model->index(row, col);
    link += model->data(index).toString();
    link += "-";
    col = model->record().indexOf("EndUnitConnectorName");
    index = model->index(row, col);
    link += model->data(index).toString();
    link += ";";
    col = model->record().indexOf("EndUnitContactName");
    index = model->index(row, col);
    link += model->data(index).toString();
    link += ")";
    linksList.append(link);
  }

  return linksList;
}
*/

QStringList mdtClUnit::toUnitRelatedLinksList(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  QString link;
  QStringList linksList;
  int i;

  // Build SQL statement
  sql = "SELECT * FROM UnitLink_view "\
        "WHERE ( StartUnit_Id_FK = " + unitId.toString() + " "\
        "OR EndUnit_Id_FK = " + unitId.toString() + " ) ";
  sql += " AND ( ArticleConnectionStart_Id_FK IS NULL AND ArticleConnectionEnd_Id_FK IS NULL ) ";
  if(unitConnectionIdList.size() > 0){
    sql += " AND ( ";
  }
  for(i = 0; i < unitConnectionIdList.size(); ++i){
    if(i > 0){
      sql += " OR ";
    }
    sql += " UnitConnectionStart_Id_FK = " + unitConnectionIdList.at(i).toString();
    sql += " OR UnitConnectionEnd_Id_FK = " + unitConnectionIdList.at(i).toString();
  }
  if(unitConnectionIdList.size() > 0){
    sql += " ) ";
  }
  // Get link data
  dataList = getData(sql, ok);
  if(!*ok){
    return linksList;
  }
  // Build link strings
  for(i = 0; i < dataList.size(); ++i){
    data = dataList.at(i);
    link = data.value("Identification").toString();
    link += "-(";
    link += data.value("StartUnitConnectorName").toString();
    link += ";";
    link += data.value("StartUnitContactName").toString();
    link += "-";
    link += data.value("EndUnitConnectorName").toString();
    link += ";";
    link += data.value("EndUnitContactName").toString();
    link += ")";
    linksList.append(link);
  }

  return linksList;
}

QString mdtClUnit::toUnitRelatedLinksListStr(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList, bool *ok)
{
  Q_ASSERT(ok != 0);

  QStringList linksList;
  QString str;
  int i;

  linksList = toUnitRelatedLinksList(unitId, unitConnectionIdList, ok);
  if(!*ok){
    return str;
  }
  for(i = 0; i < linksList.size(); ++i){
    str += linksList.at(i) + "\n";
  }

  return str;
}

QString mdtClUnit::toUnitRelatedLinksListStr(const QVariant &unitId, const QModelIndexList & indexListOfSelectedRows, bool *ok)
{
  Q_ASSERT(ok != 0);

  int i;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return toUnitRelatedLinksListStr(unitId, idList, ok);
}

bool mdtClUnit::addConnection(const mdtClUnitConnectionData & data, bool singleTransaction)
{
  QSqlQuery query(database());

  // We want to update many tables, so manually ask to beginn a transaction
  if(singleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  if(!addRecord(data, "UnitConnection_tbl", query)){
    if(singleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  // Add article link based links if required
  if(!data.value("ArticleConnection_Id_FK").isNull()){
    mdtClUnitConnectionData _data = data;
    _data.setValue("Id_PK", query.lastInsertId());
    if(!addArticleBasedLinkForUnitConnection(_data)){
      if(singleTransaction){
        rollbackTransaction();
      }
      return false;
    }
  }
  if(singleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtClUnit::editConnection(const QVariant & connectionId, const mdtClUnitConnectionData & data)
{
  /// \todo WRONG !
  return updateRecord("UnitConnection_tbl", data, "Id_PK", connectionId);
}

bool mdtClUnit::removeConnection(const QVariant & unitConnectionId, bool handleTransaction)
{
  mdtClUnitConnectionData connectionData;
  bool ok;

  // We need some information about connection, because we possibly have to remove some links
  connectionData = getConnectionData(unitConnectionId, true, &ok);
  if(!ok){
    return false;
  }
  // We want to update many tables, so manually ask to beginn a transaction
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  // Remove article link based links if required
  if(!connectionData.value("ArticleConnection_Id_FK").isNull()){
    if(!removeArticleBasedLinkForUnitConnection(connectionData)){
      if(handleTransaction){
        rollbackTransaction();
      }
      return false;
    }
  }
  // Remove connection
  if(!removeData("UnitConnection_tbl", "Id_PK", unitConnectionId)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  // Commit
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtClUnit::removeConnections(const QModelIndexList & indexListOfSelectedRows)
{
  int i;

  if(!beginTransaction()){
    return false;
  }
  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    if(!removeConnection(indexListOfSelectedRows.at(i).data(), false)){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtClUnit::addConnector(const mdtClUnitConnectorData & data)
{
  QSqlQuery query(database());
  ///QList<mdtSqlRecord> unitConnectionRecordList;
  mdtClUnitConnectionData connectionData;
  QVariant unitId;
  QVariant unitConnectorId;
  int i;

  // Check data coherence
  if(!checkConnectorData(data)){
    return false;
  }
  // Get unit ID
  unitId = data.value("Unit_Id_FK");
  Q_ASSERT(!unitId.isNull());
  // Because we add connector and get its ID for connections insertion, we use a transaction for the whole process
  if(!beginTransaction()){
    return false;
  }
  // Add unit connector
  if(!addRecord(data, "UnitConnector_tbl", query)){
    rollbackTransaction();
    return false;
  }
  // Get unit connector ID and update data
  unitConnectorId = query.lastInsertId();
  if(unitConnectorId.isNull()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot get unit connector ID for connector inertion."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  // Update and add connection data
  for(i = 0; i < data.connectionDataList().size(); ++i){
    connectionData = data.connectionDataList().at(i);
    connectionData.setValue("Unit_Id_FK", unitId);
    connectionData.setValue("UnitConnector_Id_FK", unitConnectorId);
    if(!addConnection(connectionData, false)){
      rollbackTransaction();
      return false;
    }
    /**
    mdtSqlRecord record = data.connectionDataList().at(i);
    record.setValue("Unit_Id_FK", unitId);
    record.setValue("UnitConnector_Id_FK", unitConnectorId);
    unitConnectionRecordList.append(record);
    */
  }
  // Add connections
  /**
  if(!addRecordList(unitConnectionRecordList, "UnitConnection_tbl", false)){
    rollbackTransaction();
    return false;
  }
  */
  // Commit
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtClUnit::removeConnector(const QVariant& unitConnectorId)
{
  QList<QSqlRecord> connectionIdList;
  QString sql;
  bool ok;
  int i;

  // Get list of unit connections to remove
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE UnitConnector_Id_FK = " + unitConnectorId.toString();
  connectionIdList = getData(sql, &ok);
  if(!ok){
    return false;
  }
  // Beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  // Remove connections
  for(i = 0; i < connectionIdList.size(); ++i){
    if(!removeConnection(connectionIdList.at(i).value("Id_PK"), false)){
      rollbackTransaction();
      return false;
    }
  }
  /// \todo Use remove connections, because links must be handled - OR NOT
  /**
  if(!removeData("UnitConnection_tbl", "UnitConnector_Id_FK", unitConnectorId)){
    rollbackTransaction();
    return false;
  }
  */
  // Remove connector
  if(!removeData("UnitConnector_tbl", "Id_PK", unitConnectorId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtClUnit::removeConnectors(const QModelIndexList & indexListOfSelectedRows)
{
  int i;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    if(!removeConnector(indexListOfSelectedRows.at(i).data())){
      return false;
    }
  }

  return true;
}


bool mdtClUnit::addRange(const QVariant & baseUnitConnectionId, const mdtClUnitConnectionData & rangeData) {
}

/**
mdtClLinkData mdtClUnit::getUnitLinkData(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  mdtClLinkData data;

  // Setup and run query to get data in unit link view
  sql = "SELECT "\
        "UnitConnectionStart_Id_FK , UnitConnectionEnd_Id_FK, ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, "\
        "SinceVersion, Modification, Identification, Value, LinkType_Code_FK, LinkDirection_Code_FK ";
  sql += " FROM UnitLink_view ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get unit link data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(!query.next()){
    return data;
  }
  data.setUnitConnectionStartId(query.value(0));
  data.setUnitConnectionEndId(query.value(1));
  data.setArticleConnectionStartId(query.value(2));
  data.setArticleConnectionEndId(query.value(3));
  data.setSinceVersion(query.value(4));
  data.setModification(query.value(5));
  data.setIdentification(query.value(6));
  data.setValue(query.value(7));
  data.setLinkTypeCode(query.value(8));
  data.setLinkDirectionCode(query.value(9));
  // Setup and run query to get data in vehicles type table
  sql = "SELECT VehicleTypeStart_Id_FK, VehicleTypeEnd_Id_FK FROM VehicleType_Link_tbl ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get vehicle type link data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  while(query.next()){
    data.addVehicleTypeStartId(query.value(0));
    data.addVehicleTypeEndId(query.value(1));
  }

  return data;
}
*/

/**
mdtClLinkData mdtClUnit::getUnitLinkData(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QList<QSqlRecord> dataList;
  mdtClLinkData data;
  QString sql;

  // Get Link_tbl part
  sql = "SELECT * FROM Link_tbl WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  /// \todo Implement VehicleType_Link_tbl part !
}
*/

bool mdtClUnit::editLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, const mdtClLinkData &data)
{
  /**
  QString sql;
  QSqlError sqlError;
  QList<QPair<QVariant, QVariant> > vehicleTypeList;
  int i;
  mdtClLinkData currentLinkData;

  // Get list of currently attached vehicle type
  currentLinkData = getUnitLinkData(unitConnectionStartId, unitConnectionEndId);
  if(!currentLinkData.isValid()){
    return false;
  }
  currentLinkData.buildVehicleTypeStartEndIdList();
  vehicleTypeList = currentLinkData.vehicleTypeStartEndIdList();
  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  QSqlQuery query(database());
  // Remove currently related vehicle types
  for(i = 0; i < vehicleTypeList.size(); ++i){
    if(!removeLinkFromVehicleType(vehicleTypeList.at(i).first, vehicleTypeList.at(i).second, currentLinkData.unitConnectionStartId(), currentLinkData.unitConnectionEndId(), query)){
      rollbackTransaction();
      return false;
    }
  }
  // Prepare query for Link table edition
  sql = "UPDATE Link_tbl SET "\
        "ArticleConnectionStart_Id_FK = :ArticleConnectionStart_Id_FK , "\
        "ArticleConnectionEnd_Id_FK = :ArticleConnectionEnd_Id_FK , "\
        "UnitConnectionStart_Id_FK = :UnitConnectionStart_Id_FK , "\
        "UnitConnectionEnd_Id_FK = :UnitConnectionEnd_Id_FK , "\
        "SinceVersion = :SinceVersion , "\
        "Modification = :Modification , "\
        "Identification = :Identification , "\
        "LinkDirection_Code_FK = :LinkDirection_Code_FK , "\
        "LinkType_Code_FK = :LinkType_Code_FK , "\
        "Value = :Value ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  if(!query.prepare(sql)){
    rollbackTransaction();
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":ArticleConnectionStart_Id_FK", data.articleConnectionStartId());
  query.bindValue(":ArticleConnectionEnd_Id_FK", data.articleConnectionEndId());
  query.bindValue(":UnitConnectionStart_Id_FK", data.unitConnectionStartId());
  query.bindValue(":UnitConnectionEnd_Id_FK", data.unitConnectionEndId());
  query.bindValue(":SinceVersion", data.sinceVersion());
  query.bindValue(":Modification", data.modification());
  query.bindValue(":Identification", data.identification());
  query.bindValue(":LinkDirection_Code_FK", data.linkDirectionCode());
  query.bindValue(":LinkType_Code_FK", data.linkTypeCode());
  query.bindValue(":Value", data.value());
  if(!query.exec()){
    rollbackTransaction();
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Add a link in VehicleType_Link table for each start-end vehicles id assignation
  vehicleTypeList = data.vehicleTypeStartEndIdList();
  for(i = 0; i < vehicleTypeList.size(); ++i){
    if(!addLinkToVehicleType(vehicleTypeList.at(i).first, vehicleTypeList.at(i).second, data.unitConnectionStartId(), data.unitConnectionEndId())){
      rollbackTransaction();
      return false;
    }
  }
  commitTransaction();

  return true;
  */
}

mdtClUnitConnectionData mdtClUnit::getConnectionDataPv(const QString & sql, bool includeArticleConnectionData, bool *ok)
{
  Q_ASSERT(ok != 0);

  mdtClUnitConnectionData data;
  QList<QSqlRecord> dataList;
  QVariant articleConnectionId;
  mdtClArticle art(0, database());
  mdtClArticleConnectionData articleConnectionData;

  // Get unit connection data part
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  if(includeArticleConnectionData){
    // If data is based on article connection, get article connection data
    articleConnectionId = data.value("ArticleConnection_Id_FK");
    if(!articleConnectionId.isNull()){
      articleConnectionData = art.getConnectionData(articleConnectionId, ok);
      if(!*ok){
        return data;
      }
      data.setArticleConnectionData(articleConnectionData);
    }
  }else{
    // This is important for link insersion
    data.setValue("ArticleConnection_Id_FK", QVariant(QVariant::Int));
  }
  // Done
  *ok = true;

  return data;
}

mdtClUnitConnectorData mdtClUnit::getConnectorDataPv(const QString& sql, bool* ok, bool includeConnectionData, bool includeArticleConnectorData, bool includeBaseConnectorData)
{
  Q_ASSERT(ok != 0);

  mdtClUnitConnectorData data;
  QList<QSqlRecord> dataList;
  mdtClUnitConnectionData connectionData;
  QVariant articleConnectorId;
  QVariant articleConnectionId;
  mdtClArticle art(0, database());
  mdtClArticleConnectionData articleConnectionData;
  QString _sql;
  int i;

  // Get unit connector data part
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  // Get article connactor ID
  articleConnectorId = data.value("ArticleConnector_Id_FK");
  Q_ASSERT(!data.value("Id_PK").isNull());
  // Include unit connections if requested
  if(includeConnectionData){
    _sql = "SELECT * FROM UnitConnection_tbl WHERE UnitConnector_Id_FK = " + data.value("Id_PK").toString();
    dataList = getData(_sql, ok);
    if(!*ok){
      return data;
    }
    for(i = 0; i < dataList.size(); ++i){
      connectionData = dataList.at(i);
      // If data is based on article connection, get article connection data
      articleConnectionId = connectionData.value("ArticleConnection_Id_FK");
      if(!articleConnectionId.isNull()){
        articleConnectionData = art.getConnectionData(articleConnectionId, ok);
        if(!*ok){
          return data;
        }
        connectionData.setArticleConnectionData(articleConnectionData);
      }
      data.addConnectionData(connectionData);
    }
  }
  // Add connector data part
  if((includeBaseConnectorData)&&(!data.value("Connector_Id_FK").isNull())){
    _sql = "SELECT * FROM Connector_tbl WHERE Id_PK = " + data.value("Connector_Id_FK").toString();
    dataList = getData(_sql, ok);
    if(!*ok){
      return data;
    }
    Q_ASSERT(dataList.size() == 1);
    data.setConnectorData(dataList.at(0));
  }
  // Get article connector data if requested
  if(includeArticleConnectorData){
    if(!articleConnectorId.isNull()){
      mdtClArticle art(0, database());
      mdtClArticleConnectorData articleConnectorData = art.getConnectorData(articleConnectorId, ok, false, includeBaseConnectorData);
      if(!*ok){
        pvLastError = art.lastError();
        return data;
      }
      data.setArticleConnectorData(articleConnectorData);
    }
  }
  // Check data coherence
  *ok = checkConnectorData(data);

  return data;
}


bool mdtClUnit::checkConnectorData(const mdtClUnitConnectorData & data)
{
  int i;
  QVariant articleConnectorId;

  // Unit_Id_FK must be set
  if(data.value("Unit_Id_FK").isNull()){
    pvLastError.setError(tr("Given unit connector data contains not a valid unit ID."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Get article connector ID
  articleConnectorId = data.value("ArticleConnector_Id_FK");
  /*
   * If ArticleConnector_Id_FK is set (i.e. unit connector is based on a article connector),
   * and this article connector is itself based on a connector (from Connector_tbl),
   * data's Connector_Id_FK must be the same than article connector's Connector_Id_FK.
   */
  if((!articleConnectorId.isNull())&&(data.articleConnectorData().count() > 0)){
    if(data.value("Connector_Id_FK") != data.articleConnectorData().value("Connector_Id_FK")){
      QString msg = tr("Given unit connector data is not based on the same connector than article connector data");
      msg += tr(" (Unit connector ID: ") + data.value("Id_PK").toString();
      msg += tr(" , UnitConnector_tbl.Connector_Id_FK: ") + data.value("Connector_Id_FK").toString();
      msg += tr(" , ArticleConnector_tbl.Connector_Id_FK: ") + data.articleConnectorData().value("Connector_Id_FK").toString();
      msg += tr(")");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
      pvLastError.commit();
      return false;
    }
  }
  /*
   * Check unit connections:
   *  - If a unit connection is based on a article connection,
   *    and data is based on a article connector,
   *    article connection's ArticleConnector_Id_FK must match data's ArticleConnector_Id_FK
   */
  if(!articleConnectorId.isNull()){
    for(i = 0; i < data.connectionDataList().size(); ++i){
      if(data.connectionDataList().at(i).articleConnectionData().value("ArticleConnector_Id_FK") != articleConnectorId){
        QString msg = tr("Given unit connector data contains a connection based on a different article connector than unit connector is based on");
        msg += tr(" (Unit connector ID: ") + data.value("Id_PK").toString();
        msg += tr(" , unit connection ID: ") + data.connectionDataList().at(i).value("Id_PK").toString();
        msg += tr(" , based on article connection ID: ") + data.connectionDataList().at(i).value("ArticleConnection_Id_FK").toString();
        msg += tr(" , UnitConnector_tbl.ArticleConnector_Id_FK: ") + articleConnectorId.toString();
        msg += tr(" , ArticleConnection_tbl.ArticleConnector_Id_FK: ") + data.connectionDataList().at(i).articleConnectionData().value("ArticleConnector_Id_FK").toString();
        pvLastError.setError(msg, mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
        pvLastError.commit();
        return false;
      }
    }
  }

  // Should be OK
  return true;
}

QList<mdtClLinkData> mdtClUnit::getArticleLinkListUsingConnectionId(const mdtClUnitConnectionData & unitConnectionData, bool *ok)
{
  Q_ASSERT(ok != 0);
  Q_ASSERT(!unitConnectionData.value("Id_PK").isNull());

  mdtClLink lnk(0, database());
  QList<mdtClLinkData> linkDataList;
  QList<QSqlRecord> dataList;
  QList<QVariant> vtList;
  QString sql;
  int i;

  // Get assigned vehicle type list
  sql = "SELECT VehicleType_Id_FK FROM VehicleType_Unit_tbl WHERE Unit_Id_FK = " + unitConnectionData.value("Unit_Id_FK").toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return linkDataList;
  }
  for(i = 0; i < dataList.size(); ++i){
    vtList.append(dataList.at(i).value("VehicleType_Id_FK"));
  }
  // Get related article links
  sql = "SELECT UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK,";
  sql += " LinkType_Code_FK, LinkDirection_Code_FK, Identification, Value, SinceVersion, Modification ";
  sql += " FROM ArticleLink_UnitConnection_view ";
  sql += " WHERE ( UnitConnectionStart_Id_FK = " + unitConnectionData.value("Id_PK").toString();
  sql += " OR UnitConnectionEnd_Id_FK = " + unitConnectionData.value("Id_PK").toString();
  sql += " ) AND ( StartUnit_Id_FK = " + unitConnectionData.value("Unit_Id_FK").toString();
  sql += " AND EndUnit_Id_FK = " + unitConnectionData.value("Unit_Id_FK").toString();
  sql += " )";
  dataList = getData(sql, ok);
  if(!*ok){
    return linkDataList;
  }
  // Build data list
  for(i = 0; i < dataList.size(); ++i){
    linkDataList.append(dataList.at(i));
    if(!lnk.buildVehicleTypeLinkDataList(linkDataList[i], vtList, vtList)){
      *ok = false;
      return linkDataList;
    }
  }
  *ok = true;

  return linkDataList;
}

bool mdtClUnit::addArticleBasedLinkForUnitConnection(const mdtClUnitConnectionData & unitConnectionData)
{
  Q_ASSERT(!unitConnectionData.value("Id_PK").isNull());
  Q_ASSERT(!unitConnectionData.value("Unit_Id_FK").isNull());
  Q_ASSERT(!unitConnectionData.value("ArticleConnection_Id_FK").isNull());

  mdtClLink lnk(0, database());
  QList<mdtClLinkData> linkDataList;
  bool ok;

  linkDataList = getArticleLinkListUsingConnectionId(unitConnectionData, &ok);
  if(!ok){
    return false;
  }
  if(linkDataList.size() > 0){
    if(!lnk.addLinks(linkDataList, false)){
      return false;
    }
  }

  return true;
}

bool mdtClUnit::removeArticleBasedLinkForUnitConnection(const mdtClUnitConnectionData & unitConnectionData)
{
  Q_ASSERT(!unitConnectionData.value("Id_PK").isNull());
  Q_ASSERT(!unitConnectionData.value("Unit_Id_FK").isNull());
  Q_ASSERT(!unitConnectionData.value("ArticleConnection_Id_FK").isNull());

  mdtClLink lnk(0, database());
  QList<mdtClLinkData> linkDataList;
  bool ok;
  int i;

  linkDataList = getArticleLinkListUsingConnectionId(unitConnectionData, &ok);
  if(!ok){
    return false;
  }
  for(i = 0; i < linkDataList.size(); ++i){
    if(!lnk.removeLink(linkDataList.at(i).value("UnitConnectionStart_Id_FK"), linkDataList.at(i).value("UnitConnectionEnd_Id_FK"), false)){
      return false;
    }
  }

  return true;
}


bool mdtClUnit::addLinkToVehicleType(const QVariant &vehicleTypeStartId, const QVariant &vehicleTypeEndId, const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  qDebug() << "Adding link in vehicles links table, Unit start CNN ID: " << unitConnectionStartId << " , end CNN ID: " << unitConnectionEndId;
  
  // Prepare query for insertion in VehicleType_Link table
  sql = "INSERT INTO VehicleType_Link_tbl (VehicleTypeStart_Id_FK, VehicleTypeEnd_Id_FK, UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK) "\
        "VALUES (:VehicleTypeStart_Id_FK, :VehicleTypeEnd_Id_FK, :UnitConnectionStart_Id_FK, :UnitConnectionEnd_Id_FK)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for connection inertion in VehicleType_Link_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":VehicleTypeStart_Id_FK", vehicleTypeStartId);
  query.bindValue(":VehicleTypeEnd_Id_FK", vehicleTypeEndId);
  query.bindValue(":UnitConnectionStart_Id_FK", unitConnectionStartId);
  query.bindValue(":UnitConnectionEnd_Id_FK", unitConnectionEndId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for connection inertion in VehicleType_Link_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::removeLinkFromVehicleType(const QVariant &vehicleTypeStartId, const QVariant &vehicleTypeEndId, const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, QSqlQuery &query)
{
  QString sql;
  QSqlError sqlError;
  ///QSqlQuery query(database());

  sql = "DELETE FROM VehicleType_Link_tbl "\
        "WHERE VehicleTypeStart_Id_FK = " + vehicleTypeStartId.toString();
  sql += " AND VehicleTypeEnd_Id_FK = " + vehicleTypeEndId.toString();
  sql += " AND UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  qDebug() << "REM VHC Link, SQL: " << sql;
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to remove link from VehicleType_Link_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }

  return true;
}
