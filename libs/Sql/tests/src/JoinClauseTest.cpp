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
#include "JoinClauseTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/JoinClauseItem.h"
#include "Mdt/Sql/JoinClauseItemSqlTransform.h"
#include "Mdt/Sql/JoinClauseItemList.h"
#include "Mdt/Sql/JoinClauseItemListSqlTransform.h"
#include "Mdt/Sql/JoinClause.h"
#include "Mdt/Sql/JoinClauseSqlTransform.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"

namespace Sql = Mdt::Sql;
/*
 * Init and cleanup
 */

void JoinClauseTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void JoinClauseTest::cleanupTestCase()
{
}


/*
 * Tests
 */

void JoinClauseTest::joinClauseItemTest()
{
  using Sql::JoinClauseItem;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId(ADR, address.Client_Id_FK());

  JoinClauseItem item1(JoinOperator::Join, ADR, adrClientId == clientId);
  QVERIFY(item1.joinOperator() == JoinOperator::Join);
  QCOMPARE(item1.tableName(), QString("Address_tbl"));
  QCOMPARE(item1.tableAlias(), QString("ADR"));
  JoinClauseItem item2(JoinOperator::LeftJoin, ADR, adrClientId == clientId);
  QVERIFY(item2.joinOperator() == JoinOperator::LeftJoin);
}

void JoinClauseTest::joinClauseItemSqlTransformTest()
{
  using Sql::JoinClauseItem;
  using Sql::JoinClauseItemSqlTransform;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");
  SelectTable ADR2(address);
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId(ADR, address.Client_Id_FK());
  JoinConstraintField adrClientId2(ADR2, address.Client_Id_FK());
  QString expectedSql;
  auto db = mDatabase;

  // JOIN
  JoinClauseItem item1(JoinOperator::Join, ADR, adrClientId == clientId);
  expectedSql = " JOIN \"Address_tbl\" \"ADR\"\n  ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item1, db) , expectedSql );
  // LEFT JOIN
  JoinClauseItem item2(JoinOperator::LeftJoin, ADR, adrClientId == clientId);
  expectedSql = " LEFT JOIN \"Address_tbl\" \"ADR\"\n  ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item2, db) , expectedSql );
  // JOIN , no alias
  JoinClauseItem item3(JoinOperator::Join, ADR2, adrClientId2 == clientId);
  expectedSql = " JOIN \"Address_tbl\"\n  ON \"Address_tbl\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item3, db) , expectedSql );
}

void JoinClauseTest::autoJoinClauseItemTest()
{
  using Sql::JoinClauseItem;
  using Sql::JoinOperator;
  using Sql::SelectTable;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");

  JoinClauseItem item1(JoinOperator::Join, ADR, CLI);
  QVERIFY(item1.joinOperator() == JoinOperator::Join);
  QCOMPARE(item1.tableName(), QString("Address_tbl"));
  QCOMPARE(item1.tableAlias(), QString("ADR"));
  JoinClauseItem item2(JoinOperator::LeftJoin, ADR, CLI);
  QVERIFY(item2.joinOperator() == JoinOperator::LeftJoin);
}

void JoinClauseTest::autoJoinClauseItemSqlTransformTest()
{
  using Sql::JoinClauseItem;
  using Sql::JoinClauseItemSqlTransform;
  using Sql::JoinOperator;
  using Sql::SelectTable;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");
  QString expectedSql;
  auto db = mDatabase;

  // JOIN
  JoinClauseItem item1(JoinOperator::Join, ADR, CLI);
  expectedSql = " JOIN \"Address_tbl\" \"ADR\"\n  ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item1, db) , expectedSql );
  // LEFT JOIN
  JoinClauseItem item2(JoinOperator::LeftJoin, ADR, CLI);
  expectedSql = " LEFT JOIN \"Address_tbl\" \"ADR\"\n  ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item2, db) , expectedSql );
}

void JoinClauseTest::joinClauseItemListTest()
{
  using Sql::JoinClauseItemList;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR1(address, "ADR1");
  SelectTable ADR2(address, "ADR2");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId(ADR1, address.Client_Id_FK());

  /*
   * Initial state
   */
  JoinClauseItemList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Append with explicit constraint
   */
  list.append(JoinOperator::Join, ADR1, adrClientId == clientId);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).tableAlias(), QString("ADR1"));
  /*
   * Append with auto join
   */
  list.append(JoinOperator::Join, ADR2, CLI);
  QCOMPARE(list.size(), 2);
  QCOMPARE(list.at(1).tableAlias(), QString("ADR2"));
}

