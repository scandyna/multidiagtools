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
#include "mdtClArticleConnection.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTransaction.h"
#include <QSqlQuery>
#include <QList>

#include <QDebug>

mdtClArticleConnection::mdtClArticleConnection(QObject *parent, QSqlDatabase db)
 : mdtClConnector(parent, db)
{
}

mdtClArticleConnection::mdtClArticleConnection(QSqlDatabase db) 
 : mdtClConnector(db)
{
}

mdtClArticleConnectionKeyData mdtClArticleConnection::addArticleConnection(const mdtClArticleConnectionData &data, bool handleTransaction)
{
  mdtClArticleConnectionKeyData key;
  mdtSqlRecord record;
  QSqlQuery query(database());
  mdtSqlTransaction transaction(database());

  // Setup record with given data
  if(!record.addAllFields("ArticleConnection_tbl", database())){
    pvLastError = record.lastError();
    return key;
  }
  fillRecord(record, data);
  // Save to database
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return key;
    }
  }
  if(!addRecord(record, "ArticleConnection_tbl", query)){
    return key;
  }
  key = data.keyData();
  key.id = query.lastInsertId();
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      key.clear();
      return key;
    }
  }

  return key;
}

bool mdtClArticleConnection::addArticleConnectionList(const QList<mdtClArticleConnectionData> & dataList, bool handleTransaction)
{
  mdtSqlRecord record;
  mdtSqlTransaction transaction(database());

  // Setup record with given data
  if(!record.addAllFields("ArticleConnection_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  // Save to database
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  for(const auto & data : dataList){
    fillRecord(record, data);
    if(!addRecord(record, "ArticleConnection_tbl")){
      return false;
    }
  }
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

mdtClArticleConnectionData mdtClArticleConnection::getArticleConnectionData(const mdtClArticleConnectionKeyData &key, bool &ok)
{
  mdtClArticleConnectionData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT ACNX.* , ACNR.Connector_Id_FK FROM ArticleConnection_tbl ACNX LEFT JOIN ArticleConnector_tbl ACNR ON ACNR.Id_PK = ACNX.ArticleConnector_Id_FK";
  sql += " WHERE ACNX.Id_PK = " + key.id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  fillData(data, dataList.at(0));

  return data;
}

QList<mdtClArticleConnectionData> mdtClArticleConnection::getArticleConnectionDataList(const mdtClArticleConnectorKeyData & key, bool & ok)
{
  QList<mdtClArticleConnectionData> dataList;
  QList<QSqlRecord> recordList;
  QString sql;

  sql = "SELECT ACNX.* , ACNR.Connector_Id_FK FROM ArticleConnection_tbl ACNX LEFT JOIN ArticleConnector_tbl ACNR ON ACNR.Id_PK = ACNX.ArticleConnector_Id_FK";
  sql += " WHERE ACNX.ArticleConnector_Id_FK = " + key.id.toString();
  recordList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return dataList;
  }
  for(const auto & record : recordList){
    mdtClArticleConnectionData data;
    fillData(data, record);
    dataList.append(data);
  }

  return dataList;
}

bool mdtClArticleConnection::updateArticleConnection(const mdtClArticleConnectionKeyData & key, const mdtClArticleConnectionData & data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("ArticleConnection_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, data);

  return updateRecord("ArticleConnection_tbl", record, "Id_PK", key.id);
}

bool mdtClArticleConnection::removeArticleConnection(const mdtClArticleConnectionKeyData &key)
{
  return removeData("ArticleConnection_tbl", "Id_PK", key.id.toString());
}

bool mdtClArticleConnection::removeArticleConnections(const mdtSqlTableSelection &s)
{
  QList<QVariant> idList = s.dataList("Id_PK");
  mdtSqlTransaction transaction(database());

  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  for(const auto & id : idList){
    mdtClArticleConnectionKeyData key;
    key.id = id;
    if(!removeArticleConnection(key)){
      return false;
    }
  }
  if(!transaction.commit()){
    pvLastError = transaction.lastError();
    return false;
  }

  return true;
}

mdtClArticleConnectorKeyData mdtClArticleConnection::addArticleConnector(mdtClArticleConnectorData data, bool handleTransaction)
{
  mdtClArticleConnectorKeyData key;
  mdtSqlRecord record;
  QSqlQuery query(database());
  mdtSqlTransaction transaction(database());

  // Setup record with given data
  if(!record.addAllFields("ArticleConnector_tbl", database())){
    pvLastError = record.lastError();
    return key;
  }
  record.setValue("Id_PK", data.keyData().id);
  record.setValue("Article_Id_FK", data.keyData().articleId);
  record.setValue("Connector_Id_FK", data.keyData().connectorFk.id);
  record.setValue("Name", data.name);
  // Save connector to database
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return key;
    }
  }
  if(!addRecord(record, "ArticleConnector_tbl", query)){
    return key;
  }
  // Setup key to return
  key = data.keyData();
  key.id = query.lastInsertId();
  // Save connections to database
  if(data.connectionDataList().size() > 0){
    data.setKeyData(key);
    if(!addArticleConnectionList(data.connectionDataList(), false)){
      key.clear();
      return key;
    }
  }
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      key.clear();
      return key;
    }
  }

  return key;
}

