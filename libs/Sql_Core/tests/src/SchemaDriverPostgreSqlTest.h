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
#ifndef MDT_SQL_SCHEMA_DRIVER_POSTGRESQL_TEST_H
#define MDT_SQL_SCHEMA_DRIVER_POSTGRESQL_TEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <QTemporaryFile>
#include <QSqlDatabase>

class SchemaDriverPostgreSqlTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void sandbox();

  void driverInstanceTest();

  void availableFieldTypeTest();
  void fieldTypeMapTest();

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

  QTemporaryFile pvTempFile;  // We keep it as member, so file is destroyed automatically
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_POSTGRESQL_TEST_H
