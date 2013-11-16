/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include <QSqlQuery>

#include <QDebug>

mdtClArticle::mdtClArticle(QSqlDatabase db)
 : mdtClBase(db)
{
  pvArticleModel = new QSqlQueryModel;
}

mdtClArticle::~mdtClArticle()
{
  delete pvArticleModel;
}

QSqlQueryModel *mdtClArticle::articleModelForComponentSelection(const QVariant &articleId)
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
  pvArticleModel->setQuery(sql, database());

  return pvArticleModel;
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
  QList<QVariant> idList;

  idList.append(componentId);

  return removeComponents(articleId, idList);
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

QList<mdtClArticleConnectionData> mdtClArticle::connectorContactData(const QList<QVariant> & connectorContactIdList)
{
  QString sql;
  QSqlQuery query(database());
  QList<mdtClArticleConnectionData> dataList;
  int i;

  if(connectorContactIdList.isEmpty()){
    return dataList;
  }
  // Setup query
  sql = "SELECT Id_PK, Connector_Id_FK, Name "\
        "FROM ConnectorContact_tbl ";
  Q_ASSERT(connectorContactIdList.size() > 0);
  sql += "WHERE Id_PK = " + connectorContactIdList.at(0).toString();
  for(i = 1; i < connectorContactIdList.size(); ++i){
    sql += " OR Id_PK = " + connectorContactIdList.at(i).toString();
  }
  // Exec query
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get connection contacts data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return dataList;
  }
  // Get data
  while(query.next()){
    mdtClArticleConnectionData data;
    data.setConnectionId(query.value(0));
    data.setConnectorId(query.value(1));
    data.setContactName(query.value(2));
    dataList.append(data);
  }

  return dataList;
}

bool mdtClArticle::addConnection(const mdtClArticleConnectionData &data)
{
  QString sql;
  QSqlQuery query(database());

  // Prepare query for insertion
  sql = "INSERT INTO ArticleConnection_tbl "\
        "(Article_Id_FK, ArticleConnector_Id_FK, ArticleContactName, IoType, FunctionEN, FunctionFR, FunctionDE, FunctionIT) "\
        "VALUES (:Article_Id_FK, :ArticleConnector_Id_FK, :ArticleContactName, :IoType, :FunctionEN, :FunctionFR, :FunctionDE, :FunctionIT)";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for connection inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Article_Id_FK", data.articleId());
  query.bindValue(":ArticleConnector_Id_FK", data.articleConnectorId());
  query.bindValue(":ArticleContactName", data.contactName());
  query.bindValue(":IoType", data.ioType());
  query.bindValue(":FunctionEN", data.functionEN());
  query.bindValue(":FunctionFR", data.functionFR());
  query.bindValue(":FunctionDE", data.functionDE());
  query.bindValue(":FunctionIT", data.functionIT());
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for connection inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::addConnections(const QList<mdtClArticleConnectionData> & dataList, bool singleTransaction)
{
  int i;

  if(singleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  for(i = 0; i < dataList.size(); ++i){
    if(!addConnection(dataList.at(i))){
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

bool mdtClArticle::addConnector(const QList<mdtClArticleConnectionData> & dataList)
{
  QString sql;
  QSqlQuery query(database());
  QList<mdtClArticleConnectionData> connectionDataList;
  QVariant articleConnectorId;
  int i;

  // We accept that calling this method without data is not a error
  if(dataList.isEmpty()){
    return true;
  }
  connectionDataList = dataList;

#ifndef QT_NO_DEBUG
  QVariant id;
  Q_ASSERT(connectionDataList.size() > 0);
  id = connectionDataList.at(0).articleId();
  for(i = 0; i < connectionDataList.size(); ++i){
    Q_ASSERT(connectionDataList.at(i).articleId() == id);
    id = connectionDataList.at(i).articleId();
  }
#endif  // Q_DEBUG

  // Because we add connector and get its ID for connections insertion, we use a transaction for the whole process
  if(!beginTransaction()){
    return false;
  }
  // Prepare query for insertion
  sql = "INSERT INTO ArticleConnector_tbl "\
        "(Article_Id_FK, Connector_Id_FK, Name) "\
        "VALUES (:Article_Id_FK, :Connector_Id_FK, :Name) ";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for connector inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  // Add values and execute query
  Q_ASSERT(connectionDataList.size() > 0);
  query.bindValue(":Article_Id_FK", connectionDataList.at(0).articleId());
  query.bindValue(":Connector_Id_FK", connectionDataList.at(0).connectorId());
  query.bindValue(":Name", connectionDataList.at(0).connectorName());
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for connector inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  // Get article connector ID and update data
  articleConnectorId = query.lastInsertId();
  if(articleConnectorId.isNull()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get article connector ID for connector inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    rollbackTransaction();
    return false;
  }
  for(i = 0; i < connectionDataList.size(); ++i){
    connectionDataList[i].setArticleConnectorId(articleConnectorId);
  }
  // Add connections
  if(!addConnections(connectionDataList, false)){
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
  QList<QVariant> idList;

  idList.append(articleConnectorId);

  return removeConnectors(idList);
}

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

bool mdtClArticle::addLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode)
{
  QString sql;
  QSqlQuery query(database());

  // Prepare query for insertion
  sql = "INSERT INTO ArticleLink_tbl (ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, Value, LinkDirection_Code_FK, LinkType_Code_FK)\
                  VALUES (:ArticleConnectionStart_Id_FK, :ArticleConnectionEnd_Id_FK, :Value, :LinkDirection_Code_FK, :LinkType_Code_FK)";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for link insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":ArticleConnectionStart_Id_FK", articleConnectionStartId);
  query.bindValue(":ArticleConnectionEnd_Id_FK", articleConnectionEndId);
  query.bindValue(":Value", value);
  query.bindValue(":LinkDirection_Code_FK", directionCode);
  query.bindValue(":LinkType_Code_FK", typeCode);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for link insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

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

bool mdtClArticle::editLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const mdtClLinkData &data)
{
  QString sql;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "UPDATE ArticleLink_tbl "\
        " SET ArticleConnectionStart_Id_FK = :ArticleConnectionStart_Id_FK ,"\
        "  ArticleConnectionEnd_Id_FK = :ArticleConnectionEnd_Id_FK ,"\
        "  Value = :Value ,"\
        "  LinkDirection_Code_FK = :LinkDirection_Code_FK ,"\
        "  LinkType_Code_FK = :LinkType_Code_FK "\
        " WHERE ArticleConnectionStart_Id_FK = " + articleConnectionStartId.toString() +\
        " AND ArticleConnectionEnd_Id_FK = " + articleConnectionEndId.toString();
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":ArticleConnectionStart_Id_FK", data.articleConnectionStartId());
  query.bindValue(":ArticleConnectionEnd_Id_FK", data.articleConnectionEndId());
  query.bindValue(":Value", data.value());
  query.bindValue(":LinkDirection_Code_FK", data.linkDirectionCode());
  query.bindValue(":LinkType_Code_FK", data.linkTypeCode());
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::removeLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId)
{
  QString sql;
  QSqlQuery query(database());

  // Remove links
  sql = "DELETE FROM ArticleLink_tbl ";
  sql += " WHERE ArticleConnectionStart_Id_FK = " + articleConnectionStartId.toString();
  sql += " AND ArticleConnectionEnd_Id_FK = " + articleConnectionEndId.toString();
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to remove link", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
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