mdtClArticleConnectorData mdtClArticleConnection::getArticleConnectorData(mdtClArticleConnectorKeyData key, bool includeConnectionData, bool &ok)
{
  mdtClArticleConnectorData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT * FROM ArticleConnector_tbl WHERE Id_PK = " + key.id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  // Set key data
  key.articleId = dataList.at(0).value("Article_Id_FK");
  key.connectorFk.id = dataList.at(0).value("Connector_Id_FK");
  // Set article connector data
  data.setKeyData(key);
  data.name = dataList.at(0).value("Name");
  if(includeConnectionData){
    data.setConnectionDataList(getArticleConnectionDataList(data.keyData(), ok));
    if(!ok){
      data.clear();
    }
  }

  return data;
}

bool mdtClArticleConnection::removeArticleConnector(const mdtClArticleConnectorKeyData & key, bool handleTransaction)
{
  mdtSqlTransaction transaction(database());

  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  if(!removeData("ArticleConnection_tbl", "ArticleConnector_Id_FK", key.id.toString())){
    return false;
  }
  if(!removeData("ArticleConnector_tbl", "Id_PK", key.id.toString())){
    return false;
  }
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

void mdtClArticleConnection::addConnectionsToArticleConnector(mdtClArticleConnectorData & acnrData, const QList< mdtClConnectorContactData > & contactDataList)
{
  for(const auto & ccData : contactDataList){
    mdtClArticleConnectionData acData;
    acData.setConnectionType(ccData.keyData().connectionTypeFk);
    acData.name = ccData.name;
    acnrData.addConnectionData(acData);
  }
}

void mdtClArticleConnection::fillRecord(mdtSqlRecord &record, const mdtClArticleConnectionData &data)
{
  Q_ASSERT(!record.isEmpty());

  auto key = data.keyData();

  record.clearValues();
  record.setValue("Id_PK", key.id);
  record.setValue("Article_Id_FK", key.articleId);
  record.setValue("ArticleConnector_Id_FK", key.articleConnectorFk.id);
  record.setValue("ConnectionType_Code_FK", key.connectionTypeFk.code);
  record.setValue("ArticleContactName", data.name);
  record.setValue("Resistance", data.resistance);
  record.setValue("IoType", data.ioType);
  record.setValue("FunctionEN", data.functionEN);
  record.setValue("FunctionFR", data.functionFR);
  record.setValue("FunctionDE", data.functionDE);
  record.setValue("FunctionIT", data.functionIT);
}

void mdtClArticleConnection::fillData(mdtClArticleConnectionData &data, const QSqlRecord &record)
{
  Q_ASSERT(record.contains("Id_PK"));
  Q_ASSERT(record.contains("Article_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnector_Id_FK"));
  Q_ASSERT(record.contains("ConnectionType_Code_FK"));
  Q_ASSERT(record.contains("ArticleContactName"));
  Q_ASSERT(record.contains("Resistance"));
  Q_ASSERT(record.contains("IoType"));
  Q_ASSERT(record.contains("functionEN"));
  Q_ASSERT(record.contains("functionFR"));
  Q_ASSERT(record.contains("functionDE"));
  Q_ASSERT(record.contains("functionIT"));
  Q_ASSERT(record.contains("Connector_Id_FK")); // From Connector_tbl

  mdtClArticleConnectionKeyData key;
  // Fill key
  key.id = record.value("Id_PK");
  key.articleId = record.value("Article_Id_FK");
  key.articleConnectorFk.id = record.value("ArticleConnector_Id_FK");
  key.articleConnectorFk.articleId = key.articleId;
  key.articleConnectorFk.connectorFk.id = record.value("Connector_Id_FK");
  key.connectionTypeFk.code = record.value("ConnectionType_Code_FK");
  // Fill data
  data.setKeyData(key);
  data.name = record.value("ArticleContactName");
  data.resistance = record.value("Resistance");
  data.ioType = record.value("IoType");
  data.functionEN = record.value("FunctionEN");
  data.functionFR = record.value("FunctionFR");
  data.functionDE = record.value("FunctionDE");
  data.functionIT = record.value("FunctionIT");
}
