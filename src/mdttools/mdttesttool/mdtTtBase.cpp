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
#include "mdtTtBase.h"
#include <QSqlError>
#include <QSqlError>
#include <QVector>

//#include <QDebug>

/// \todo Add tr() in errors

mdtTtBase::mdtTtBase(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
}

mdtTtBase::~mdtTtBase() 
{
}

QSqlDatabase mdtTtBase::database() 
{
  return pvDatabase;
}

mdtError mdtTtBase::lastError() 
{
  return pvLastError;
}

bool mdtTtBase::addRecord(const mdtSqlRecord & record, const QString & tableName)
{
  QSqlQuery query(database());
  return addRecord(record, tableName, query);
}

bool mdtTtBase::addRecord(const mdtSqlRecord & record, const QString & tableName, QSqlQuery & query)
{
  QString sql;
  QVector<int> indexList;
  int i;

  indexList = record.fieldIndexesWithValue();
  if(indexList.isEmpty()){
    pvLastError.setError(tr("Cannot inertion data in table '") + tableName + tr("' : given record contains no data to update."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }
  sql = record.sqlForInsert(tableName);
  // Prepare query
  if(!query.prepare(sql)){
    QString msg;
    QSqlError sqlError = query.lastError();
    msg = tr("Cannot prepare query for inertion in table '") + tableName + tr("'. ");
    msg += tr("Data: ") + record.dataToUpdateInfo() + tr(".");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  for(i = 0; i < indexList.size(); ++i){
    query.bindValue(i, record.value(indexList.at(i)));
  }
  if(!query.exec()){
    QString msg;
    QSqlError sqlError = query.lastError();
    msg = tr("Cannot exec query for inertion in table '") + tableName + tr("'. ");
    msg += tr("Data: ") + record.dataToUpdateInfo() + tr(".");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

/**
bool mdtTtBase::addRecordList(const QList<mdtSqlRecord> & recordList, const QString & tableName, bool singleTransaction )
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
*/

QList<QSqlRecord> mdtTtBase::getData(const QString & sql, bool *ok, const QStringList & expectedFields)
{
  QSqlQuery query(database());
  QList<QSqlRecord> dataList;

  if(!processGetData(sql, query, expectedFields)){
    if(ok != 0){
      *ok = false;
      return dataList;
    }
  }
  // Get data
  while(query.next()){
    dataList.append(query.record());
  }
  if(ok != 0){
    *ok = true;
  }

  return dataList;
}

bool mdtTtBase::updateRecord(const QString & tableName, const mdtSqlRecord & record, const QSqlRecord & matchData)
{
  QSqlQuery query(database());
  QString sql;
  QVector<int> indexList;
  int i;

  // Set base SQL statement and prepare query
  indexList = record.fieldIndexesWithValue();
  sql = record.sqlForUpdate(tableName, matchData);
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot prepare query to update table '") + tableName + tr("'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  for(i = 0; i < indexList.size(); ++i){
    query.bindValue(i, record.value(indexList.at(i)));
  }
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot exec query to update table '") + tableName + tr("'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtBase::updateRecord(const QString & tableName, const mdtSqlRecord & record, const QString & matchField, const QVariant & matchData)
{
  mdtSqlRecord matchRecord;

  if(!matchRecord.addField(matchField, tableName, database())){
    pvLastError = matchRecord.lastError();
    return false;
  }
  matchRecord.setValue(matchField, matchData);

  return updateRecord(tableName, record, matchRecord);
}

bool mdtTtBase::removeData(const QString & tableName, const QStringList & fields, const QModelIndexList & indexes)
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
  if(max > fields.size()){
    pvLastError.setError(tr("Cannot remove rows from table '") + tableName + tr(": indexes contains more columns than fields list."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
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
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError;
    QString msg;
    sqlError = query.lastError();
    msg = tr("Cannot remove rows from table '") + tableName + tr("'.");
    msg += "\n" + tr("SQL statement : '") + sql + tr("'.");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtBase::removeData(const QString & tableName, const QString & fieldName, const QModelIndexList & indexes)
{
  return removeData(tableName, QStringList(fieldName), indexes);
}

bool mdtTtBase::removeData(const QString & tableName, const QString & fieldName, const QVariant & matchData)
{
  QString sql;
  QString delimiter;

  // Generate SQL statement
  delimiter = sqlDataDelimiter(matchData.type());
  sql = "DELETE FROM " + tableName + " WHERE " + fieldName + " = " + delimiter + matchData.toString() + delimiter;
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError;
    QString msg;
    sqlError = query.lastError();
    msg = tr("Cannot remove rows from table '") + tableName + tr("'.");
    msg += "\n" + tr("SQL statement : '") + sql + tr("'.");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtBase::removeData(const QString & tableName, const QString & fieldName1, const QVariant & matchData1, const QString & fieldName2, const QVariant & matchData2)
{
  QString sql;
  QString delimiter1, delimiter2;

  // Generate SQL statement
  delimiter1 = sqlDataDelimiter(matchData1.type());
  delimiter2 = sqlDataDelimiter(matchData2.type());
  sql = "DELETE FROM " + tableName + " WHERE " + fieldName1 + " = " + delimiter1 + matchData1.toString() + delimiter1;
  sql += " AND " + fieldName2 + " = " + delimiter2 + matchData2.toString() + delimiter2;
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError;
    sqlError = query.lastError();
    pvLastError.setError(tr("Cannot remove row from table '") + tableName + tr("'."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtBase::removeData(const QString& tableName, const mdtSqlTableSelection & s, bool handleTransaction)
{
  QString sql;
  int row;
  QStringList fieldList;
  QString field;
  QVariant data;
  QString delimiter;
  int i;

  if(s.isEmpty()){
    return true;
  }
  // Generate SQL statement
  sql = "DELETE FROM " + tableName;
  Q_ASSERT(s.rowCount() > 0);
  // Firs row
  sql += " WHERE (";
  fieldList = s.fields(0);
  // First field
  if(fieldList.size() > 0){
    field = fieldList.at(0);
    data = s.data(0, field);
    delimiter = sqlDataDelimiter(data.type());
    sql += field + "=" + delimiter + data.toString() + delimiter;
  }
  // Other fields
  for(i = 1; i < fieldList.size(); ++i){
    sql += " AND ";
    field = fieldList.at(i);
    data = s.data(0, field);
    delimiter = sqlDataDelimiter(data.type());
    sql += field + "=" + delimiter + data.toString() + delimiter;
  }
  sql += ")";
  // Other rows
  for(row = 1; row < s.rowCount(); ++row){
    sql += " OR (";
    fieldList = s.fields(row);
    // First field
    if(fieldList.size() > 0){
      field = fieldList.at(0);
      data = s.data(row, field);
      delimiter = sqlDataDelimiter(data.type());
      sql += field + "=" + delimiter + data.toString() + delimiter;
    }
    // Other fields
    for(i = 1; i < fieldList.size(); ++i){
      sql += " AND ";
      field = fieldList.at(i);
      data = s.data(row, field);
      delimiter = sqlDataDelimiter(data.type());
      sql += field + "=" + delimiter + data.toString() + delimiter;
    }
    sql += ")";
  }
  // Begin transaction
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError;
    QString msg;
    sqlError = query.lastError();
    msg = tr("Cannot remove rows from table '") + tableName + tr("'.");
    msg += "\n" + tr("SQL statement : '") + sql + tr("'.");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    if(handleTransaction){
      rollbackTransaction();
    }
    pvLastError.commit();
    return false;
  }
  // Commit
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtTtBase::beginTransaction() 
{
  if(!pvDatabase.transaction()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn transaction (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtTtBase::rollbackTransaction() 
{
  if(!pvDatabase.rollback()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn rollback (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtTtBase::commitTransaction() 
{
  if(!pvDatabase.commit()){
    QSqlError sqlError = pvDatabase.lastError();
    pvLastError.setError(tr("Cannot beginn commit (database: '") + pvDatabase.databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }
  return true;
}

QString mdtTtBase::sqlDataDelimiter(QVariant::Type type)
{
  return mdtSqlRecord::sqlDataDelimiter(type);
}

bool mdtTtBase::processGetData(const QString & sql, QSqlQuery & query, const QStringList & expectedFields)
{
  int i;

  // Execute query
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot exec query: '") + sql + tr("'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
    pvLastError.commit();
    return false;
  }
  // If requested, check that expected fields exists in result
  if(expectedFields.size() > 0){
    QStringList missingFields;
    QSqlRecord record = query.record();
    for(i = 0; i < expectedFields.size(); ++i){
      if(record.indexOf(expectedFields.at(i)) < 0){
        missingFields.append(expectedFields.at(i));
      }
    }
    if(missingFields.size() > 0){
      QString text = tr("A query returned not all expected fields. Missing fields:\n");
      for(i = 0; i < missingFields.size(); ++i){
        text += " - " + missingFields.at(i) + "\n";
      }
      text += tr("SQL statement: '") + sql + tr("'");
      pvLastError.setError(text, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
      pvLastError.commit();
      return false;
    }
  }

  return true;
}
