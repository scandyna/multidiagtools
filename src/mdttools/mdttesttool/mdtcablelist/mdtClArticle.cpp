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
#include "mdtClArticle.h"
#include "mdtError.h"
#include <QString>
#include <QStringList>
#include <QSqlQuery>

#include <QDebug>

mdtClArticle::mdtClArticle(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClArticle::~mdtClArticle()
{
}

QString mdtClArticle::sqlForArticleComponentSelection(const QVariant &articleId) const
{
  QString sql;

  sql =  "SELECT Id_PK, ArticleCode, Unit, DesignationEN "\
         "FROM Article_tbl "\
         "WHERE ( Id_PK <> " + articleId.toString() + " ) "\
         "AND ( Id_PK NOT IN ( "\
         " SELECT Component_Id_FK "\
         " FROM ArticleComponent_tbl "\
         " WHERE Composite_Id_FK = " + articleId.toString() + " ) "\
         " ) ";

  return sql;
}

bool mdtClArticle::addComponent(const QVariant &articleId, const QVariant &componentId, const QVariant &qty, const QVariant &qtyUnit)
{
  QString sql;
  QSqlQuery query(database());

  // Prepare query for insertion
  sql = "INSERT INTO ArticleComponent_tbl (Composite_Id_FK, Component_Id_FK, ComponentQty, ComponentQtyUnit) "\
        "VALUES (:Composite_Id_FK, :Component_Id_FK, :ComponentQty, :ComponentQtyUnit)";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for component inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Composite_Id_FK", articleId);
  query.bindValue(":Component_Id_FK", componentId);
  query.bindValue(":ComponentQty", qty);
  query.bindValue(":ComponentQtyUnit", qtyUnit);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::editComponent(const QVariant &articleId, const QVariant &currentComponentId, const QVariant &newComponentId, const QVariant &qty, const QVariant &qtyUnit)
{
  QString sql;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "UPDATE ArticleComponent_tbl "\
        "SET Component_Id_FK = :Component_Id_FK , "\
            "ComponentQty = :ComponentQty , "\
            "ComponentQtyUnit = :ComponentQtyUnit "\
        "WHERE ( Composite_Id_FK = " + articleId.toString();
  sql += " AND   Component_Id_FK = " + currentComponentId.toString() + ")";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for component edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Component_Id_FK", newComponentId);
  query.bindValue(":ComponentQty", qty);
  query.bindValue(":ComponentQtyUnit", qtyUnit);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}


bool mdtClArticle::removeComponent(const QVariant &articleId, const QVariant &componentId)
{
  return removeData("ArticleComponent_tbl", "Composite_Id_FK", articleId, "Component_Id_FK", componentId);
}

bool mdtClArticle::removeComponents(const QVariant &articleId, const QList<QVariant> &componentIdList)
{
  int i;
  QString sql;

  if(componentIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "DELETE FROM ArticleComponent_tbl ";
  for(i = 0; i < componentIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += " ( Component_Id_FK = " + componentIdList.at(i).toString();
    sql += " AND Composite_Id_FK = " + articleId.toString() + " ) ";
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::removeComponents(const QVariant &articleId, const QModelIndexList & indexListOfSelectedRows)
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return removeComponents(articleId, idList);
}

QList<QSqlRecord> mdtClArticle::getConnectorContactDataList(const QList<QVariant> & connectorContactIdList, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QStringList expectedFields;
  int i;

  /// \todo Is this a error or not ?
  if(connectorContactIdList.isEmpty()){
    return QList<QSqlRecord>();
  }
  // Setup query
  sql = "SELECT Id_PK, Connector_Id_FK, Name "\
        "FROM ConnectorContact_tbl ";
  Q_ASSERT(connectorContactIdList.size() > 0);
  sql += "WHERE Id_PK = " + connectorContactIdList.at(0).toString();
  for(i = 1; i < connectorContactIdList.size(); ++i){
    sql += " OR Id_PK = " + connectorContactIdList.at(i).toString();
  }
  expectedFields << "Id_PK" << "Connector_Id_FK" << "Name";

  return getData(sql, ok, expectedFields);
}

QList<QSqlRecord> mdtClArticle::getConnectionDataListFromConnectorContactDataList(const QList<QVariant> & connectorContactIdList, bool *ok)
{
  Q_ASSERT(ok != 0);

  QList<QSqlRecord> connectorContactDataList;
  QList<QSqlRecord> articleConnectionDataList;
  mdtSqlRecord articleConnectionData;
  int i;

  // Get connector contact data
  connectorContactDataList = getConnectorContactDataList(connectorContactIdList, ok);
  if(!*ok){
    return articleConnectionDataList;
  }
  // Create article contact data list
  if(!articleConnectionData.addAllFields("ArticleConnection_tbl", database())){
    pvLastError = articleConnectionData.lastError();
    *ok = false;
    return articleConnectionDataList;
  }
  for(i = 0; i < connectorContactDataList.size(); ++i){
    articleConnectionData.clearValues();
    articleConnectionData.setValue("ArticleContactName", connectorContactDataList.at(i).value("Name"));
    articleConnectionDataList.append(articleConnectionData);
  }
  *ok = true;

  return articleConnectionDataList;
}

bool mdtClArticle::addConnection(const mdtSqlRecord &data)
{
  return addRecord(data, "ArticleConnection_tbl");
}

bool mdtClArticle::addConnections(const QList<mdtSqlRecord> & dataList, bool singleTransaction)
{
  return addRecordList(dataList, "ArticleConnection_tbl", singleTransaction);
}

bool mdtClArticle::removeConnection(const QVariant & articleConnectionId)
{
  QList<QVariant> idList;

  idList.append(articleConnectionId);

  return removeConnections(idList);
}

bool mdtClArticle::removeConnections(const QList<QVariant> & articleConnectionIdList)
{
  int i;
  QString sql;

  if(articleConnectionIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "DELETE FROM ArticleConnection_tbl ";
  for(i = 0; i < articleConnectionIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += " Id_PK = " + articleConnectionIdList.at(i).toString();
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for connection deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::removeConnections(const QModelIndexList & indexListOfSelectedRows)
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return removeConnections(idList);
}

bool mdtClArticle::removeConnectorConnections(const QVariant & articleConnectorId)
{
  QList<QVariant> idList;

  idList.append(articleConnectorId);

  return removeConnectorsConnections(idList);
}

bool mdtClArticle::removeConnectorsConnections(const QList<QVariant> & articleConnectorIdList)
{
  int i;
  QString sql;

  if(articleConnectorIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "DELETE FROM ArticleConnection_tbl ";
  for(i = 0; i < articleConnectorIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += " ArticleConnector_Id_FK = " + articleConnectorIdList.at(i).toString();
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for connector's connection deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::addConnector(const mdtSqlRecord & articleConnectorData, const QList<QSqlRecord> & articleConnectionDataList)
{
  QString sql;
  QSqlQuery query(database());
  QList<mdtSqlRecord> _articleConnectionDataList; // Because we update FKs, we need a copy
  QVariant articleConnectorId;
  QVariant articleId;
  int i;

  // We accept that calling this method without data is not a error
  if(articleConnectionDataList.isEmpty()){
    return true;
  }

  // Because we add connector and get its ID for connections insertion, we use a transaction for the whole process
  if(!beginTransaction()){
    return false;
  }
  // Get article ID
  articleId = articleConnectorData.value("Article_Id_FK");
  if(articleId.isNull()){
    pvLastError.setError(tr("Given article connector data contains not a valid article ID."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  // Add article connector
  if(!addRecord(articleConnectorData, "ArticleConnector_tbl", query)){
    rollbackTransaction();
    return false;
  }
  // Get article connector ID and update data
  articleConnectorId = query.lastInsertId();
  if(articleConnectorId.isNull()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot get article connector ID for connector inertion."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  // Update connection data
  for(i = 0; i < articleConnectionDataList.size(); ++i){
    mdtSqlRecord record = articleConnectionDataList.at(i);
    record.setValue("Article_Id_FK", articleId);
    record.setValue("ArticleConnector_Id_FK", articleConnectorId);
    _articleConnectionDataList.append(record);
  }
  // Add connections
  if(!addConnections(_articleConnectionDataList, false)){
    rollbackTransaction();
    return false;
  }
  // Commit
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtClArticle::removeConnector(const QVariant & articleConnectorId)
{
  if(!beginTransaction()){
    return false;
  }
  // Remove connections
  if(!removeData("ArticleConnection_tbl", "ArticleConnector_Id_FK", articleConnectorId)){
    rollbackTransaction();
    return false;
  }
  // Remove connector
  if(!removeData("ArticleConnector_tbl", "Id_PK", articleConnectorId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtClArticle::removeConnectors(const QModelIndexList & indexListOfSelectedRows)
{
  if(!beginTransaction()){
    return false;
  }
  // Remove connections
  if(!removeData("ArticleConnection_tbl", "ArticleConnector_Id_FK", indexListOfSelectedRows)){
    rollbackTransaction();
    return false;
  }
  // Remove connectors
  if(!removeData("ArticleConnector_tbl", "Id_PK", indexListOfSelectedRows)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

/**
bool mdtClArticle::removeConnectors(const QList<QVariant> & articleConnectorIdList)
{
  int i;
  QString sql;

  if(articleConnectorIdList.size() < 1){
    return true;
  }
  if(!beginTransaction()){
    return false;
  }
  // Generate SQL to remove connectors
  sql = "DELETE FROM ArticleConnector_tbl ";
  for(i = 0; i < articleConnectorIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += " Id_PK = " + articleConnectorIdList.at(i).toString();
  }
  sql += " ) ";
  // Remove connections
  if(!removeConnectorsConnections(articleConnectorIdList)){
    rollbackTransaction();
    return false;
  }
  // Submit query to remove connectors
  QSqlQuery query(database());
  if(!query.exec(sql)){
    rollbackTransaction();
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for connector's connection deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}
*/

bool mdtClArticle::addLink(const mdtSqlRecord & linkData)
{
  return addRecord(linkData, "ArticleLink_tbl");
}

bool mdtClArticle::addCableLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const QVariant & identification, const QVariant & R)
{
  mdtSqlRecord record;

  if(!record.addAllFields("ArticleLink_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("ArticleConnectionStart_Id_FK", articleConnectionStartId);
  record.setValue("ArticleConnectionEnd_Id_FK", articleConnectionEndId);
  record.setValue("LinkType_Code_FK", "CABLELINK");
  record.setValue("LinkDirection_Code_FK", "BID");
  record.setValue("Identification", identification);
  record.setValue("Value", R);

  return addRecord(record, "ArticleLink_tbl");
}

/*
bool mdtClArticle::addResistor(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value)
{
  return addLink(articleConnectionStartId, articleConnectionEndId, value, "BID", "RESISTOR");
}

bool mdtClArticle::addDiode(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double Vf, const QVariant & directionCode)
{
  return addLink(articleConnectionStartId, articleConnectionEndId, Vf, directionCode, "DIODE");
}

bool mdtClArticle::addBridge(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId)
{
  return addLink(articleConnectionStartId, articleConnectionEndId, 0.0, "BID", "ARTBRIDGE");
}
*/

bool mdtClArticle::editLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const mdtSqlRecord & data)
{
  mdtSqlRecord matchData;

  // Setup row condition
  if(!matchData.addField("ArticleConnectionStart_Id_FK", "ArticleLink_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  if(!matchData.addField("ArticleConnectionEnd_Id_FK", "ArticleLink_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  matchData.setValue("ArticleConnectionStart_Id_FK", articleConnectionStartId);
  matchData.setValue("ArticleConnectionEnd_Id_FK", articleConnectionEndId);

  return updateRecord("ArticleLink_tbl", data, matchData);
}

bool mdtClArticle::removeLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId)
{
  return removeData("ArticleLink_tbl", "ArticleConnectionStart_Id_FK", articleConnectionStartId, "ArticleConnectionEnd_Id_FK", articleConnectionEndId);
}

bool mdtClArticle::removeLinks(const QList<QModelIndexList> &indexListOfSelectedRowsByRows)
{
  int row;
  QModelIndexList indexes;

  if(!beginTransaction()){
    return false;
  }
  for(row = 0; row < indexListOfSelectedRowsByRows.size(); ++row){
    indexes = indexListOfSelectedRowsByRows.at(row);
    Q_ASSERT(indexes.size() == 2);
    if(!removeLink(indexes.at(0).data(), indexes.at(1).data())){
      rollbackTransaction();
      return false;
    }
  }

  return commitTransaction();
}
