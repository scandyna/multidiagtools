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
#include "mdtClArticle.h"
#include "mdtError.h"
#include "mdtClConnectorData.h"
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

  sql =  "SELECT Id_PK, ArticleCode, Unit, DesignationEN, DesignationFR, DesignationDE, DesignationIT "\
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

/**
bool mdtClArticle::removeComponents(const QVariant &articleId, const QModelIndexList & indexListOfSelectedRows)
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return removeComponents(articleId, idList);
}
*/

bool mdtClArticle::removeComponents(const QVariant &articleId, const mdtSqlTableSelection & s)
{
  QList<QVariant> idList;

  idList = s.dataList("Component_Id_PK");

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
  sql = "SELECT Id_PK, Connector_Id_FK, ConnectionType_Code_FK, Name "\
        "FROM ConnectorContact_tbl ";
  Q_ASSERT(connectorContactIdList.size() > 0);
  sql += "WHERE Id_PK = " + connectorContactIdList.at(0).toString();
  for(i = 1; i < connectorContactIdList.size(); ++i){
    sql += " OR Id_PK = " + connectorContactIdList.at(i).toString();
  }
  expectedFields << "Id_PK" << "Connector_Id_FK" << "ConnectionType_Code_FK" << "Name";

  return getData(sql, ok, expectedFields);
}

mdtClArticleConnectionData mdtClArticle::getConnectionData(const QVariant & articleConnectionId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;

  if(articleConnectionId.isNull()){
    pvLastError.setError("Trying to get article connection data for a NULL article connection ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    *ok = false;
    return mdtClArticleConnectionData();
  }
  sql = "SELECT * FROM ArticleConnection_tbl WHERE Id_PK = " + articleConnectionId.toString();

  return getConnectionDataPv(sql, ok);
}

QList<mdtClArticleConnectionData> mdtClArticle::getConnectionDataListFromConnectionIdList(const QList<QVariant> & connectionIdList, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QSqlQuery query(database());
  QList<mdtClArticleConnectionData> connectionDataList;
  int i;

  /// \todo Is this a error or not ?
  if(connectionIdList.isEmpty()){
    return connectionDataList;
  }
  // Setup query
  sql = "SELECT * "\
        "FROM ArticleConnection_tbl ";
  Q_ASSERT(connectionIdList.size() > 0);
  sql += "WHERE Id_PK = " + connectionIdList.at(0).toString();
  for(i = 1; i < connectionIdList.size(); ++i){
    sql += " OR Id_PK = " + connectionIdList.at(i).toString();
  }
  // Submit query
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot execute query to get article connections."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    *ok = false;
    return connectionDataList;
  }
  // Fill data
  while(query.next()){
    connectionDataList.append(query.record());
  }
  *ok = true;

  return connectionDataList;
}

