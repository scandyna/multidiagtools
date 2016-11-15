/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "InsertQuery.h"
#include "Schema/AutoIncrementPrimaryKey.h"
#include "Schema/Field.h"
#include "Schema/Table.h"
#include "Mdt/Sql/Error.h"
#include <QSqlRecord>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QChar>
#include <QObject>

namespace Mdt{ namespace Sql{

InsertQuery::InsertQuery(const QSqlDatabase & db)
 : pvDatabase(db)
{
  Q_ASSERT(pvDatabase.isValid());
}

void InsertQuery::setTableName(const QString & name)
{
  pvTableName = name;
}

void InsertQuery::setTable(const Schema::Table& table)
{
  pvTableName = table.tableName();
}

void InsertQuery::addValue(const FieldName & fieldName, const QVariant & value)
{
  pvFieldNameList << escapeFieldName(fieldName.toString());
  pvValueList.append(value);
}

void InsertQuery::addValue(const Schema::Field & field, const QVariant & value)
{
  pvFieldNameList << escapeFieldName(field.name());
  pvValueList.append(value);
}

void InsertQuery::addValue(const Schema::AutoIncrementPrimaryKey& field, const QVariant& value)
{
  pvFieldNameList << escapeFieldName(field.fieldName());
  pvValueList.append(value);
}

bool InsertQuery::exec()
{
  const QString sql = getPrepareStatement();

  if(!pvQuery.prepare(sql)){
    QString msg = tr("Preparing query for insertion into '%1' failed.").arg(pvTableName);
    pvLastError = mdtErrorNew(msg, Mdt::Error::Critical, "InsertQuery");
    pvLastError.stackError(mdtErrorFromQSqlQuery(pvQuery, "InsertQuery"));
    pvLastError.commit();
    return false;
  }
  for(const auto & value : pvValueList){
    pvQuery.addBindValue(value);
  }
  if(!pvQuery.exec()){
    QString msg = tr("Executing query for insertion into '%1' failed.").arg(pvTableName);
    pvLastError = mdtErrorNew(msg, Mdt::Error::Critical, "InsertQuery");
    pvLastError.stackError(mdtErrorFromQSqlQuery(pvQuery, "InsertQuery"));
    pvLastError.commit();
    return false;
  }

  return true;
}

void InsertQuery::clear()
{
  pvTableName.clear();
  pvFieldNameList.clear();
  pvValueList.clear();
}

QString InsertQuery::getPrepareStatement() const
{
  Q_ASSERT(pvFieldNameList.size() == pvValueList.size());

  QString sql;
  const int n = pvValueList.size();
  QStringList placeHolders;

  placeHolders.reserve(n);
  for(int i = 0; i < n; ++i){
    placeHolders.append(QChar('?'));
  }
  // Build SQL
  sql = QStringLiteral("INSERT INTO ") % escapeTableName(pvTableName) \
      % QStringLiteral(" (") % pvFieldNameList.join(QChar(',')) \
      % QStringLiteral(") VALUES (") \
      % placeHolders.join(QChar(',')) \
      % QStringLiteral(")");
  
  return sql;
}

QString InsertQuery::escapeFieldName(const QString& fieldName) const
{
  Q_ASSERT(pvDatabase.driver() != nullptr);
  return pvDatabase.driver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString InsertQuery::escapeTableName(const QString& tableName) const
{
  Q_ASSERT(pvDatabase.driver() != nullptr);
  return pvDatabase.driver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

QString InsertQuery::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}

}} // namespace Mdt{ namespace Sql{
