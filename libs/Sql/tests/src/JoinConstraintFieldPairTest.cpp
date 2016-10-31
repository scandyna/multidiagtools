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
#include "JoinConstraintFieldPairTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Impl/JoinConstraintFieldPair.h"
#include "Mdt/Sql/Impl/JoinConstraintFieldPairList.h"
#include "Mdt/Sql/Impl/JoinConstraintFieldPairListSqlTransform.h"

namespace Sql = Mdt::Sql;

void JoinConstraintFieldPairTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void JoinConstraintFieldPairTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void JoinConstraintFieldPairTest::fieldPairTest()
{
  using Sql::Impl::JoinConstraintFieldPair;

  JoinConstraintFieldPair fp("a", "b");
  QCOMPARE(fp.leftField(), QString("a"));
  QCOMPARE(fp.rightField(), QString("b"));
}

void JoinConstraintFieldPairTest::fieldPairListTest()
{
  using Sql::Impl::JoinConstraintFieldPairList;

  /*
   * Initial state
   */
  JoinConstraintFieldPairList list("A", "B");
  QCOMPARE(list.leftTable(), QString("A"));
  QCOMPARE(list.rightTable(), QString("B"));
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Build
   */
  list.addFieldPair("a1", "b1");
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).leftField(), QString("a1"));
  QCOMPARE(list.at(0).rightField(), QString("b1"));
}

void JoinConstraintFieldPairTest::sqlTransformTest()
{
  using Sql::Impl::JoinConstraintFieldPairList;
  using Sql::Impl::JoinConstraintFieldPairListSqlTransform;

  auto db = mDatabase;
  QString expectedSql;
  JoinConstraintFieldPairList list("A", "B");

  // 1 pair of fields
  list.addFieldPair("a1", "b1");
  expectedSql = "\"A\".\"a1\"=\"B\".\"b1\"";
  QCOMPARE( JoinConstraintFieldPairListSqlTransform::getSql(list, db) , expectedSql );
  // 2 pair of fields
  list.addFieldPair("a2", "b2");
  expectedSql = "\"A\".\"a1\"=\"B\".\"b1\" AND \"A\".\"a2\"=\"B\".\"b2\"";
  QCOMPARE( JoinConstraintFieldPairListSqlTransform::getSql(list, db) , expectedSql );
  // 3 pair of fields
  list.addFieldPair("a3", "b3");
  expectedSql = "\"A\".\"a1\"=\"B\".\"b1\" AND \"A\".\"a2\"=\"B\".\"b2\" AND \"A\".\"a3\"=\"B\".\"b3\"";
  QCOMPARE( JoinConstraintFieldPairListSqlTransform::getSql(list, db) , expectedSql );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  JoinConstraintFieldPairTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
