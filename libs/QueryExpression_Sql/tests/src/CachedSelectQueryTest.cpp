/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "CachedSelectQueryTest.h"
#include "Mdt/QueryExpression/SqlSelectQuery.h"
#include "Mdt/QueryExpression/CachedSelectQuery.h"
#include "Mdt/Entity/QueryEntity.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/QueryExpression/SqlSelectQueryFactory.h"
#include <memory>

using namespace Mdt::Entity;
using Mdt::QueryExpression::SqlSelectQuery;
using Mdt::QueryExpression::CachedSelectQuery;

/*
 * Init / cleanup
 */

void CachedSelectQueryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createTestSchema());
}

void CachedSelectQueryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void CachedSelectQueryTest::nullQueryTest()
{
  Mdt::QueryExpression::CachedSelectQuery query;
  QVERIFY(query.isNull());
  QCOMPARE(query.rowCount(), 0);
  QCOMPARE(query.columnCount(), 0);
}

void CachedSelectQueryTest::execQueryTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  auto query = CachedSelectQuery::make<SqlSelectQuery>();
  query.impl<SqlSelectQuery>().setDatabase(database());

  QVERIFY(!query.isNull());
  QCOMPARE(query.rowCount(), 0);
  QCOMPARE(query.columnCount(), 0);

  QVERIFY(query.exec(stm));
  QCOMPARE(query.rowCount(), 2);
  QCOMPARE(query.columnCount(), 4);
  QCOMPARE(query.value(0, 0), QVariant(1));
  QCOMPARE(query.value(0, 1), QVariant("P1"));
  QCOMPARE(query.value(0, 2), QVariant(10));
  QCOMPARE(query.value(0, 3), QVariant("R1"));
  QCOMPARE(query.value(1, 0), QVariant(2));
  QCOMPARE(query.value(1, 1), QVariant("P2"));
  QCOMPARE(query.value(1, 2), QVariant(20));
  QCOMPARE(query.value(1, 3), QVariant("R2"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CachedSelectQueryTest test;

  return QTest::qExec(&test, argc, argv);
}
