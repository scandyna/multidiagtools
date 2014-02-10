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
#include "mdtSqlRecord.h"
#include <QSqlError>
#include <QSqlField>

//#include <QDebug>

mdtSqlRecord::mdtSqlRecord()
{
}

mdtSqlRecord::mdtSqlRecord(const QSqlRecord & sqlRecord)
{
  pvRecord = sqlRecord;
}

mdtSqlRecord::~mdtSqlRecord()
{
}

mdtSqlRecord::operator QSqlRecord()
{
  return pvRecord;
}


bool mdtSqlRecord::addField(const QString & fieldName, const QString & tableName, const QSqlDatabase & db) 
{
  QSqlRecord dbRecord;
  QSqlField fld;

  // Get record object to fetch attributes
  dbRecord = db.record(tableName);
  if(dbRecord.isEmpty()){
    QSqlError sqlError = db.lastError();
    pvLastError.setError("Cannot get field information. Field: " + fieldName + ", table: " + tableName, mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRecord");
    pvLastError.commit();
    return false;
  }
  // Get field and add it
  fld = dbRecord.field(fieldName);
  if(!fld.isValid()){
    pvLastError.setError("Field '" + fieldName + "' was not found in table '" + tableName + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRecord");
    pvLastError.commit();
    return false;
  }
  fld.setGenerated(false);
  pvRecord.append(fld);

  return true;
}

bool mdtSqlRecord::addAllFields(const QString & tableName, const QSqlDatabase & db)
{
  QSqlRecord dbRecord;
  QSqlField fld;
  int i;

  // Get record object to fetch attributes
  dbRecord = db.record(tableName);
  if(dbRecord.isEmpty()){
    QSqlError sqlError = db.lastError();
    pvLastError.setError("Table '" + tableName + "' was not found in database.", mdtError::Error);
    if(sqlError.isValid()){
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
    }
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRecord");
    pvLastError.commit();
    return false;
  }
  // Get fields and add them
  for(i = 0; i < dbRecord.count(); ++i){
    fld = dbRecord.field(i);
    if(!fld.isValid()){
      pvLastError.setError("Field '" + fld.name() + "' was not found in table '" + tableName + "'", mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtSqlRecord");
      pvLastError.commit();
      return false;
    }
    fld.setGenerated(false);
    pvRecord.append(fld);
  }

  return true;
}

QStringList mdtSqlRecord::fieldNames() const
{
  QStringList names;
  int i;

  for(i = 0; i < count(); ++i){
    names.append(pvRecord.fieldName(i));
  }

  return names;
}

void mdtSqlRecord::setValue(int fieldIndex, const QVariant & val)
{
  pvRecord.setGenerated(fieldIndex, true);
  pvRecord.setValue(fieldIndex, val);
}

void mdtSqlRecord::setValue(const QString & fieldName, const QVariant & val)
{
  setValue(pvRecord.indexOf(fieldName), val);
}

void mdtSqlRecord::clear()
{
  pvLastError.clear();
  pvRecord.clear();
}

void mdtSqlRecord::clearValues()
{
  int i;

  for(i = 0; i < count(); ++i){
    pvRecord.setGenerated(i, false);
  }
  pvRecord.clearValues();
}

mdtError mdtSqlRecord::lastError() const
{
  return pvLastError;
}

QVector<int> mdtSqlRecord::fieldIndexesWithValue() const
{
  QVector<int> indexList;
  int i;

  for(i = 0; i < count(); ++i){
    if(hasValue(i)){
      indexList.append(i);
    }
  }

  return indexList;
}

QString mdtSqlRecord::sqlForInsert(const QString & tableName) const
{
  QString sql;
  QVector<int> indexList;
  int i;

  indexList = fieldIndexesWithValue();
  sql = "INSERT INTO " + tableName + " (";
  for(i = 0; i < indexList.size(); ++i){
    sql += pvRecord.fieldName(indexList.at(i));
    if(i < (indexList.size()-1)){
      sql += ",";
    }
  }
  sql += ") VALUES (";
  for(i = 0; i < indexList.size(); ++i){
    sql += "?";
    if(i < (indexList.size()-1)){
      sql += ",";
    }
  }
  sql += ")";

  return sql;
}

QString mdtSqlRecord::sqlForUpdate(const QString & tableName, const QSqlRecord & matchData) const
{
  QString sql;
  QVector<int> indexList;
  int index;
  QString delimiter;
  QSqlField field;
  int i;

  indexList = fieldIndexesWithValue();
  // Build base update SQL statement
  sql = "UPDATE " + tableName + " SET ";
  for(i = 0; i < indexList.size(); ++i){
    index = indexList.at(i);
    sql += pvRecord.fieldName(index) + "=?";
    if(i < (indexList.size()-1)){
      sql += ",";
    }
  }
  // Add row condition
  if(matchData.count() > 0){
    field = matchData.field(0);
    delimiter = sqlDataDelimiter(field.type());
    sql += " WHERE " + field.name() + "=" + delimiter + field.value().toString() + delimiter;
    for(i = 1; i < matchData.count(); ++i){
      field = matchData.field(i);
      delimiter = sqlDataDelimiter(field.type());
      sql += " AND " + field.name() + "=" + delimiter + field.value().toString() + delimiter;
    }
  }

  return sql;
}

QString mdtSqlRecord::sqlDataDelimiter(QVariant::Type type)
{
  switch(type){
    case QVariant::String:
      return "'";
    default:
      return "";
  }
}

QString mdtSqlRecord::dataToUpdateInfo() const
{
  QString str;
  QVector<int> indexList;
  int i;

  indexList = fieldIndexesWithValue();
  for(i = 0; i < indexList.size(); ++i){
    str += pvRecord.fieldName(indexList.at(i));
    str += ": '" + pvRecord.value(indexList.at(i)).toString() + "'";
    if(i < (indexList.size()-1)){
      str += " , ";
    }
  }

  return str;
}
