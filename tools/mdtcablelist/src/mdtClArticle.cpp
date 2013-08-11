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

mdtClArticle::mdtClArticle(QSqlDatabase db)
{
  pvDatabase = db;
}

mdtClArticle::~mdtClArticle()
{
}

const QSqlError &mdtClArticle::lastError() const
{
  return pvLastError;
}

bool mdtClArticle::addLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode)
{
  QString sql;
  QSqlQuery query(pvDatabase);

  // Prepare query for insertion
  sql = "INSERT INTO Link_tbl (ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK, Value, LinkDirection_Code_FK, LinkType_Code_FK)\
                  VALUES (:ArticleConnectionStart_Id_FK, :ArticleConnectionEnd_Id_FK, :Value, :LinkDirection_Code_FK, :LinkType_Code_FK)";
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for link inertion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClArticle");
    e.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":ArticleConnectionStart_Id_FK", articleConnectionStartId);
  query.bindValue(":ArticleConnectionEnd_Id_FK", articleConnectionEndId);
  query.bindValue(":Value", value);
  query.bindValue(":LinkDirection_Code_FK", directionCode);
  query.bindValue(":LinkType_Code_FK", typeCode);
  if(!query.exec()){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for link inertion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClArticle");
    e.commit();
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

bool mdtClArticle::editLink(const QVariant &linkId, const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode)
{
  QString sql;
  QSqlQuery query(pvDatabase);

  // Prepare query for edition
  sql = "UPDATE Link_tbl \
         SET ArticleConnectionStart_Id_FK = :ArticleConnectionStart_Id_FK ,\
             ArticleConnectionEnd_Id_FK = :ArticleConnectionEnd_Id_FK ,\
             Value = :Value ,\
             LinkDirection_Code_FK = :LinkDirection_Code_FK ,\
             LinkType_Code_FK = :LinkType_Code_FK\
         WHERE Id_PK = " + linkId.toString();
  if(!query.prepare(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot prepare query for link edition", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClArticle");
    e.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":ArticleConnectionStart_Id_FK", articleConnectionStartId);
  query.bindValue(":ArticleConnectionEnd_Id_FK", articleConnectionEndId);
  query.bindValue(":Value", value);
  query.bindValue(":LinkDirection_Code_FK", directionCode);
  query.bindValue(":LinkType_Code_FK", typeCode);
  if(!query.exec()){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for link edition", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClArticle");
    e.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::removeLink(const QVariant & linkId)
{
  QList<QVariant> idList;

  idList.append(linkId);

  return removeLinks(idList);
}

bool mdtClArticle::removeLinks(const QList<QVariant> &linkIdList)
{
  int i;
  QString sql;

  if(linkIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "DELETE FROM Link_tbl ";
  for(i = 0; i < linkIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += "Id_PK = " + linkIdList.at(i).toString();
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(pvDatabase);
  if(!query.exec(sql)){
    pvLastError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot execute query for link deletion", mdtError::Error);
    e.setSystemError(pvLastError.number(), pvLastError.text());
    MDT_ERROR_SET_SRC(e, "mdtClArticle");
    e.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::removeLinks(const QModelIndexList & indexListOfSelectedRows)
{
  int i;
  QString sql;
  QList<QVariant> idList;

  for(i = 0; i < indexListOfSelectedRows.size(); ++i){
    idList.append(indexListOfSelectedRows.at(i).data());
  }

  return removeLinks(idList);
}
