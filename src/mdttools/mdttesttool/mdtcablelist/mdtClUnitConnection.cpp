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
#include "mdtClArticleLink.h"
#include "mdtClArticleLinkData.h"
#include "mdtClLink.h"
#include "mdtClLinkData.h"
#include <QSqlQuery>
#include <QList>

#include <QDebug>

mdtClUnitConnection::mdtClUnitConnection(QObject *parent, QSqlDatabase db)
 : mdtClArticleConnection(parent, db)
{
}

mdtClUnitConnection::mdtClUnitConnection(QSqlDatabase db)
 : mdtClUnitConnection(nullptr, db)
{
}

mdtClUnitConnectionPkData mdtClUnitConnection::addUnitConnection(const mdtClUnitConnectionData & data, bool handleTransaction)
{
  mdtClUnitConnectionPkData pk;
  mdtSqlRecord record;
  QSqlQuery query(database());
  mdtSqlTransaction transaction(database());

  pvAddedLinks.clear();
  // Setup record with given data
  if(!record.addAllFields("UnitConnection_tbl", database())){
    pvLastError = record.lastError();
    return pk;
  }
  fillRecord(record, data);
  // Save to database
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return pk;
    }
  }
  if(!addRecord(record, "UnitConnection_tbl", query)){
    return pk;
  }
  pk.id = query.lastInsertId();
  // If connection is based on a article connection, create possibly required links
  if(data.isBasedOnArticleConnection()){
    mdtClArticleLink alnk(database());
    bool ok;
    // Get possibly related article links
    QList<mdtClArticleLinkData> aLinkDataList = alnk.getLinkDataList(pk, data.keyData().unitId(), ok);
    if(!ok){
      pvLastError = alnk.lastError();
      pk.clear();
      return pk;
    }
    // If article links are related, create a link on its base
    mdtClLink lnk(database());
    for(const auto & aLinkData : aLinkDataList){
      if(!lnk.addLink(aLinkData, data.keyData().unitId())){
        pvLastError = lnk.lastError();
        pk.clear();
        return pk;
      }
      AddedLink_t al{aLinkData.keyData().pk, data.keyData().unitId()};
      pvAddedLinks.append(al);
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

bool mdtClUnitConnection::addUnitConnectionList(const QList<mdtClUnitConnectionData> & dataList, bool handleTransaction)
{
  mdtSqlTransaction transaction(database());

  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  QList<AddedLink_t> addedLinks = pvAddedLinks;
  for(const auto & data : dataList){
    if(addUnitConnection(data, false).isNull()){
      return false;
    }
    addedLinks.append(pvAddedLinks);
  }
  pvAddedLinks = addedLinks;
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

bool mdtClUnitConnection::linksHaveBeenAdded() const
{
  return !pvAddedLinks.isEmpty();
}

QString mdtClUnitConnection::getAddedLinksText(bool & ok)
{
  QString text;
  mdtClLink lnk(database());

  for(const auto & al : pvAddedLinks){
    // Get link data part
    mdtClLinkData linkData = lnk.getLinkData(al.articleLinkPk, al.unitId, al.unitId, ok);
    if(!ok){
      return text;
    }
    mdtClUnitConnectionPkData ucnxPk;
    // Get start connection data part
    ucnxPk.id = linkData.pk().connectionStartId;
    mdtClUnitConnectionData ucnxStartData = getUnitConnectionData(ucnxPk, ok);
    if(!ok){
      return text;
    }
    // Get start connector data part
    mdtClUnitConnectorData ucnrStartData;
    if(ucnxStartData.isPartOfUnitConnector()){
      ucnrStartData = getUnitConnectorData(ucnxStartData.keyData().unitConnectorFk().pk, false, ok);
      if(!ok){
        return text;
      }
    }
    // Get end connection data part
    ucnxPk.id = linkData.pk().connectionEndId;
    mdtClUnitConnectionData ucnxEndData = getUnitConnectionData(ucnxPk, ok);
    if(!ok){
      return text;
    }
    // Get end connector data part
    mdtClUnitConnectorData ucnrEndData;
    if(ucnxEndData.isPartOfUnitConnector()){
      ucnrEndData = getUnitConnectorData(ucnxEndData.keyData().unitConnectorFk().pk, false, ok);
      if(!ok){
        return text;
      }
    }
    // Build line text
    text += tr(" - ") + linkData.identification.toString() \
          + tr(" , start (connector: ") + ucnrStartData.name.toString() + tr(" , connection: ") + ucnxStartData.name.toString() + tr(")") \
          + tr(" , end (connector: ") + ucnrEndData.name.toString() + tr(" , connection: ") + ucnxEndData.name.toString() + tr(")") + "\n";

  }

  return text;
}

mdtClUnitConnectionData mdtClUnitConnection::getUnitConnectionData(const mdtClUnitConnectionPkData & pk, bool & ok)
{
  mdtClUnitConnectionData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnection();
  sql += " WHERE UCNX.Id_PK = " + pk.id.toString();
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

bool mdtClUnitConnection::updateUnitConnection(const mdtClUnitConnectionPkData & pk, const mdtClUnitConnectionData & data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("UnitConnection_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, data);

  return updateRecord("UnitConnection_tbl", record, "Id_PK", pk.id);
}

bool mdtClUnitConnection::removeUnitConnection(const mdtClUnitConnectionPkData & pk)
{
  return removeData("UnitConnection_tbl", "Id_PK", pk.id);
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
    mdtClUnitConnectionPkData pk;
    pk.id = id;
    if(!removeUnitConnection(pk)){
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

mdtClUnitConnectorData mdtClUnitConnection::getUnitConnectorData(mdtClUnitConnectionPkData pk, bool &ok)
{
  mdtClUnitConnectorData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnector();
  sql += " JOIN UnitConnection_tbl UCNX ON UCNX.UnitConnector_Id_FK = UCNR.Id_PK";
  sql += " WHERE UCNX.Id_PK = " + pk.id.toString();
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

mdtClUnitConnectorKeyData mdtClUnitConnection::getUnitConnectorKeyData(const mdtClArticleConnectionKeyData & key, const QVariant & unitId, bool & ok)
{
  mdtClUnitConnectorKeyData unitConnectorKey;
  mdtClUnitConnectorData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = baseSqlForUnitConnector();
  sql += " LEFT JOIN ArticleConnection_tbl ACNX ON ACNX.ArticleConnector_Id_FK = ACNR.Id_PK\n";
  sql += " WHERE ACNX.Id_PK = " + key.id.toString();
  sql += " AND UCNR.Unit_Id_FK = " + unitId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return unitConnectorKey;
  }
  if(dataList.isEmpty()){
    return unitConnectorKey;
  }
  
  qDebug() << dataList;
  
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
  record.setValue("Id_PK", key.pk.id);
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

  mdtClConnectorPkData connectorFk;
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
  key.pk.id = record.value("Id_PK");
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

  mdtClConnectorPkData connectorFk;
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