mdtClArticleConnectorData mdtClArticle::getConnectorData(const QVariant & articleConnectorId, bool *ok, bool includeConnectionData, bool includeBaseConnectorData)
{
  Q_ASSERT(ok != 0);

  QString sql;

  if(articleConnectorId.isNull()){
    pvLastError.setError("Trying to get article connector data for a NULL article connector ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    *ok = false;
    return mdtClArticleConnectorData();
  }
  sql = "SELECT * FROM ArticleConnector_tbl WHERE Id_PK = " + articleConnectorId.toString();

  return getConnectorDataPv(sql, ok, includeConnectionData, includeBaseConnectorData);
}

bool mdtClArticle::addConnectionDataListFromConnectorContactIdList(mdtClArticleConnectorData & data, const QList<QVariant> & connectorContactIdList)
{
  QList<QSqlRecord> connectorContactDataList;
  mdtClArticleConnectionData articleConnectionData;
  int i;
  bool ok;

  // Get connector contact data
  connectorContactDataList = getConnectorContactDataList(connectorContactIdList, &ok);
  if(!ok){
    return false;
  }
  // Create article contact data list
  if(!articleConnectionData.setup(database())){
    pvLastError = articleConnectionData.lastError();
    return false;
  }
  for(i = 0; i < connectorContactDataList.size(); ++i){
    articleConnectionData.clearValues();
    articleConnectionData.setValue("ArticleContactName", connectorContactDataList.at(i).value("Name"));
    articleConnectionData.setValue("Article_Id_FK", data.value("Article_Id_FK"));
    articleConnectionData.setValue("ConnectionType_Code_FK", connectorContactDataList.at(i).value("ConnectionType_Code_FK"));
    articleConnectionData.setValue("ArticleConnector_Id_FK", data.value("Id_PK"));
    data.addConnectionData(articleConnectionData);
  }

  return true;
}

bool mdtClArticle::updateUnitConnections(const QStringList & fields, const QVariant & connectionId)
{
  QList<QSqlRecord> articleConnectionDataList;
  QSqlRecord articleConnectionData;
  mdtSqlRecord unitConnectionRecord;
  QString fieldName;
  QSqlDatabase db = database();
  QString sql;
  bool ok;
  int i;

  // Get data from ArticleConnection_tbl
  if(connectionId.isNull()){
    pvLastError.setError("Trying to update unit connection by giving a Null article connection ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  sql = "SELECT * FROM ArticleConnection_tbl WHERE Id_PK = " + connectionId.toString();
  articleConnectionDataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(articleConnectionDataList.size() == 1);
  articleConnectionData = articleConnectionDataList.at(0);
  // Map and copy article connection fields to unit fields
  for(i = 0; i < fields.size(); ++i){
    fieldName = fields.at(i);
    // Resistance
    if(fieldName == "Resistance"){
      if(!unitConnectionRecord.addField("Resistance", "UnitConnection_tbl", db)){
        pvLastError = unitConnectionRecord.lastError();
        return false;
      }
      unitConnectionRecord.setValue("Resistance", articleConnectionData.value("Resistance"));
    }else if(fieldName == "ArticleContactName"){
      // Contact name
      if(!unitConnectionRecord.addField("UnitContactName", "UnitConnection_tbl", db)){
        pvLastError = unitConnectionRecord.lastError();
        return false;
      }
      unitConnectionRecord.setValue("UnitContactName", articleConnectionData.value("ArticleContactName"));
    }else if(fieldName == "FunctionEN"){
      // FunctionEN
      if(!unitConnectionRecord.addField("FunctionEN", "UnitConnection_tbl", db)){
        pvLastError = unitConnectionRecord.lastError();
        return false;
      }
      unitConnectionRecord.setValue("FunctionEN", articleConnectionData.value("FunctionEN"));
    }else if(fieldName == "FunctionFR"){
      // FunctionFR
      if(!unitConnectionRecord.addField("FunctionFR", "UnitConnection_tbl", db)){
        pvLastError = unitConnectionRecord.lastError();
        return false;
      }
      unitConnectionRecord.setValue("FunctionFR", articleConnectionData.value("FunctionFR"));
    }else if(fieldName == "FunctionDE"){
      // FunctionDE
      if(!unitConnectionRecord.addField("FunctionDE", "UnitConnection_tbl", db)){
        pvLastError = unitConnectionRecord.lastError();
        return false;
      }
      unitConnectionRecord.setValue("FunctionDE", articleConnectionData.value("FunctionDE"));
    }else if(fieldName == "FunctionIT"){
      // FunctionIT
      if(!unitConnectionRecord.addField("FunctionIT", "UnitConnection_tbl", db)){
        pvLastError = unitConnectionRecord.lastError();
        return false;
      }
      unitConnectionRecord.setValue("FunctionIT", articleConnectionData.value("FunctionIT"));
    }else{
      pvLastError.setError(tr("Source field") + " '" + fieldName + "' " + tr("cannot be mapped."), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
      pvLastError.commit();
      return false;
    }
  }

  return updateRecord("UnitConnection_tbl", unitConnectionRecord, "ArticleConnection_Id_FK", connectionId);
}

bool mdtClArticle::updateUnitConnections(const QStringList & fields, const mdtSqlTableSelection & s)
{
  int row;

  if(!beginTransaction()){
    return false;
  }
  for(row = 0; row < s.rowCount(); ++row){
    if(!updateUnitConnections(fields, s.data(row, "Id_PK"))){
      rollbackTransaction();
      return false;
    }
  }

  return commitTransaction();
}

// bool mdtClArticle::addCableLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const QVariant & identification, const QVariant & R)
// {
//   mdtSqlRecord record;
// 
//   if(!record.addAllFields("ArticleLink_tbl", database())){
//     pvLastError = record.lastError();
//     return false;
//   }
//   record.setValue("ArticleConnectionStart_Id_FK", articleConnectionStartId);
//   record.setValue("ArticleConnectionEnd_Id_FK", articleConnectionEndId);
//   record.setValue("LinkType_Code_FK", "CABLELINK");
//   record.setValue("LinkDirection_Code_FK", "BID");
//   record.setValue("Identification", identification);
//   record.setValue("Resistance", R);
// 
//   return addRecord(record, "ArticleLink_tbl");
// }

// bool mdtClArticle::addInternalLink(const QVariant& articleConnectionStartId, const QVariant& articleConnectionEndId, const QVariant& identification, const QVariant& R )
// {
//   mdtSqlRecord record;
// 
//   if(!record.addAllFields("ArticleLink_tbl", database())){
//     pvLastError = record.lastError();
//     return false;
//   }
//   record.setValue("ArticleConnectionStart_Id_FK", articleConnectionStartId);
//   record.setValue("ArticleConnectionEnd_Id_FK", articleConnectionEndId);
//   record.setValue("LinkType_Code_FK", "INTERNLINK");
//   record.setValue("LinkDirection_Code_FK", "BID");
//   record.setValue("Identification", identification);
//   record.setValue("Resistance", R);
// 
//   return addRecord(record, "ArticleLink_tbl");
// }

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
  return updateRecord("ArticleLink_tbl", data, "ArticleConnectionStart_Id_FK", articleConnectionStartId , "ArticleConnectionEnd_Id_FK", articleConnectionEndId);
//   mdtSqlRecord matchData;
// 
//   // Setup row condition
//   if(!matchData.addField("ArticleConnectionStart_Id_FK", "ArticleLink_tbl", database())){
//     pvLastError = matchData.lastError();
//     return false;
//   }
//   if(!matchData.addField("ArticleConnectionEnd_Id_FK", "ArticleLink_tbl", database())){
//     pvLastError = matchData.lastError();
//     return false;
//   }
//   matchData.setValue("ArticleConnectionStart_Id_FK", articleConnectionStartId);
//   matchData.setValue("ArticleConnectionEnd_Id_FK", articleConnectionEndId);
// 
//   return updateRecord("ArticleLink_tbl", data, matchData);
}

int mdtClArticle::relatedLinksCount(const QVariant& articleConnectionStartId, const QVariant& articleConnectionEndId)
{
  QString sql;
  QList<QVariant> dataList;
  bool ok;

  sql = "SELECT COUNT(*) FROM Link_tbl WHERE ArticleConnectionStart_Id_FK = " + articleConnectionStartId.toString() + " AND ArticleConnectionEnd_Id_FK = " + articleConnectionEndId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return -1;
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0).toInt();
}

bool mdtClArticle::updateRelatedLinks(const QVariant& articleConnectionStartId, const QVariant& articleConnectionEndId, const QStringList& articleLinkFields)
{
  Q_ASSERT(!articleConnectionStartId.isNull());
  Q_ASSERT(!articleConnectionEndId.isNull());
  /*
   * We not handle changing link primary key,
   *  better fail than do wrong things
   */
  Q_ASSERT(!articleLinkFields.contains("ArticleConnectionStart_Id_FK"));
  Q_ASSERT(!articleLinkFields.contains("ArticleConnectionEnd_Id_FK"));

  QString sql;
  QList<QSqlRecord> articleLinkDataList;
  QSqlRecord articleLinkData;
  mdtSqlRecord linkData;
  QSqlDatabase db = database();
  bool ok;

  // Get article link data
  sql = "SELECT * FROM ArticleLink_tbl WHERE ArticleConnectionStart_Id_FK = " + articleConnectionStartId.toString() + " AND ArticleConnectionEnd_Id_FK = " + articleConnectionEndId.toString();
  articleLinkDataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(articleLinkDataList.size() == 1);
  articleLinkData = articleLinkDataList.at(0);
  // Map and copy article link data for each given field
  for(auto & alFieldName : articleLinkFields){
    // LinkType_Code_FK
    if(alFieldName == "LinkType_Code_FK"){
      if(!linkData.addField("LinkType_Code_FK", "Link_tbl", db)){
        pvLastError = linkData.lastError();
        return false;
      }
      linkData.setValue("LinkType_Code_FK", articleLinkData.value("LinkType_Code_FK"));
    }else if(alFieldName == "LinkDirection_Code_FK"){
      // LinkDirection_Code_FK
      if(!linkData.addField("LinkDirection_Code_FK", "Link_tbl", db)){
        pvLastError = linkData.lastError();
        return false;
      }
      linkData.setValue("LinkDirection_Code_FK", articleLinkData.value("LinkDirection_Code_FK"));
    }else if(alFieldName == "LinkDirection_Code_FK"){
      // Identification
      if(!linkData.addField("Identification", "Link_tbl", db)){
        pvLastError = linkData.lastError();
        return false;
      }
      linkData.setValue("Identification", articleLinkData.value("Identification"));
    }else if(alFieldName == "LinkDirection_Code_FK"){
      // Resistance
      if(!linkData.addField("Resistance", "Link_tbl", db)){
        pvLastError = linkData.lastError();
        return false;
      }
      linkData.setValue("Resistance", articleLinkData.value("Resistance"));
    }
  }
  // Update in Link_tbl
  if(!beginTransaction()){
    return false;
  }
  if(!updateRecord("Link_tbl", linkData, "ArticleConnectionStart_Id_FK", articleConnectionStartId , "ArticleConnectionEnd_Id_FK", articleConnectionEndId)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtClArticle::removeLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId)
{
  return removeData("ArticleLink_tbl", "ArticleConnectionStart_Id_FK", articleConnectionStartId, "ArticleConnectionEnd_Id_FK", articleConnectionEndId);
}

bool mdtClArticle::removeLinks(const mdtSqlTableSelection & s)
{
  int row;
  QModelIndexList indexes;

  if(!beginTransaction()){
    return false;
  }
  for(row = 0; row < s.rowCount(); ++row){
    if(!removeLink(s.data(row, "ArticleConnectionStart_Id_FK"), s.data(row, "ArticleConnectionEnd_Id_FK"))){
      rollbackTransaction();
      return false;
    }
  }

  return commitTransaction();
}

mdtClArticleConnectionData mdtClArticle::getConnectionDataPv(const QString & sql, bool *ok)
{
  Q_ASSERT(ok != 0);

  mdtClArticleConnectionData data;
  QList<QSqlRecord> dataList;

  // Get article connection data part
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  // Done
  *ok = true;

  return data;
}

mdtClArticleConnectorData mdtClArticle::getConnectorDataPv(const QString & sql, bool *ok, bool includeConnectionData, bool includeBaseConnectorData)
{
  Q_ASSERT(ok != 0);

  mdtClArticleConnectorData data;
  QList<QSqlRecord> dataList;
  QString _sql;
  int i;

  // Get article connector data part
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  Q_ASSERT(!data.value("Id_PK").isNull());
  // Add article connection data
  if(includeConnectionData){
    _sql = "SELECT * FROM ArticleConnection_tbl WHERE ArticleConnector_Id_FK = " + data.value("Id_PK").toString();
    dataList = getData(_sql, ok);
    if(!*ok){
      return data;
    }
    for(i = 0; i < dataList.size(); ++i){
      data.addConnectionData(dataList.at(i));
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
    ///data.setConnectorData(dataList.at(0));
  }
  // Done
  *ok = true;

  return data;
}
