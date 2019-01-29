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
#include "QueryBenchmark.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/InsertStatement.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/UpdateStatement.h"
#include "Mdt/Sql/UpdateQuery.h"
#include "Mdt/Sql/DeleteStatement.h"
#include "Mdt/Sql/DeleteQuery.h"
#include "Mdt/Sql/SelectQuery.h"
#include "Mdt/Error.h"
#include "Mdt/ErrorCode.h"
#include "Schema/TestSchema.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include <QSqlQuery>
#include <QSqlError>

// #include <QDebug>

using Mdt::Sql::FieldName;

/*
 * Init/cleanup functions
 */

void QueryBenchmark::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createClientTable());
}

void QueryBenchmark::cleanupTestCase()
{
}

/*
 * Benchmarks
 */

void QueryBenchmark::insertStatementSet()
{
  Mdt::Sql::InsertStatement statement;
  QString expectedSql;
  const auto db = database();

  QBENCHMARK{
    statement.clear();
    statement.setTableName("Client_tbl");
    statement.addValue(FieldName("FirstName"), "F 1");
    statement.addValue(FieldName("LastName"), "L 1");
  }
  expectedSql = "INSERT INTO \"Client_tbl\" (\"FirstName\",\"LastName\") VALUES (?,?)";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  QCOMPARE(statement.toValueList(), QVariantList({"F 1","L 1"}));
}

void QueryBenchmark::insertStatementToPrepareSql()
{
  Mdt::Sql::InsertStatement statement;
  QString sql;
  QString expectedSql;
  const auto db = database();

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("FirstName"), "F 1");
  statement.addValue(FieldName("LastName"), "L 1");
  QBENCHMARK{
    sql = statement.toPrepareStatementSql(db);
  }
  expectedSql = "INSERT INTO \"Client_tbl\" (\"FirstName\",\"LastName\") VALUES (?,?)";
  QCOMPARE(sql, expectedSql);
  QCOMPARE(statement.toValueList(), QVariantList({"F 1","L 1"}));
}

void QueryBenchmark::insertStatementToValueList()
{
  Mdt::Sql::InsertStatement statement;
  QVariantList valueList;
  QString expectedSql;
  const auto db = database();

  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("FirstName"), "F 1");
  statement.addValue(FieldName("LastName"), "L 1");
  QBENCHMARK{
    valueList = statement.toValueList();
  }
  expectedSql = "INSERT INTO \"Client_tbl\" (\"FirstName\",\"LastName\") VALUES (?,?)";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  QCOMPARE(valueList, QVariantList({"F 1","L 1"}));
}

void QueryBenchmark::insertStatementSetAndToPrepareSql()
{
  Mdt::Sql::InsertStatement statement;
  QString sql;
  QString expectedSql;
  QVariantList valueList;
  const auto db = database();

  QBENCHMARK{
    statement.clear();
    statement.setTableName("Client_tbl");
    statement.addValue(FieldName("FirstName"), "F 1");
    statement.addValue(FieldName("LastName"), "L 1");
    sql = statement.toPrepareStatementSql(db);
    valueList = statement.toValueList();
  }
  expectedSql = "INSERT INTO \"Client_tbl\" (\"FirstName\",\"LastName\") VALUES (?,?)";
  QCOMPARE(sql, expectedSql);
  QCOMPARE(valueList, QVariantList({"F 1","L 1"}));
}

/*
 * Helpers
 */

Mdt::Sql::PrimaryKeyRecord QueryBenchmark::buildPrimaryKeyRecord(int id)
{
  Mdt::Sql::PrimaryKeyRecord pkr;
  pkr.addValue(FieldName("Id_PK"), id);

  return pkr;
}

Mdt::Sql::PrimaryKeyRecord QueryBenchmark::buildPrimaryKeyRecord(int idA, int idB)
{
  Mdt::Sql::PrimaryKeyRecord pkr;
  pkr.addValue(FieldName("IdA_PK"), idA);
  pkr.addValue(FieldName("IdB_PK"), idB);

  return pkr;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QueryBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
