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
#include "mdtClUnit.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QPair>

#include <QDebug>

mdtClUnit::mdtClUnit(QSqlDatabase db)
{
  pvDatabase = db;
  pvToUnitConnectionRelatedRangesModel = new QSqlQueryModel;
  pvUnitModel = new QSqlQueryModel;
  pvArticleConnectionModel = new QSqlQueryModel;
  pvUnitLinkModel = 0;
}

mdtClUnit::~mdtClUnit()
{
  delete pvToUnitConnectionRelatedRangesModel;
  delete pvUnitModel;
  delete pvArticleConnectionModel;
  delete pvUnitLinkModel;
}

const QSqlError &mdtClUnit::lastError()
{
  return pvLastError;
}

QSqlQueryModel *mdtClUnit::unitModelForComponentSelection(const QVariant &unitId)
{
  QString sql;

  sql =  "SELECT Id_PK, SchemaPosition, Cabinet, Coordinate "\
         "FROM Unit_tbl "\
         "WHERE ( Id_PK <> " + unitId.toString() + " "\
         "AND Composite_Id_FK IS NULL )";
  pvUnitModel->setQuery(sql, pvDatabase);

  return pvUnitModel;
}

bool mdtClUnit::addComponent(const QVariant &unitId, const QVariant &componentId)
{
  QString sql;
  QSqlQuery query(pvDatabase);

  // Prepare query for edition
  sql = "UPDATE Unit_tbl "\
        "SET Composite_Id_FK = :Composite_Id_FK "\
        "WHERE Id_PK = " + componentId.toString();
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for component assignation", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Composite_Id_FK", unitId);
  if(!query.exec()){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for component assignation", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
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
  QSqlQuery query(pvDatabase);
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for component assignation remove", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
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

QSqlQueryModel *mdtClUnit::modelForArticleConnectionSelection(const QVariant & unitId, const QVariant &articleId)
{
  QString sql;

  sql = "SELECT DISTINCT "\
        " ArticleConnection_tbl.Id_PK , "\
        " ArticleConnection_tbl.Article_Id_FK , "\
        " ArticleConnection_tbl.ArticleConnectorName , "\
        " ArticleConnection_tbl.ArticleContactName , "\
        " ArticleConnection_tbl.IoType , "\
        " ArticleConnection_tbl.FunctionEN "\
        "FROM ArticleConnection_tbl "\
        "LEFT JOIN UnitConnection_tbl "\
        " ON ArticleConnection_tbl.Id_PK = UnitConnection_tbl.ArticleConnection_Id_FK "\
        "WHERE Article_Id_FK = " + articleId.toString();
  sql += " AND ArticleConnection_tbl.Id_PK NOT IN ( "\
         " SELECT ArticleConnection_Id_FK "\
         " FROM UnitConnection_tbl "\
         " WHERE ( Unit_Id_FK = " + unitId.toString() + " ) "\
         " AND ( ArticleConnection_Id_FK IS NOT NULL ) )";
  ///qDebug() << "SEL ART CNN SQL: " << sql;
  pvArticleConnectionModel->setQuery(sql, pvDatabase);
  pvLastError = pvArticleConnectionModel->lastError();
  if(pvLastError.isValid()){
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for article connection selection", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
  }

  return pvArticleConnectionModel;
}

int mdtClUnit::toUnitRelatedArticleConnectionCount(const QVariant & unitId)
{
  QString sql;
  QSqlQuery query(pvDatabase);

  // Build and run query
  sql = "SELECT COUNT(*) FROM UnitConnection_tbl \
         JOIN ArticleConnection_tbl \
         ON UnitConnection_tbl.ArticleConnection_Id_FK = ArticleConnection_tbl.Id_PK \
         WHERE UnitConnection_tbl.Unit_Id_FK = " + unitId.toString();
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Query execution failed" , mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
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
QList<QVariant> mdtClUnit::toUnitRelatedArticleConnectionIds(const QVariant & unitId) {
}

bool mdtClUnit::unitConnectionIsRelatedToArticleConnection(const QVariant & unitConnectionId) {
}

QVariant mdtClUnit::toUnitConnectionRelatedArticleConnectionData(const QVariant & unitConnectionId, const QString & field) {
}
*/

QSqlQueryModel *mdtClUnit::toUnitConnectionRelatedRangesModel(const QVariant & unitConnectionId)
{
  return pvToUnitConnectionRelatedRangesModel;
}

QSqlQueryModel *mdtClUnit::toUnitRelatedLinksModel(const QVariant &unitId, const QList<QVariant> &unitConnectionIdList)
{
  QString sql;
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
  pvUnitLinkModel->setQuery(sql, pvDatabase);
  pvLastError = pvUnitLinkModel->lastError();
  if(pvLastError.isValid()){
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query to get links related to unit", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
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

mdtClUnitConnectionData mdtClUnit::getUnitConnectionData(const QVariant & unitConnectionId)
{
  QSqlQuery query(pvDatabase);
  mdtClUnitConnectionData data;
  QSqlRecord rec;
  QString sql;

  // Run query
  sql = "SELECT * FROM UnitConnection_view WHERE UnitConnection_Id_PK = " + unitConnectionId.toString();
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query to get connection data", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return data;
  }
  if(!query.next()){
    return data;
  }
  rec = query.record();
  // Set data
  data.setId(query.value(rec.indexOf("UnitConnection_Id_PK")));
  data.setArticleConnectionId(query.value(rec.indexOf("ArticleConnection_Id_FK")));
  data.setArticleConnectorName(query.value(rec.indexOf("ArticleConnectorName")));
  data.setArticleContactName(query.value(rec.indexOf("ArticleContactName")));
  data.setArticleIoType(query.value(rec.indexOf("IoType")));
  data.setArticleFunctionEN(query.value(rec.indexOf("ArticleFunctionEN")));
  data.setUnitId(query.value(rec.indexOf("Unit_Id_FK")));
  data.setSchemaPage(query.value(rec.indexOf("SchemaPage")));
  data.setFunctionEN(query.value(rec.indexOf("UnitFunctionEN")));
  data.setSignalName(query.value(rec.indexOf("SignalName")));
  data.setSwAddress(query.value(rec.indexOf("SwAddress")));
  data.setUnitConnectorName(query.value(rec.indexOf("UnitConnectorName")));
  data.setUnitContactName(query.value(rec.indexOf("UnitContactName")));

  return data;
}

bool mdtClUnit::addUnitConnection(const mdtClUnitConnectionData & data)
{
  QString sql;
  QSqlQuery query(pvDatabase);

  // Prepare query for insertion
  sql = "INSERT INTO UnitConnection_tbl (Unit_Id_FK, ArticleConnection_Id_FK, IsATestPoint, SchemaPage, FunctionEN, SignalName, SwAddress, UnitConnectorName, UnitContactName) "\
        "VALUES (:Unit_Id_FK, :ArticleConnection_Id_FK, :IsATestPoint, :SchemaPage, :FunctionEN, :SignalName, :SwAddress, :UnitConnectorName, :UnitContactName)";
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for component inertion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Unit_Id_FK", data.unitId());
  query.bindValue(":ArticleConnection_Id_FK", data.articleConnectionId());
  query.bindValue(":IsATestPoint", QVariant());   /// \todo Implement
  query.bindValue(":SchemaPage", data.schemaPage());
  query.bindValue(":FunctionEN", data.functionEN());
  query.bindValue(":SignalName", data.signalName());
  query.bindValue(":SwAddress", data.swAddress());
  query.bindValue(":UnitConnectorName", data.unitConnectorName());
  query.bindValue(":UnitContactName", data.unitContactName());
  if(!query.exec()){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for connection inertion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::editUnitConnection(const mdtClUnitConnectionData & data)
{
  QString sql;
  QSqlQuery query(pvDatabase);

  // Prepare query for insertion
  sql = "UPDATE UnitConnection_tbl "\
        " SET Unit_Id_FK = :Unit_Id_FK , "\
        "  ArticleConnection_Id_FK = :ArticleConnection_Id_FK , "\
        "  IsATestPoint = :IsATestPoint , "\
        "  SchemaPage = :SchemaPage , "\
        "  FunctionEN = :FunctionEN , "\
        "  SignalName = :SignalName , "\
        "  SwAddress = :SwAddress , "\
        "  UnitConnectorName = :UnitConnectorName , "\
        "  UnitContactName = :UnitContactName "\
        "WHERE Id_PK = " + data.id().toString();
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for connection update", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Unit_Id_FK", data.unitId());
  query.bindValue(":ArticleConnection_Id_FK", data.articleConnectionId());
  query.bindValue(":IsATestPoint", QVariant());   /// \todo Implement
  query.bindValue(":SchemaPage", data.schemaPage());
  query.bindValue(":FunctionEN", data.functionEN());
  query.bindValue(":SignalName", data.signalName());
  query.bindValue(":SwAddress", data.swAddress());
  query.bindValue(":UnitConnectorName", data.unitConnectorName());
  query.bindValue(":UnitContactName", data.unitContactName());
  if(!query.exec()){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for connection edition", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::removeUnitConnection(const QVariant & unitConnectionId)
{
  QList<QVariant> idList;

  idList.append(unitConnectionId);

  return removeUnitConnections(idList);
}

bool mdtClUnit::removeUnitConnections(const QList<QVariant> &unitConnectionIdList)
{
  int i;
  QString sql;

  if(unitConnectionIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "DELETE FROM UnitConnection_tbl ";
  for(i = 0; i < unitConnectionIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += " Id_PK = " + unitConnectionIdList.at(i).toString();
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(pvDatabase);
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for unit connection deletion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::removeUnitConnections(const QModelIndexList & indexListOfSelectedRows)
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return removeUnitConnections(idList);
}


bool mdtClUnit::addRange(const QVariant & baseUnitConnectionId, const mdtClUnitConnectionData & rangeData) {
}

mdtClLinkData mdtClUnit::getUnitLinkData(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId)
{
  QString sql;
  QSqlQuery query(pvDatabase);
  mdtClLinkData data;

  // Setup and run query to get data in unit link view
  sql = "SELECT "\
        "UnitConnectionStart_Id_FK , UnitConnectionEnd_Id_FK, ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, "\
        "SinceVersion, Modification, Identification, Value, LinkType_Code_FK, LinkDirection_Code_FK ";
  sql += " FROM UnitLink_view ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot get link data", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
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
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot get vehicle type link data", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return data;
  }
  while(query.next()){
    data.addVehicleTypeStartId(query.value(0));
    data.addVehicleTypeEndId(query.value(1));
  }

  return data;
}

bool mdtClUnit::addLink(const mdtClLinkData &data)
{
  QString sql;
  QList<QPair<QVariant, QVariant> > lst;
  int i;

  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  QSqlQuery query(pvDatabase);
  // Prepare query for insertion in Link table
  sql = "INSERT INTO Link_tbl (UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, "\
                              "ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, "\
                              "SinceVersion, Modification, Identification, LinkDirection_Code_FK, "\
                              "LinkType_Code_FK, Value) "\
        "VALUES (:UnitConnectionStart_Id_FK, :UnitConnectionEnd_Id_FK, :ArticleConnectionStart_Id_FK, :ArticleConnectionEnd_Id_FK, "\
                               ":SinceVersion, :Modification, :Identification, :LinkDirection_Code_FK, "\
                               ":LinkType_Code_FK, :Value)";
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for link inertion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    rollbackTransaction();
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
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for link inertion in Link_tbl", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    rollbackTransaction();
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

bool mdtClUnit::editLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, const mdtClLinkData &data)
{
  QString sql;
  QList<QPair<QVariant, QVariant> > vehicleTypeList;
  int i;
  mdtClLinkData currentLinkData;

  // Get list of currently attached vehicle type
  currentLinkData = getUnitLinkData(unitConnectionStartId, unitConnectionEndId);
  if(!currentLinkData.isValid()){
    pvLastError.setDatabaseText("Cannot get current link data");
    mdtError e(MDT_DATABASE_ERROR, "Cannot get current link data", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }
  currentLinkData.buildVehicleTypeStartEndIdList();
  vehicleTypeList = currentLinkData.vehicleTypeStartEndIdList();
  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  QSqlQuery query(pvDatabase);
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
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for link edition", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    rollbackTransaction();
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
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for link edition", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    rollbackTransaction();
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
}

bool mdtClUnit::removeLink(const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId)
{
  mdtClLinkData data;
  QList<QPair<QVariant, QVariant> > vehicleTypeList;
  QString sql;
  int i;

  // Get list of attached vehicle type
  data = getUnitLinkData(unitConnectionStartId, unitConnectionEndId);
  data.buildVehicleTypeStartEndIdList();
  vehicleTypeList = data.vehicleTypeStartEndIdList();
  // We want to update 2 tables, so manually ask to beginn a transaction
  if(!beginTransaction()){
    return false;
  }
  QSqlQuery query(pvDatabase);
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
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query to remove link Link_tbl", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    rollbackTransaction();
    return false;
  }
  // Commit the insertion
  if(!commitTransaction()){
    return false;
  }

  return true;
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

bool mdtClUnit::addLinkToVehicleType(const QVariant &vehicleTypeStartId, const QVariant &vehicleTypeEndId, const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId)
{
  QString sql;
  QSqlQuery query(pvDatabase);

  qDebug() << "Adding link in vehicles links table, Unit start CNN ID: " << unitConnectionStartId << " , end CNN ID: " << unitConnectionEndId;
  
  // Prepare query for insertion in VehicleType_Link table
  sql = "INSERT INTO VehicleType_Link_tbl (VehicleTypeStart_Id_FK, VehicleTypeEnd_Id_FK, UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK) "\
        "VALUES (:VehicleTypeStart_Id_FK, :VehicleTypeEnd_Id_FK, :UnitConnectionStart_Id_FK, :UnitConnectionEnd_Id_FK)";
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for link inertion in VehicleType_Link_tbl", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":VehicleTypeStart_Id_FK", vehicleTypeStartId);
  query.bindValue(":VehicleTypeEnd_Id_FK", vehicleTypeEndId);
  query.bindValue(":UnitConnectionStart_Id_FK", unitConnectionStartId);
  query.bindValue(":UnitConnectionEnd_Id_FK", unitConnectionEndId);
  if(!query.exec()){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for connection inertion in VehicleType_Link_tbl", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::removeLinkFromVehicleType(const QVariant &vehicleTypeStartId, const QVariant &vehicleTypeEndId, const QVariant &unitConnectionStartId, const QVariant &unitConnectionEndId, QSqlQuery &query)
{
  QString sql;
  ///QSqlQuery query(pvDatabase);

  sql = "DELETE FROM VehicleType_Link_tbl "\
        "WHERE VehicleTypeStart_Id_FK = " + vehicleTypeStartId.toString();
  sql += " AND VehicleTypeEnd_Id_FK = " + vehicleTypeEndId.toString();
  sql += " AND UnitConnectionStart_Id_FK = " + unitConnectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + unitConnectionEndId.toString();
  qDebug() << "REM VHC Link, SQL: " << sql;
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query to remove link in VehicleType_Link_tbl", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }

  return true;
}

bool mdtClUnit::beginTransaction()
{
  if(!pvDatabase.transaction()){
    pvLastError = pvDatabase.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot beginn transaction", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }
  return true;
}

bool mdtClUnit::rollbackTransaction()
{
  if(!pvDatabase.rollback()){
    pvLastError = pvDatabase.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot rollback transaction", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
  }
  return true;
}

bool mdtClUnit::commitTransaction()
{
  if(!pvDatabase.commit()){
    pvLastError = pvDatabase.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot commit transaction", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnit");
    e.commit();
    return false;
  }
  return true;
}