void JoinClauseTest::joinClauseItemListSqlTransformTest()
{
  using Sql::JoinClauseItemListSqlTransform;
  using Sql::JoinClauseItemList;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR1(address, "ADR1");
  SelectTable ADR2(address, "ADR2");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId(ADR1, address.Client_Id_FK());
  QString expectedSql;
  auto db = mDatabase;
  JoinClauseItemList list;

  list.append(JoinOperator::Join, ADR1, adrClientId == clientId);
  expectedSql = " JOIN \"Address_tbl\" \"ADR1\"\n" \
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemListSqlTransform::getSql(list, db) , expectedSql );
  list.append(JoinOperator::Join, ADR2, CLI);
  expectedSql = " JOIN \"Address_tbl\" \"ADR1\"\n" \
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"\n" \
                " JOIN \"Address_tbl\" \"ADR2\"\n" \
                "  ON \"ADR2\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemListSqlTransform::getSql(list, db) , expectedSql );
}

void JoinClauseTest::joinClauseTest()
{
  using Sql::JoinClause;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable CLI3(client, "CLI3");
  SelectTable ADR1(address, "ADR1");
  SelectTable ADR2(address, "ADR2");
  SelectTable ADR3(address, "ADR3");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId1(ADR1, address.Client_Id_FK());

  // Only from table
  JoinClause clause(CLI);
  QCOMPARE(clause.tableName(), QString("Client_tbl"));
  QCOMPARE(clause.tableAlias(), QString("CLI"));
  QCOMPARE(clause.itemList().size(), 0);
  QVERIFY(!clause.hasJoinedTable());
  QVERIFY(!clause.isNull());
  // Join table on expression
  clause.joinTableOn(JoinOperator::Join, ADR1, adrClientId1 == clientId);
  QCOMPARE(clause.itemList().size(), 1);
  QVERIFY(clause.hasJoinedTable());
  // Join table automatically
  clause.joinTableOn(JoinOperator::Join, ADR2);
  QCOMPARE(clause.itemList().size(), 2);
  // Join table automatically
  clause.joinTableOn(JoinOperator::Join, ADR3, CLI3);
  QCOMPARE(clause.itemList().size(), 3);
}

void JoinClauseTest::joinClauseSqlTransformTest()
{
  using Sql::JoinClause;
  using Sql::JoinClauseSqlTransform;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable CLI3(client, "CLI3");
  SelectTable ADR1(address, "ADR1");
  SelectTable ADR2(address, "ADR2");
  SelectTable ADR3(address, "ADR3");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId1(ADR1, address.Client_Id_FK());
  QString expectedSql;
  auto db = mDatabase;

  // Only from table
  JoinClause clause(CLI);
  expectedSql = "\"Client_tbl\" \"CLI\"";
  QCOMPARE( JoinClauseSqlTransform::getSql(clause, db) , expectedSql );
  // Join table on expression
  clause.joinTableOn(JoinOperator::Join, ADR1, adrClientId1 == clientId);
  expectedSql = "\"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR1\"\n"\
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseSqlTransform::getSql(clause, db) , expectedSql );
  // Join table automatically
  clause.joinTableOn(JoinOperator::Join, ADR2);
  expectedSql = "\"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR1\"\n"\
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"\n"\
                " JOIN \"Address_tbl\" \"ADR2\"\n"\
                "  ON \"ADR2\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseSqlTransform::getSql(clause, db) , expectedSql );
  // Join table automatically
  clause.joinTableOn(JoinOperator::Join, ADR3, CLI3);
  expectedSql = "\"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR1\"\n"\
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"\n"\
                " JOIN \"Address_tbl\" \"ADR2\"\n"\
                "  ON \"ADR2\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"\n"\
                " JOIN \"Address_tbl\" \"ADR3\"\n"\
                "  ON \"ADR3\".\"Client_Id_FK\"=\"CLI3\".\"Id_PK\"";
  QCOMPARE( JoinClauseSqlTransform::getSql(clause, db) , expectedSql );
  // Also check with a from table without alias
  SelectTable CLI5(client);
  JoinClause clause5(CLI5);
  expectedSql = "\"Client_tbl\"";
  QCOMPARE( JoinClauseSqlTransform::getSql(clause5, db) , expectedSql );
  clause5.joinTableOn(JoinOperator::Join, ADR1);
  expectedSql = "\"Client_tbl\"\n"\
                " JOIN \"Address_tbl\" \"ADR1\"\n"\
                "  ON \"ADR1\".\"Client_Id_FK\"=\"Client_tbl\".\"Id_PK\"";
  QCOMPARE( JoinClauseSqlTransform::getSql(clause5, db) , expectedSql );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  JoinClauseTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
