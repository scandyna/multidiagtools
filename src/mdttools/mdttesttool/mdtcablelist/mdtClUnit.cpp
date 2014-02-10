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
#include <QSqlQuery>
#include <QSqlField>
#include <QPair>

#include <QDebug>

mdtClUnit::mdtClUnit(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
  pvToUnitConnectionRelatedRangesModel = new QSqlQueryModel;
  pvUnitModel = new QSqlQueryModel;
  pvUnitLinkModel = 0;
}

mdtClUnit::~mdtClUnit()
{
  delete pvToUnitConnectionRelatedRangesModel;
  delete pvUnitModel;
  delete pvUnitLinkModel;
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

QString mdtClUnit::sqlForArticleConnectionLinkedToArticleSelection(const QVariant & articleId, const QVariant & unitId) const
{
  QString sql;

  sql = "SELECT * FROM ArticleConnection_tbl ";
  sql += "WHERE Article_Id_FK = " + articleId.toString();
  sql += " AND Id_PK NOT IN ("\
         "  SELECT ArticleConnection_Id_FK"\
         "  FROM UnitConnection_tbl"\
         "  WHERE Unit_Id_FK = " + unitId.toString();
  sql += "  AND ArticleConnection_Id_FK IS NOT NULL)";

  return sql;
}

QString mdtClUnit::sqlForArticleConnectionLinkedToUnitConnectorSelection(const QVariant & unitConnectorId, const QVariant & unitId) const
{
  QString sql;

  sql = "SELECT"\
        " UnitConnector_tbl.Id_PK AS UnitConnector_Id_PK,"\
        " UnitConnector_tbl.Unit_Id_FK,"\
        " ArticleConnector_tbl.Id_PK AS ArticleConnector_Id_PK,"\
        " ArticleConnector_tbl.Name AS ArticleConnectorName,"\
        " ArticleConnection_tbl.Id_PK AS ArticleConnection_Id_PK,"\
        " ArticleConnection_tbl.ArticleContactName,"\
        " ArticleConnection_tbl.IoType,"\
        " ArticleConnection_tbl.FunctionEN,"\
        " ArticleConnection_tbl.FunctionFR,"\
        " ArticleConnection_tbl.FunctionDE,"\
        " ArticleConnection_tbl.FunctionIT "\
        "FROM UnitConnector_tbl "\
        " JOIN ArticleConnector_tbl"\
        "  ON ArticleConnector_tbl.Id_PK = UnitConnector_tbl.ArticleConnector_Id_FK "\
        " JOIN ArticleConnection_tbl"\
        "  ON ArticleConnection_tbl.ArticleConnector_Id_FK = ArticleConnector_tbl.Id_PK "\
        "WHERE UnitConnector_Id_PK = " + unitConnectorId.toString();
  sql += " AND ArticleConnection_Id_PK NOT IN ("\
         "  SELECT ArticleConnection_Id_FK"\
         "  FROM UnitConnection_tbl"\
         "  WHERE Unit_Id_FK = " + unitId.toString();
  sql += "  AND ArticleConnection_Id_FK IS NOT NULL)";

  return sql;
}

mdtClUnitConnectionData mdtClUnit::getConnectionData(const QVariant & unitConnectionId, bool *ok)
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

  return getConnectionDataPv(sql, ok);
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


/**
mdtClUnitConnectionData mdtClUnit::getBaseConnectorContactData(const QVariant & contactId, mdtClUnitConnectionData data)
{
  ///mdtClUnitConnectionData data;
  QString sql;
  QSqlError sqlError;
  QSqlRecord rec;
  QSqlQuery query(database());

  if(contactId.isNull()){
    pvLastError.setError("Trying to get connector contact data for a NULL contact ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  sql = "SELECT * FROM ConnectorContact_tbl WHERE Id_PK = " + contactId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get connector contact data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(!query.next()){
    pvLastError.setError("There is no data for connector contact ID " + contactId.toString(), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  rec = query.record();
  data.setConnectorId(query.value(rec.indexOf("Connector_Id_FK")));
  data.setContactName(query.value(rec.indexOf("Name")));

  return data;
}
*/

/**
mdtSqlRecord mdtClUnit::getBaseConnectorContactData(const QVariant & connectorContactId, bool *ok, mdtSqlRecord data)
{
  Q_ASSERT(ok != 0);

  QList<QSqlRecord> connectorContactData;

  // If data is empty, add fields, else check that Connector_Id_FK and Name exists
  if(data.count() < 1){
    if(!data.addAllFields("")
  }else{
    
  }
}
*/

/**
mdtClUnitConnectionData mdtClUnit::getConnectionDataByArticleConnectionId(const QVariant & articleConnectionId, const QVariant & unitId)
{
  mdtClUnitConnectionData data;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  if(articleConnectionId.isNull()){
    pvLastError.setError("Trying to get unit connection data for a NULL article connection ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(unitId.isNull()){
    pvLastError.setError("Trying to get unit connection data for a NULL unit ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  // Find the unit connection ID
  sql = "SELECT Id_PK FROM UnitConnection_tbl";
  sql += " WHERE ArticleConnection_Id_FK = " + articleConnectionId.toString();
  sql += " AND Unit_Id_FK = " + unitId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connection ID from article connection ID", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(!query.next()){
    pvLastError.setError("No linked article connection found", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }

  return getConnectionDataByUnitConnectionId(query.value(0));
}
*/

/**
mdtClUnitConnectionData mdtClUnit::getArticleConnectionData(const QVariant & articleConnectionId, mdtClUnitConnectionData data)
{
  if(articleConnectionId.isNull()){
    pvLastError.setError("Trying to get article connection data for a NULL article connection ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }

  // Fill article connection part
  if(!fillArticleConnectionDataPart(data, articleConnectionId)){
    data.clear();
    return data;
  }
  // Fill article connector part
  if(!data.articleConnectionData().articleConnectorId().isNull()){
    if(!fillArticleConnectorDataPart(data, data.articleConnectionData().articleConnectorId())){
      data.clear();
      return data;
    }
  }

  return data;
}
*/

/**
mdtClUnitConnectionData mdtClUnit::getUnitConnectorData(const QVariant & unitConnectorId, mdtClUnitConnectionData data)
{
  if(unitConnectorId.isNull()){
    pvLastError.setError("Trying to get unit connector data for a NULL unit connector ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(!fillUnitConnectorDataPart(data, unitConnectorId)){
    data.clear();
    return data;
  }

  return data;
}
*/

/**
mdtClUnitConnectionData mdtClUnit::getArticleConnectorData(const QVariant & articleConnectorId, mdtClUnitConnectionData data)
{
  if(articleConnectorId.isNull()){
    pvLastError.setError("Trying to get article connector data for a NULL article connector ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(!fillArticleConnectorDataPart(data, articleConnectorId)){
    data.clear();
    return data;
  }

  return data;
}
*/

/**
mdtClUnitConnectionData mdtClUnit::getUnitConnectorDataByArticleConnectorId(const QVariant & articleConnectorId, const QVariant & unitId, mdtClUnitConnectionData data)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant unitConnectorId;

  if(articleConnectorId.isNull()){
    pvLastError.setError("Trying to get unit connector data for a NULL article connector ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(unitId.isNull()){
    pvLastError.setError("Trying to get unit connector data for a NULL unit ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  // Find the unit connactor ID
  sql = "SELECT Id_PK FROM UnitConnector_tbl ";
  sql += " WHERE ArticleConnector_Id_FK = " + articleConnectorId.toString();
  sql += " AND Unit_Id_FK = " + unitId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connector ID from article connector ID", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return data;
  }
  if(query.next()){
    unitConnectorId = query.value(0);
    if(!unitConnectorId.isNull()){
      data = getUnitConnectorData(unitConnectorId, data);
    }
  }

  return data;
}
*/

QSqlQueryModel *mdtClUnit::unitModelForComponentSelection(const QVariant &unitId)
{
  QString sql;

  sql =  "SELECT Id_PK, SchemaPosition, Cabinet, Coordinate "\
         "FROM Unit_tbl "\
         "WHERE ( Id_PK <> " + unitId.toString() + " "\
         "AND Composite_Id_FK IS NULL )";
  pvUnitModel->setQuery(sql, database());

  return pvUnitModel;
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

QSqlQueryModel *mdtClUnit::toUnitConnectionRelatedRangesModel(const QVariant & unitConnectionId)
{
  return pvToUnitConnectionRelatedRangesModel;
}

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

QString mdtClUnit::toUnitRelatedLinksListStr(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList)
{
  QStringList linksList;
  QString str;
  int i;

  linksList = toUnitRelatedLinksList(unitId, unitConnectionIdList);
  for(i = 0; i < linksList.size(); ++i){
    str += linksList.at(i) + "\n";
  }

  return str;
}

QString mdtClUnit::toUnitRelatedLinksListStr(const QVariant &unitId, const QModelIndexList & indexListOfSelectedRows)
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return toUnitRelatedLinksListStr(unitId, idList);
}


bool mdtClUnit::addConnection(const mdtClUnitConnectionData & data)
{
  return addRecord(data, "UnitConnection_tbl");
}

bool mdtClUnit::editConnection(const QVariant & connectionId, const mdtClUnitConnectionData & data)
{
  return updateRecord("UnitConnection_tbl", data, "Id_PK", connectionId);
}

bool mdtClUnit::removeConnection(const QVariant & unitConnectionId)
{
  return removeData("UnitConnection_tbl", "Id_PK", unitConnectionId);
}

bool mdtClUnit::removeConnections(const QModelIndexList & indexListOfSelectedRows)
{
  return removeData("UnitConnection_tbl", "Id_PK", indexListOfSelectedRows);
}

bool mdtClUnit::addConnector(const mdtClUnitConnectorData & data)
{
  QSqlQuery query(database());
  QList<mdtSqlRecord> unitConnectionRecordList;
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
  // Update connection data
  for(i = 0; i < data.connectionDataList().size(); ++i){
    mdtSqlRecord record = data.connectionDataList().at(i);
    record.setValue("Unit_Id_FK", unitId);
    record.setValue("UnitConnector_Id_FK", unitConnectorId);
    unitConnectionRecordList.append(record);
  }
  // Add connections
  if(!addRecordList(unitConnectionRecordList, "UnitConnection_tbl", false)){
    rollbackTransaction();
    return false;
  }
  // Commit
  if(!commitTransaction()){
    return false;
  }

  return true;
}

/**
bool mdtClUnit::addConnector(const QVariant & unitId, const QVariant & baseConnectorId, const QVariant & articleConnectorId, const QVariant & name)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for insertion
  sql = "INSERT INTO UnitConnector_tbl (Unit_Id_FK, Connector_Id_FK, ArticleConnector_Id_FK, Name) "\
        "VALUES (:Unit_Id_FK, :Connector_Id_FK, :ArticleConnector_Id_FK, :Name)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for connector inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Unit_Id_FK", unitId);
  query.bindValue(":Connector_Id_FK", baseConnectorId);
  query.bindValue(":ArticleConnector_Id_FK", articleConnectorId);
  query.bindValue(":Name", name);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for connector inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }

  return true;
}
*/

/**
bool mdtClUnit::addConnector(const QVariant & unitId, const QVariant & baseConnectorId, const QVariant & articleConnectorId, const QVariant & name, const QList<mdtClUnitConnectionData> connectionList)
{
  int i;
  QVariant unitConnectorId;
  QList<mdtClUnitConnectionData> _connectionList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  if(!beginTransaction()){
    return false;
  }
  if(!addConnector(unitId, baseConnectorId, articleConnectorId, name)){
    rollbackTransaction();
    return false;
  }
  // Get freshly inserted unit connector ID
  sql = "SELECT Id_PK FROM UnitConnector_tbl";
  if(!query.exec(sql)){
    rollbackTransaction();
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connector ID for connections insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  unitConnectorId = query.lastInsertId();
  if(unitConnectorId.isNull()){
    rollbackTransaction();
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get unit connector ID for connections inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // We have to edit connection data, so make a copy ..
  _connectionList = connectionList;
  for(i = 0; i < _connectionList.size(); ++i){
    _connectionList[i].setUnitId(unitId);
    _connectionList[i].setUnitConnectorId(unitConnectorId);
    if(!addUnitConnection(_connectionList.at(i))){
      rollbackTransaction();
      return false;
    }
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}
*/

/**
bool mdtClUnit::removeConnector(const QVariant & unitConnectorId)
{
  QSqlError sqlError;
  QString sql;
  QSqlQuery query(database());

  sql = "DELETE FROM UnitConnector_tbl WHERE Id_PK = " + unitConnectorId.toString();
  // Submit query
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for unit connector deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }

  return true;
}
*/

bool mdtClUnit::removeConnector(const QVariant& unitConnectorId)
{
  if(!beginTransaction()){
    return false;
  }
  // Remove connections
  if(!removeData("UnitConnection_tbl", "UnitConnector_Id_FK", unitConnectorId)){
    rollbackTransaction();
    return false;
  }
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

/**
bool mdtClUnit::addLink(const mdtClLinkData & data)
{
  QString sql;
  QSqlError sqlError;
  QList<QPair<QVariant, QVariant> > lst;
  int i;

  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  QSqlQuery query(database());
  // Prepare query for insertion in Link table
  sql = "INSERT INTO Link_tbl (UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, "\
                              "ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, "\
                              "SinceVersion, Modification, Identification, LinkDirection_Code_FK, "\
                              "LinkType_Code_FK, Value) "\
        "VALUES (:UnitConnectionStart_Id_FK, :UnitConnectionEnd_Id_FK, :ArticleConnectionStart_Id_FK, :ArticleConnectionEnd_Id_FK, "\
                               ":SinceVersion, :Modification, :Identification, :LinkDirection_Code_FK, "\
                               ":LinkType_Code_FK, :Value)";
  if(!query.prepare(sql)){
    rollbackTransaction();
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for link inertion into Link_tbl", mdtError::Error);
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
    pvLastError.setError("Cannot execute query for link inertion into Link_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Add a link in VehicleType_Link table for each start-end vehicles id assignation
  lst = data.vehicleTypeStartEndIdList();
  for(i = 0; i < lst.size(); ++i){
    if(!addLinkToVehicleType(lst.at(i).first, lst.at(i).second, data.unitConnectionStartId(), data.unitConnectionEndId())){
      rollbackTransaction();
      return false;
    }
  }
  // Commit the insertion
  if(!commitTransaction()){
    return false;
  }

  return true;
}
*/

bool mdtClUnit::addLink(const mdtClLinkData & data)
{
  QSqlQuery query(database());

  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  // Add Link_tbl part
  if(!addRecord(data, "Link_tbl", query)){
    rollbackTransaction();
    return false;
  }
  /// \todo Implement VehicleType_Link_tbl part !!

}

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

bool mdtClUnit::removeLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId)
{
  /**
  mdtClLinkData data;
  QList<QPair<QVariant, QVariant> > vehicleTypeList;
  QString sql;
  QSqlError sqlError;
  int i;

  // Get list of attached vehicle type
  data = getUnitLinkData(unitConnectionStartId, unitConnectionEndId);
  data.buildVehicleTypeStartEndIdList();
  vehicleTypeList = data.vehicleTypeStartEndIdList();
  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  QSqlQuery query(database());
  // Remove related vehicle type links
  for(i = 0; i < vehicleTypeList.size(); ++i){
    if(!removeLinkFromVehicleType(vehicleTypeList.at(i).first, vehicleTypeList.at(i).second, data.unitConnectionStartId(), data.unitConnectionEndId(), query)){
      rollbackTransaction();
      return false;
    }
  }
  // Remove links
  sql = "DELETE FROM Link_tbl ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + data.unitConnectionStartId().toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + data.unitConnectionEndId().toString();
  if(!query.exec(sql)){
    rollbackTransaction();
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to remove link from Link_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  // Commit the insertion
  if(!commitTransaction()){
    return false;
  }

  return true;
  */
}

bool mdtClUnit::removeLinks(const QList<QModelIndexList> &indexListOfSelectedRowsByRows)
{
  int row;
  QModelIndexList indexes;

  for(row = 0; row < indexListOfSelectedRowsByRows.size(); ++row){
    indexes = indexListOfSelectedRowsByRows.at(row);
    Q_ASSERT(indexes.size() == 2);
    if(!removeLink(indexes.at(0).data(), indexes.at(1).data())){
      return false;
    }
  }

  return true;
}

mdtClUnitConnectionData mdtClUnit::getConnectionDataPv(const QString & sql, bool *ok)
{
  Q_ASSERT(ok != 0);

  mdtClUnitConnectionData data;
  QList<QSqlRecord> dataList;
  QVariant articleConnectionId;
  ///QString acSql;
  mdtClArticle art(0, database());
  mdtClArticleConnectionData articleConnectionData;

  // Get unit connection data part
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  // If data is based on article connection, get article connection data
  articleConnectionId = data.value("ArticleConnection_Id_FK");
  if(!articleConnectionId.isNull()){
    /**
    acSql = "SELECT * FROM ArticleConnection_tbl WHERE Id_PK = " + articleConnectionId.toString();
    dataList = getData(acSql, ok);
    if(!*ok){
      return data;
    }
    Q_ASSERT(dataList.size() == 1);
    */
    ///data.articleConnectionData() = dataList.at(0);
    ///articleConnectionData = dataList.at(0);
    articleConnectionData = art.getConnectionData(articleConnectionId, ok);
    if(!*ok){
      return data;
    }
    data.setArticleConnectionData(articleConnectionData);
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
  if(!articleConnectorId.isNull()){
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

/**
bool mdtClUnit::fillUnitConnectionDataPart(mdtClUnitConnectionData & data, const QVariant & unitConnectionId)
{
  Q_ASSERT(!unitConnectionId.isNull());

  QString sql;
  QSqlError sqlError;
  QSqlRecord rec;
  QSqlQuery query(database());

  sql = "SELECT * FROM UnitConnection_tbl WHERE Id_PK = " + unitConnectionId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connection data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  if(!query.next()){
    pvLastError.setError("There is no data for unit connection ID " + unitConnectionId.toString(), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return true;
  }
  rec = query.record();
  data.setConnectionId(query.value(rec.indexOf("Id_PK")));
  data.setUnitId(query.value(rec.indexOf("Unit_Id_FK")));
  data.setUnitConnectorId(query.value(rec.indexOf("UnitConnector_Id_FK")));
  data.articleConnectionData().setConnectionId(query.value(rec.indexOf("ArticleConnection_Id_FK")));
  data.setContactName(query.value(rec.indexOf("UnitContactName")));
  data.setSchemaPage(query.value(rec.indexOf("SchemaPage")));
  data.setSignalName(query.value(rec.indexOf("SignalName")));
  data.setSwAddress(query.value(rec.indexOf("SwAddress")));
  data.setConnectorName(query.value(rec.indexOf("UnitConnectorName")));
  data.setFunctionEN(query.value(rec.indexOf("FunctionEN")));
  data.setFunctionFR(query.value(rec.indexOf("FunctionFR")));
  data.setFunctionDE(query.value(rec.indexOf("FunctionDE")));
  data.setFunctionIT(query.value(rec.indexOf("FunctionIT")));

  return true;
}

bool mdtClUnit::fillUnitConnectorDataPart(mdtClUnitConnectionData & data, const QVariant & unitConnectorId)
{
  Q_ASSERT(!unitConnectorId.isNull());

  QString sql;
  QSqlError sqlError;
  QSqlRecord rec;
  QSqlQuery query(database());

  sql = "SELECT * FROM UnitConnector_tbl WHERE Id_PK = " + unitConnectorId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connector data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  if(!query.next()){
    pvLastError.setError("There is no data for connector ID " + unitConnectorId.toString(), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return true;
  }
  rec = query.record();
  data.setUnitConnectorId(query.value(rec.indexOf("Id_PK")));
  data.setUnitId(query.value(rec.indexOf("Unit_Id_FK")));
  data.setConnectorId(query.value(rec.indexOf("Connector_Id_FK")));
  data.articleConnectionData().setConnectorId(query.value(rec.indexOf("ArticleConnector_Id_FK")));
  data.setConnectorName(query.value(rec.indexOf("Name")));

  return true;
}

bool mdtClUnit::fillArticleConnectionDataPart(mdtClUnitConnectionData & data, const QVariant & articleConnectionId)
{
  Q_ASSERT(!articleConnectionId.isNull());

  QString sql;
  QSqlError sqlError;
  QSqlRecord rec;
  QSqlQuery query(database());

  sql = "SELECT * FROM ArticleConnection_tbl WHERE Id_PK = " + articleConnectionId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get article connection data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  if(!query.next()){
    pvLastError.setError("There is no data for article connection ID " + articleConnectionId.toString(), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return true;
  }
  rec = query.record();
  data.articleConnectionData().setConnectionId(query.value(rec.indexOf("Id_PK")));
  data.articleConnectionData().setArticleId(query.value(rec.indexOf("Article_Id_FK")));
  data.articleConnectionData().setArticleConnectorId(query.value(rec.indexOf("ArticleConnector_Id_FK")));
  data.articleConnectionData().setContactName(query.value(rec.indexOf("ArticleContactName")));
  data.articleConnectionData().setIoType(query.value(rec.indexOf("IoType")));
  data.articleConnectionData().setFunctionEN(query.value(rec.indexOf("FunctionEN")));
  data.articleConnectionData().setFunctionFR(query.value(rec.indexOf("FunctionFR")));
  data.articleConnectionData().setFunctionDE(query.value(rec.indexOf("FunctionDE")));
  data.articleConnectionData().setFunctionIT(query.value(rec.indexOf("FunctionIT")));

  return true;
}

bool mdtClUnit::fillArticleConnectorDataPart(mdtClUnitConnectionData & data, const QVariant & articleConnectorId)
{
  Q_ASSERT(!articleConnectorId.isNull());

  QString sql;
  QSqlError sqlError;
  QSqlRecord rec;
  QSqlQuery query(database());

  sql = "SELECT * FROM ArticleConnector_tbl WHERE Id_PK = " + articleConnectorId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get article connector data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return false;
  }
  if(!query.next()){
    pvLastError.setError("There is no data for article connector ID " + articleConnectorId.toString(), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClUnit");
    pvLastError.commit();
    return true;
  }
  rec = query.record();
  data.articleConnectionData().setArticleConnectorId(query.value(rec.indexOf("Id_PK")));
  data.articleConnectionData().setArticleId(query.value(rec.indexOf("Article_Id_FK")));
  data.articleConnectionData().setConnectorId(query.value(rec.indexOf("Connector_Id_FK")));
  data.articleConnectionData().setConnectorName(query.value(rec.indexOf("Name")));

  return true;
}
*/
