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
#include "DriverSQLite.h"
#include <QStringBuilder>

namespace Mdt{ namespace Sql{ namespace Schema{

DriverSQLite::DriverSQLite(const QSqlDatabase& db)
 : DriverImplementationInterface(db)
{
  Q_ASSERT(qsqlDriver()->dbmsType() == QSqlDriver::SQLite);
}

QString DriverSQLite::getCollationDefinition(const Collation & collation) const
{
  if(collation.isNull()){
    return QString();
  }
  /*
   * SQLite has no locale support,
   * only case sensitivity for our case.
   * Note: in current version, we ignore RTRIM functionnality
   */
  if(collation.isCaseSensitive()){
    return QStringLiteral("COLLATE BINARY");
  }else{
    return QStringLiteral("COLLATE NOCASE");
  }
}

QString DriverSQLite::getFieldDefinition(const Field & field) const
{
  QString sql;

  // Field name and type
  sql = escapeFieldName(field.name()) % QStringLiteral(" ") % fieldTypeName(field.type());
  // Length
  if(field.length() > 0){
    sql += QStringLiteral("(") % QString::number(field.length()) % QStringLiteral(")");
  }
  // Null constraint
  if(field.isRequired()){
    sql += QStringLiteral(" NOT NULL");
  }
  // Default value
  if(field.defaultValue().isNull()){
    sql += QStringLiteral(" DEFAULT NULL");
  }else{
    sql += QStringLiteral(" DEFAULT ") + field.defaultValue().toString();
  }
  // Collation
  if(!field.collation().isNull()){
    sql += QStringLiteral(" ") % getCollationDefinition(field.collation());
  }

  return sql;
}

QString DriverSQLite::getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey & pk) const
{
  QString sql;

  sql = escapeFieldName(pk.fieldName()) \
      % QStringLiteral(" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT");

  return sql;
}

QString DriverSQLite::getPrimaryKeyFieldDefinition(const SingleFieldPrimaryKey& pk) const
{
  QString sql;

  // Field name and type
  sql = escapeFieldName(pk.fieldName()) % QStringLiteral(" ") % fieldTypeName(pk.fieldType());
  // Length
  if(pk.fieldLength() > 0){
    sql += QStringLiteral("(") % QString::number(pk.fieldLength()) % QStringLiteral(")");
  }
  // Primary key constraint
  sql += QStringLiteral(" NOT NULL PRIMARY KEY");
  // Collation
  if(!pk.collation().isNull()){
    sql += QStringLiteral(" ") % getCollationDefinition(pk.collation());
  }

  return sql;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
