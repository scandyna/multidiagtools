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
#include "DriverMySql.h"
#include "Mdt/Sql/Error.h"
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringBuilder>
#include <QLocale>

// #include <QDebug>

namespace Mdt{ namespace Sql{ namespace Schema{

DriverMySql::DriverMySql(const QSqlDatabase& db)
 : DriverImplementationInterface(db)
{
  Q_ASSERT(qsqlDriver()->dbmsType() == QSqlDriver::MySqlServer);
}

Charset DriverMySql::getDatabaseDefaultCharset() const
{
  Charset cs;
  QSqlQuery query(database());

  const QString sql = QStringLiteral("SELECT DEFAULT_CHARACTER_SET_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = '") \
                    % database().databaseName() % QStringLiteral("'");
  if(!query.exec(sql)){
    QString msg = tr("Fetching database default character set failed.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverMySql");
    error.stackError(mdtErrorFromQSqlQuery(query, "DriverMySql"));
    error.commit();
    setLastError(error);
    return cs;
  }
  if(!query.next()){
    QString msg = tr("Fetching database default character set failed.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "DriverMySql");
    error.commit();
    setLastError(error);
    return cs;
  }
  cs.setCharsetName( query.value(0).toString() );

  return cs;
}

QString DriverMySql::getCollationDefinition(const Collation & collation) const
{
  QString sql;

  if(collation.isNull()){
    return sql;
  }
  /*
   * We have to build charset_language_cs string,
   * so get charset name
   */
  const QString charsetName = getDatabaseDefaultCharset().charsetName();
  if(charsetName.isEmpty()){
    return sql;
  }
  /*
   * Get language name
   */
  QString languageName;
  if(collation.locale().isNull()){
    languageName = QStringLiteral("general");
  }else{
    languageName = QLocale::languageToString(collation.locale().language()).toLower();
  }
  /*
   * Build SQL
   */
  switch(collation.caseSensitivity()){
    case CaseSensitivity::NotDefined:
    case CaseSensitivity::CaseInsensitive:
      sql = QStringLiteral("COLLATE ") % charsetName % QStringLiteral("_") % languageName % QStringLiteral("_ci");
      break;
    case CaseSensitivity::CaseSensitive:
      sql = QStringLiteral("COLLATE ") % charsetName % QStringLiteral("_bin");
      break;
  }

  return sql;
}

QString DriverMySql::getFieldDefinition(const Field & field) const
{

}

QString DriverMySql::getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey & pk) const
{

}

Mdt::Expected<PrimaryKeyContainer> DriverMySql::getTablePrimaryKeyFromDatabase(const QString& tableName) const
{

}

Mdt::Expected<ForeignKeyList> DriverMySql::getTableForeignKeyListFromDatabase(const QString& tableName) const
{

}

Mdt::Expected< IndexList > DriverMySql::getTableIndexListFromDatabase(const QString& tableName) const
{
  /*
   * To get indexes, see INFORMATION_SCHEMA.STATISTICS
   *
   * To see which column in table has a constraint,
   * see INFORMATION_SCHEMA.KEY_COLUMN_USAGE.
   *
   * By building a query, we can also filter indexes that where created by CREATE INDEX,
   *  and not automatically created by a (typically UNIQUE) column constraint.
   */
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
