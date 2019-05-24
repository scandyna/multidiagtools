/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "DriverPostgreSQL.h"
#include "FieldTypeName.h"
#include "Mdt/Sql/Error.h"
#include <QStringBuilder>
#include <QLatin1String>
#include <QLatin1Char>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QLocale>

namespace Mdt{ namespace Sql{ namespace Schema{

DriverPostgreSQL::DriverPostgreSQL(const Connection & connection)
 : DriverImplementationInterface(connection)
{
  Q_ASSERT(qsqlDriver()->dbmsType() == QSqlDriver::PostgreSQL);
}

QString DriverPostgreSQL::fieldTypeName(FieldType fieldType) const
{
  switch(fieldType){
    case FieldType::Float:
      return QLatin1String("REAL");
    case FieldType::Double:
      return QLatin1String("DOUBLE PRECISION");
    case FieldType::Char:
      return QLatin1String("CHARACTER");
    case FieldType::DateTime:
      return QLatin1String("TIMESTAMP");
    default:
      break;
  }
  return BaseClass::fieldTypeName(fieldType);
}

Charset DriverPostgreSQL::getDatabaseDefaultCharset() const
{
  Charset cs;
  QSqlQuery query(database());

  const QString sql = QStringLiteral("SELECT character_set_name FROM information_schema.character_sets WHERE default_collate_catalog = '") \
                    % database().databaseName() % QStringLiteral("'");
  if(!query.exec(sql)){
    QString msg = tr("Fetching database default character set failed.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverPostgreSQL");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverPostgreSQL"));
    error.commit();
    setLastError(error);
    return cs;
  }
  if(!query.next()){
    QString msg = tr("Fetching database default character set failed.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverPostgreSQL");
    error.commit();
    setLastError(error);
    return cs;
  }
  cs.setCharsetName( query.value(0).toString() );

  return cs;
}

QString DriverPostgreSQL::getCollationDefinition(const Collation& collation) const
{
  QString sql;

  /*
   * PostgreSQL does not support specifying case sensitivity in COLLATE option.
   * The only possible way to make a field case insensitive is to use a CITEXT field type.
   */
  if(collation.locale().isNull()){
    return sql;
  }
  QLocale locale(collation.locale().language(), collation.locale().country());
  sql = QStringLiteral("COLLATE \"") % locale.name() % QStringLiteral("\"");

  return sql;
}

QString DriverPostgreSQL::getFieldDefinition(const Field& field) const
{

}

QString DriverPostgreSQL::getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey& pk) const
{
  QString serialTypeName;

  switch(pk.fieldType()){
    case FieldType::Smallint:
      serialTypeName = QLatin1String("SMALLSERIAL");
      break;
    case FieldType::Integer:
      serialTypeName = QLatin1String("SERIAL");
      break;
    case FieldType::Bigint:
      serialTypeName = QLatin1String("BIGSERIAL");
      break;
    default:
      break;
  }

  return escapeFieldName(pk.fieldName()) % QLatin1Char(' ') % serialTypeName;
}

Mdt::Expected<PrimaryKeyContainer> DriverPostgreSQL::getTablePrimaryKeyFromDatabase(const QString& tableName) const
{

}

Mdt::Expected<ForeignKeyList> DriverPostgreSQL::getTableForeignKeyListFromDatabase(const QString& tableName) const
{

}

Mdt::Expected<IndexList> DriverPostgreSQL::getTableIndexListFromDatabase(const QString& tableName) const
{

}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
