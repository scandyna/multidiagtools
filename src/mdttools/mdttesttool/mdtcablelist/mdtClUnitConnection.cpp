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
#include "mdtClUnitConnection.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTransaction.h"
#include <QSqlQuery>
#include <QList>

//#include <QDebug>

mdtClUnitConnection::mdtClUnitConnection(QObject *parent, QSqlDatabase db)
 : mdtClArticleConnection(parent, db)
{
}

mdtClUnitConnection::mdtClUnitConnection(QSqlDatabase db)
 : mdtClUnitConnection(nullptr, db)
{
}

mdtClUnitConnectionKeyData mdtClUnitConnection::addUnitConnection(const mdtClUnitConnectionData & data, bool handleTransaction)
{
  mdtClUnitConnectionKeyData key;
  mdtSqlRecord record;
  QSqlQuery query(database());
  mdtSqlTransaction transaction(database());

  // Setup record with given data
  if(!record.addAllFields("UnitConnection_tbl", database())){
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
  if(!addRecord(record, "UnitConnection_tbl", query)){
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

bool mdtClUnitConnection::addUnitConnectionList(const QList<mdtClUnitConnectionData> & dataList, bool handleTransaction)
{
  mdtSqlTransaction transaction(database());

  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  for(const auto & data : dataList){
    if(addUnitConnection(data, false).isNull()){
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

mdtClUnitConnectionData mdtClUnitConnection::getUnitConnectionData(const mdtClUnitConnectionKeyData & key, bool & ok)
{
  mdtClUnitConnectionData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnection();
  sql += " WHERE UCNX.Id_PK = " + key.id.toString();
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

mdtClUnitConnectionData mdtClUnitConnection::getUnitConnectionData(const mdtClArticleConnectionKeyData & key, bool &ok)
{
  mdtClUnitConnectionData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnection();
  sql += " WHERE UCNX.ArticleConnection_Id_FK = " + key.id.toString();
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

QList<mdtClUnitConnectionData> mdtClUnitConnection::getUnitConnectionDataList(const mdtClUnitConnectorPkData & pk, bool & ok)
{
  QList<mdtClUnitConnectionData> dataList;
  QList<QSqlRecord> recordList;
  QString sql;

  sql = baseSqlForUnitConnection();
  sql += " WHERE UCNX.UnitConnector_Id_FK = " + pk.id.toString();
  recordList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return dataList;
  }
  for(const auto & record : recordList){
    mdtClUnitConnectionData data;
    fillData(data, record);
    dataList.append(data);
  }

  return dataList;
}

bool mdtClUnitConnection::updateUnitConnection(const mdtClUnitConnectionKeyData & key, const mdtClUnitConnectionData & data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("UnitConnection_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, data);

  return updateRecord("UnitConnection_tbl", record, "Id_PK", key.id);
}

bool mdtClUnitConnection::removeUnitConnection(const mdtClUnitConnectionKeyData & key)
{
  return removeData("UnitConnection_tbl", "Id_PK", key.id);
}

bool mdtClUnitConnection::removeUnitConnections(const mdtSqlTableSelection & s)
{
  QList<QVariant> idList = s.dataList("Id_PK");
  mdtSqlTransaction transaction(database());

  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  for(const auto & id : idList){
    mdtClUnitConnectionKeyData key;
    key.id = id;
    if(!removeUnitConnection(key)){
      return false;
    }
  }
  if(!transaction.commit()){
    pvLastError = transaction.lastError();
    return false;
  }

  return true;
}

mdtClUnitConnectorPkData mdtClUnitConnection::addUnitConnector(mdtClUnitConnectorData data, bool handleTransaction)
{
  mdtClUnitConnectorPkData pk;
  mdtSqlRecord record;
  QSqlQuery query(database());
  mdtSqlTransaction transaction(database());

  // Setup record with given data
  if(!record.addAllFields("UnitConnector_tbl", database())){
    pvLastError = record.lastError();
    return pk;
  }
  fillRecord(record, data);
  // Save connector to database
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return pk;
    }
  }
  if(!addRecord(record, "UnitConnector_tbl", query)){
    return pk;
  }
  // Setup key to return
  ///key = data.keyData();
  pk.id = query.lastInsertId();
  // Save connections to database
  if(data.connectionDataList().size() > 0){
    data.setId(pk.id);
    if(!addUnitConnectionList(data.connectionDataList(), false)){
      pk.clear();
      return pk;
    }
  }
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      pk.clear();
      return pk;
    }
  }

  return pk;
}

mdtClUnitConnectorData mdtClUnitConnection::getUnitConnectorData(mdtClUnitConnectorPkData pk, bool includeConnectionData, bool &ok)
{
  mdtClUnitConnectorData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnector();
  sql += " WHERE UCNR.Id_PK = " + pk.id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  fillData(data, dataList.at(0));
  if(includeConnectionData){
    data.setConnectionDataList(getUnitConnectionDataList(pk, ok));
    if(!ok){
      data.clear();
    }
  }

  return data;
}

mdtClUnitConnectorData mdtClUnitConnection::getUnitConnectorData(mdtClUnitConnectionKeyData key, bool &ok)
{
  mdtClUnitConnectorData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnector();
  sql += " JOIN UnitConnection_tbl UCNX ON UCNX.UnitConnector_Id_FK = UCNR.Id_PK";
  sql += " WHERE UCNX.Id_PK = " + key.id.toString();
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

mdtClUnitConnectorKeyData mdtClUnitConnection::getUnitConnectorKeyData(const mdtClArticleConnectionKeyData & key, bool & ok)
{
  mdtClUnitConnectorKeyData unitConnectorKey;
  mdtClUnitConnectorData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnector();
  sql += " LEFT JOIN ArticleConnection_tbl ACNX ON ACNX.ArticleConnector_Id_FK = ACNR.Id_PK\n";
  sql += " WHERE ACNX.Id_PK = " + key.id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return unitConnectorKey;
  }
  if(dataList.isEmpty()){
    return unitConnectorKey;
  }
  Q_ASSERT(dataList.size() == 1);
  fillData(data, dataList.at(0));
  unitConnectorKey = data.keyData();

  return unitConnectorKey;
}

bool mdtClUnitConnection::updateUnitConnectorName(const mdtClUnitConnectorPkData & pk, const QVariant &name)
{
  mdtSqlRecord record;

  if(!record.addAllFields("UnitConnector_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("Name", name);

  return updateRecord("UnitConnector_tbl", record, "Id_PK", pk.id);
}

bool mdtClUnitConnection::removeUnitConnector(const mdtClUnitConnectorPkData & pk, bool handleTransaction)
{
  mdtSqlTransaction transaction(database());

  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  /// \todo when links are implemented, this must be changed
  if(!removeData("UnitConnection_tbl", "UnitConnector_Id_FK", pk.id.toString())){
    return false;
  }
  if(!removeData("UnitConnector_tbl", "Id_PK", pk.id.toString())){
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

bool mdtClUnitConnection::removeUnitConnectors(const mdtSqlTableSelection & s)
{
  QList<QVariant> idList = s.dataList("Id_PK");
  mdtSqlTransaction transaction(database());

  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  for(const auto & id : idList){
    mdtClUnitConnectorPkData pk;
    pk.id = id;
    if(!removeUnitConnector(pk, false)){
      return false;
    }
  }
  if(!transaction.commit()){
    pvLastError = transaction.lastError();
    return false;
  }

  return true;
}

void mdtClUnitConnection::addConnectionsToUnitConnector(mdtClUnitConnectorData & ucnrData, const QList<mdtClConnectorContactData> & contactDataList)
{
  for(const auto & ccData : contactDataList){
    mdtClUnitConnectionData ucData;
    ucData.setConnectionType(ccData.connectionType());
    ucData.name = ccData.name;
    ucnrData.addConnectionData(ucData);
  }
}

void mdtClUnitConnection::addConnectionsToUnitConnector(mdtClUnitConnectorData & ucnrData, const QList<mdtClArticleConnectionData> & articleConnectionDataList)
{
  for(const auto & acnxData : articleConnectionDataList){
    mdtClUnitConnectionKeyData ucKey;
    mdtClUnitConnectionData ucData;
    ucKey.setArticleConnectionFk(acnxData.keyData());
    ucData.setKeyData(ucKey);
    ucData.name = acnxData.name;
    ucnrData.addConnectionData(ucData);
  }
}

QString mdtClUnitConnection::baseSqlForUnitConnection() const
{
  return "SELECT UCNX.*,\n"\
        " UCNR.Connector_Id_FK AS UCNR_Connector_Id_FK,\n"\
        " UCNR.ArticleConnector_Id_FK AS UCNR_ArticleConnector_Id_FK,\n"\
        " ACNX.ArticleConnector_Id_FK AS ACNX_ArticleConnector_Id_FK,\n"\
        " ACNX.Article_Id_FK,\n"\
        " ACNX.ConnectionType_Code_FK AS ACNX_ConnectionType_Code_FK,\n"\
        " ACNR.Connector_Id_FK AS ACNR_Connector_Id_FK\n"\
        "FROM UnitConnection_tbl UCNX\n"\
        " LEFT JOIN UnitConnector_tbl UCNR ON UCNR.Id_PK = UCNX.UnitConnector_Id_FK\n"\
        " LEFT JOIN ArticleConnection_tbl ACNX ON ACNX.Id_PK = UCNX.ArticleConnection_Id_FK\n"\
        " LEFT JOIN ArticleConnector_tbl ACNR ON ACNR.Id_PK = ACNX.ArticleConnector_Id_FK\n";
}

void mdtClUnitConnection::fillRecord(mdtSqlRecord & record, const mdtClUnitConnectionData & data)
{
  Q_ASSERT(!record.isEmpty());

  auto key = data.keyData();

  record.clearValues();
  record.setValue("Id_PK", key.id);
  record.setValue("Unit_Id_FK", key.unitId());
  record.setValue("UnitConnector_Id_FK", key.unitConnectorFk().pk.id);
  record.setValue("ArticleConnection_Id_FK", key.articleConnectionFk().id);
  record.setValue("ConnectionType_Code_FK", key.connectionTypeFk().code);
  record.setValue("UnitContactName", data.name);
  record.setValue("Resistance", data.resistance);
  record.setValue("SchemaPage", data.schemaPage);
  record.setValue("SignalName", data.signalName);
  record.setValue("SwAddress", data.swAddress);
  record.setValue("FunctionEN", data.functionEN);
  record.setValue("FunctionFR", data.functionFR);
  record.setValue("FunctionDE", data.functionDE);
  record.setValue("FunctionIT", data.functionIT);
}

void mdtClUnitConnection::fillData(mdtClUnitConnectionData & data, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("Id_PK"));
  Q_ASSERT(record.contains("Unit_Id_FK"));
  Q_ASSERT(record.contains("UnitConnector_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnection_Id_FK"));
  Q_ASSERT(record.contains("ConnectionType_Code_FK"));
  Q_ASSERT(record.contains("UnitContactName"));
  Q_ASSERT(record.contains("Resistance"));
  Q_ASSERT(record.contains("SchemaPage"));
  Q_ASSERT(record.contains("SignalName"));
  Q_ASSERT(record.contains("SwAddress"));
  Q_ASSERT(record.contains("functionEN"));
  Q_ASSERT(record.contains("functionFR"));
  Q_ASSERT(record.contains("functionDE"));
  Q_ASSERT(record.contains("functionIT"));
  Q_ASSERT(record.contains("UCNR_Connector_Id_FK"));
  Q_ASSERT(record.contains("UCNR_ArticleConnector_Id_FK"));
  Q_ASSERT(record.contains("ACNX_ArticleConnector_Id_FK"));
  Q_ASSERT(record.contains("Article_Id_FK"));
  Q_ASSERT(record.contains("ACNX_ConnectionType_Code_FK"));
  Q_ASSERT(record.contains("ACNR_Connector_Id_FK"));

  mdtClConnectorKeyData connectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClUnitConnectorKeyData unitConnectorFk;
  mdtClArticleConnectionKeyData articleConnectionFk;
  mdtClUnitConnectionKeyData key;

  // Setup article connector FK
  connectorFk.id = record.value("ACNR_Connector_Id_FK");
  articleConnectorFk.id = record.value("ACNX_ArticleConnector_Id_FK");
  articleConnectorFk.setArticleId(record.value("Article_Id_FK"));
  if( (!articleConnectorFk.isNull())&&(!connectorFk.isNull()) ){
    articleConnectorFk.setConnectorFk(connectorFk);
  }
  // Setup unit connector FK
  unitConnectorFk.pk.id = record.value("UnitConnector_Id_FK");
  unitConnectorFk.setUnitId(record.value("Unit_Id_FK"));
  if(!articleConnectorFk.isNull()){
    unitConnectorFk.setArticleConnectorFk(articleConnectorFk);
  }else{
    connectorFk.id = record.value("UCNR_Connector_Id_FK");
    if(!connectorFk.isNull()){
      unitConnectorFk.setConnectorFk(connectorFk);
    }
  }
  // Setup article connection FK
  articleConnectionFk.id = record.value("ArticleConnection_Id_FK");
  articleConnectionFk.setArticleId(record.value("Article_Id_FK"));
  if(!articleConnectorFk.isNull()){
    articleConnectionFk.setArticleConnectorFk(articleConnectorFk);
  }
  articleConnectionFk.setConnectionTypeCode(record.value("ACNX_ConnectionType_Code_FK"));
  // Setup unit connection key
  key.id = record.value("Id_PK");
  key.setUnitId(record.value("Unit_Id_FK"));
  if(!unitConnectorFk.isNull()){
    key.setUnitConnectorFk(unitConnectorFk);
  }
  if(articleConnectionFk.isNull()){
    key.setConnectionTypeCode(record.value("ConnectionType_Code_FK"));
  }else{
    key.setArticleConnectionFk(articleConnectionFk);
  }
  // Fill data
  data.setKeyData(key);
  data.name = record.value("UnitContactName");
  data.resistance = record.value("Resistance");
  data.schemaPage = record.value("SchemaPage");
  data.signalName = record.value("SignalName");
  data.swAddress = record.value("SwAddress");
  data.functionEN = record.value("functionEN");
  data.functionFR = record.value("functionFR");
  data.functionDE = record.value("functionDE");
  data.functionIT = record.value("functionIT");
}

QString mdtClUnitConnection::baseSqlForUnitConnector() const
{
  return "SELECT UCNR.*,"\
         " ACNR.Article_Id_FK AS ACNR_Article_Id_FK,"\
         " ACNR.Connector_Id_FK AS ACNR_Connector_Id_FK\n"\
         "FROM UnitConnector_tbl UCNR\n"\
         " LEFT JOIN ArticleConnector_tbl ACNR ON ACNR.Id_PK = UCNR.ArticleConnector_Id_FK\n";
}

void mdtClUnitConnection::fillRecord(mdtSqlRecord &record, const mdtClUnitConnectorData & data)
{
  Q_ASSERT(!record.isEmpty());

  auto key = data.keyData();

  record.clearValues();
  record.setValue("Id_PK", key.pk.id);
  record.setValue("Unit_Id_FK", key.unitId());
  record.setValue("Connector_Id_FK", key.connectorFk().id);
  record.setValue("ArticleConnector_Id_FK", key.articleConnectorFk().id);
  record.setValue("Name", data.name);
}

void mdtClUnitConnection::fillData(mdtClUnitConnectorData &data, const QSqlRecord &record)
{
  Q_ASSERT(record.contains("Id_PK"));
  Q_ASSERT(record.contains("Unit_Id_FK"));
  Q_ASSERT(record.contains("Connector_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnector_Id_FK"));
  Q_ASSERT(record.contains("Name"));
  Q_ASSERT(record.contains("ACNR_Article_Id_FK"));
  Q_ASSERT(record.contains("ACNR_Connector_Id_FK"));

  mdtClConnectorKeyData connectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClUnitConnectorKeyData key;

  // Setup article connector FK
  articleConnectorFk.id = record.value("ArticleConnector_Id_FK");
  articleConnectorFk.setArticleId(record.value("ACNR_Article_Id_FK"));
  connectorFk.id = record.value("ACNR_Connector_Id_FK");
  if(!connectorFk.isNull()){
    articleConnectorFk.setConnectorFk(connectorFk);
  }
  // Setup connector FK
  connectorFk.id = record.value("Connector_Id_FK");
  // Setup unit connector FK
  key.pk.id = record.value("Id_PK");
  key.setUnitId(record.value("Unit_Id_FK"));
  if(!articleConnectorFk.isNull()){
    key.setArticleConnectorFk(articleConnectorFk);
  }else{
    if(!connectorFk.isNull()){
      key.setConnectorFk(connectorFk);
    }
  }
  // Fill data
  data.setKeyData(key);
  data.name = record.value("Name");
}
