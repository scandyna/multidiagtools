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
#ifndef MDT_SQL_SCHEMA_DRIVER_POSTGRESQL_TEST_H
#define MDT_SQL_SCHEMA_DRIVER_POSTGRESQL_TEST_H

#include "TestBase.h"
#include "Mdt/Sql/Connection.h"
#include <QString>

class SchemaDriverPostgreSqlTest : public QObject
{
 Q_OBJECT

 private:

  bool initDatabasePostgreSql();

  Mdt::Sql::Connection connection() const
  {
    Q_ASSERT(!mConnectionName.isEmpty());
    return Mdt::Sql::Connection(mConnectionName);
  }

  QSqlDatabase database() const
  {
    Q_ASSERT(!mConnectionName.isEmpty());
    return connection().database();
  }

  bool isDatabaseOpen() const
  {
    if(mConnectionName.isEmpty()){
      return false;
    }
    return database().isOpen();
  }

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void sandbox();

  void driverInstanceTest();

  void availableFieldTypeTest();
  void fieldTypeNameTest();
  void fieldTypeFromStringTest();
  void fieldLengthFromStringTest();

  void databaseDefaultCharsetTest();
  void collationDefinitionTest();
  void fieldDefinitionTest();
  void autoIncrementPrimaryKeyDefinitionTest();
  void singleFieldPrimaryKeyDefinitionTest();
  void primaryKeyDefinitionTest();
  void indexDefinitionTest();

  void createTableTest();

 private:

  bool dropTable(const QString & tableName);

  QString mConnectionName;
};

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_POSTGRESQL_TEST_H
