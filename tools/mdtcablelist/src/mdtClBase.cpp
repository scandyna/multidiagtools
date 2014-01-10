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
#include "mdtClBase.h"
#include <QSqlError>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

mdtClBase::mdtClBase(QSqlDatabase db) 
{
  pvDatabase = db;
}

mdtClBase::~mdtClBase() 
{
}

QSqlDatabase mdtClBase::database() 
{
  return pvDatabase;
}

mdtError mdtClBase::lastError() 
{
  return pvLastError;
}

bool mdtClBase::addRecord(const mdtSqlRecord & record, const QString & tableName)
{
  QString sql;
  QSqlQuery query(database());
  int i, index, lastIndex;

  // Get last field index that has a value
  lastIndex = -1;
  for(i = 0; i < record.count(); ++i){
    if(record.hasValue(i)){
      lastIndex = i;
    }
  }
  // Prepare query for insertion
  sql = "INSERT INTO " + tableName +" (";
  for(i = 0; i <= lastIndex; ++i){
    if(record.hasValue(i)){
      sql += record.fieldName(i);
      if(i < (lastIndex-1)){
        sql += ",";
      }
    }
  }
  sql += ") VALUES (";
  for(i = 0; i <= lastIndex; ++i){
    if(record.hasValue(i)){
      sql += "?";
      if(i < (lastIndex-1)){
        sql += ",";
      }
    }
  }
  sql += ")";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for inertion in table '" + tableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  index = 0;
  for(i = 0; i <= lastIndex; ++i){
    if(record.hasValue(i)){
      query.bindValue(index, record.value(i));
      ++index;
    }
  }
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot exec query for inertion in table '" + tableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClBase::addRecordList(const QList<mdtSqlRecord> & recordList, const QString & tableName, bool singleTransaction )
{
  int i;

  if(singleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  for(i = 0; i < recordList.size(); ++i){
    if(!addRecord(recordList.at(i), tableName)){
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

bool mdtClBase::removeData(const QString & tableName, const QStringList & fields, const QModelIndexList & indexes)
{
  int i;
  int col;
  int max;
  QVariant data;
  QString sql;
  QString delimiter;

  // If no index was given, we simply do nothing
  if(indexes.isEmpty()){
    return true;
  }
  // Check that indexes contains not more columns than fields
  max = 0;
  for(i = 0; i < indexes.size(); ++i){
    col = indexes.at(i).column();
    if(col > max){
      max = col;
    }
  }
  ///qDebug() << "indexes max column : " << max;
  if(max > fields.size()){
    pvLastError.setError("Cannot remove rows in table " + tableName + ": indexes contains more columns than fields list.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  // Generate SQL statement
  sql = "DELETE FROM " + tableName;
  for(i = 0; i < indexes.size(); ++i){
    if(i == 0){
      sql += " WHERE (";
    }else{
      sql += " OR ";
    }
    col = indexes.at(i).column();
    data = indexes.at(i).data();
    // Set the delimiter
    delimiter = sqlDataDelimiter(data.type());
    Q_ASSERT(col < fields.size());
    sql += fields.at(col) + " = " + delimiter + data.toString() + delimiter;
  }
  sql += ")";
  ///qDebug() << "SQL: " << sql;
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError;
    sqlError = query.lastError();
    pvLastError.setError("Cannot remove rows in table " + tableName, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClBase::removeData(const QString & tableName, const QString & fieldName, const QModelIndexList & indexes)
{
  return removeData(tableName, QStringList(fieldName), indexes);
}

bool mdtClBase::removeData(const QString & tableName, const QString & fieldName, const QVariant & matchData)
{
  QString sql;
  QString delimiter;

  // Generate SQL statement
  delimiter = sqlDataDelimiter(matchData.type());
  sql = "DELETE FROM " + tableName + " WHERE " + fieldName + " = " + delimiter + matchData.toString() + delimiter;
  ///qDebug() << "SQL: " << sql;
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError;
    sqlError = query.lastError();
    pvLastError.setError("Cannot remove row in table " + tableName, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClBase::beginTransaction() 
{
  if(!pvDatabase.transaction()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError("Cannot beginn transaction (database: " + pvDatabase.databaseName() + ")", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtClBase::rollbackTransaction() 
{
  if(!pvDatabase.rollback()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError("Cannot beginn rollback (database: " + pvDatabase.databaseName() + ")", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtClBase::commitTransaction() 
{
  if(!pvDatabase.commit()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError("Cannot beginn commit (database: " + pvDatabase.databaseName() + ")", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

QString mdtClBase::sqlDataDelimiter(QVariant::Type type)
{
  switch(type){
    case QVariant::String:
      return "'";
    default:
      return "";
  }
}
