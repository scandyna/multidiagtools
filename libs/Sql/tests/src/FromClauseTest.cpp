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
#include "FromClauseTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/FromClause.h"
#include "Mdt/Sql/FromClauseSqlTransform.h"
#include "Mdt/Sql/SelectTable.h"
#include "Mdt/Sql/TableName.h"
#include <QString>

namespace Sql = Mdt::Sql;

void FromClauseTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void FromClauseTest::cleanupTestCase()
{
}


/*
 * Tests
 */

void FromClauseTest::setTableTest()
{
  using Sql::TableName;
  using Sql::SelectTable;
  using Sql::FromClause;

  SelectTable table(TableName("Client_tbl"), "CLI");
  /*
   * Initial state
   */
  FromClause clause;
  QVERIFY(clause.isNull());
  /*
   * Set table
   */
  clause.setTable(table);
  QVERIFY(!clause.isNull());
  QCOMPARE( boost::get<SelectTable>(clause.clause()).tableName() , QString("Client_tbl") );
}

void FromClauseTest::setJoinClauseTest()
{
  QFAIL("Not implemented");
}

void FromClauseTest::setSqlStringTest()
{
  using Sql::FromClause;

  /*
   * Initial state
   */
  FromClause clause;
  QVERIFY(clause.isNull());
  /*
   * Set SQL string
   */
  clause.setSqlString("Client_tbl CLI, Address_tbl ADR");
  QVERIFY(!clause.isNull());
  QCOMPARE( boost::get<QString>(clause.clause()) , QString("Client_tbl CLI, Address_tbl ADR") );
}

void FromClauseTest::switchTableSqlStringTest()
{
  using Sql::TableName;
  using Sql::SelectTable;
  using Sql::FromClause;

  SelectTable table(TableName("A_tbl"), "A");
  /*
   * Initial state
   */
  FromClause clause;
  QVERIFY(clause.isNull());
  /*
   * Set table
   */
  clause.setTable(table);
  QVERIFY(!clause.isNull());
  QCOMPARE( boost::get<SelectTable>(clause.clause()).tableName() , QString("A_tbl") );
  /*
   * Set SQL string
   */
  clause.setSqlString("A_tbl A, B_tbl B");
  QVERIFY(!clause.isNull());
  QCOMPARE( boost::get<QString>(clause.clause()) , QString("A_tbl A, B_tbl B") );
}

void FromClauseTest::tableSqlTransformTest()
{
  using Sql::TableName;
  using Sql::SelectTable;
  using Sql::FromClause;
  using Sql::FromClauseSqlTransform;

  const auto db = mDatabase;
  QString expectedSql;
  SelectTable A(TableName("A_tbl"), "A");
  SelectTable B(TableName("B_tbl"));
  FromClause clause;

  clause.setTable(A);
  expectedSql = "FROM \"A_tbl\" \"A\"";
  QCOMPARE( FromClauseSqlTransform::getSql(clause, db) , expectedSql );

  clause.setTable(B);
  expectedSql = "FROM \"B_tbl\"";
  QCOMPARE( FromClauseSqlTransform::getSql(clause, db) , expectedSql );
}

void FromClauseTest::joinClauseSqlTransformTest()
{
  QFAIL("Not implemented");
}

void FromClauseTest::sqlStringSqlTransformTest()
{
  using Sql::FromClause;
  using Sql::FromClauseSqlTransform;

  const auto db = mDatabase;
  QString expectedSql;
  FromClause clause;

  clause.setSqlString("A_tbl, B_tbl");
  expectedSql = "FROM A_tbl, B_tbl";
  QCOMPARE( FromClauseSqlTransform::getSql(clause, db) , expectedSql );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FromClauseTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
