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

mdtClUnit::mdtClUnit(QSqlDatabase db)
{
  pvDatabase = db;
  pvToUnitConnectionRelatedRangesModel = new QSqlQueryModel;
  pvUnitModel = new QSqlQueryModel;
}

mdtClUnit::~mdtClUnit()
{
  delete pvToUnitConnectionRelatedRangesModel;
  delete pvUnitModel;
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
         "WHERE ( Id_PK <> " + unitId.toString() + " ) "\
         "AND ( Composite_Id_FK NOT IN ( "\
         " SELECT Composite_Id_FK "\
         " FROM Unit_tbl "\
         " WHERE Composite_Id_FK = " + unitId.toString() + " ) "\
         " ) ";
  pvUnitModel->setQuery(sql, pvDatabase);

  return pvUnitModel;
}

bool mdtClUnit::addComponent(const QVariant &unitId, const QVariant &componentId)
{
}

bool mdtClUnit::editComponent(const QVariant &unitId, const QVariant &currentComponentId, const QVariant &newComponentId)
{
}

bool mdtClUnit::removeComponent(const QVariant &unitId, const QVariant &componentId)
{
}

bool mdtClUnit::removeComponents(const QVariant &unitId, const QList<QVariant> &componentIdList)
{
}

bool mdtClUnit::removeComponents(const QVariant &unitId, const QModelIndexList & indexListOfSelectedRows)
{
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

QList<QVariant> mdtClUnit::toUnitRelatedArticleConnectionIds(const QVariant & unitId) {
}

bool mdtClUnit::unitConnectionIsRelatedToArticleConnection(const QVariant & unitConnectionId) {
}

QVariant mdtClUnit::toUnitConnectionRelatedArticleConnectionData(const QVariant & unitConnectionId, const QString & field) {
}

QSqlQueryModel *mdtClUnit::toUnitConnectionRelatedRangesModel(const QVariant & unitConnectionId)
{
  
  
  return pvToUnitConnectionRelatedRangesModel;
}

bool mdtClUnit::addUnitConnection(const mdtClUnitConnectionData & data) {
}

bool mdtClUnit::removeUnitConnection(const QVariant & unitConnectionId) {
}

bool mdtClUnit::editUnitConnection(const mdtClUnitConnectionData & data) {
}

mdtClUnitConnectionData mdtClUnit::getUnitConnectionData(const QVariant & unitConnectionId) {
}



bool mdtClUnit::addRange(const QVariant & baseUnitConnectionId, const mdtClUnitConnectionData & rangeData) {
}

