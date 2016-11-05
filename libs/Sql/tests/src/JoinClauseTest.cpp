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
#include "Mdt/Sql/JoinClause.h"
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
  expectedSql = "JOIN \"Address_tbl\" \"ADR\"\n ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item1, db) , expectedSql );
  // LEFT JOIN
  JoinClauseItem item2(JoinOperator::LeftJoin, ADR, adrClientId == clientId);
  expectedSql = "LEFT JOIN \"Address_tbl\" \"ADR\"\n ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item2, db) , expectedSql );
  // JOIN , no alias
  JoinClauseItem item3(JoinOperator::Join, ADR2, adrClientId2 == clientId);
  expectedSql = "JOIN \"Address_tbl\"\n ON \"Address_tbl\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
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
  expectedSql = "JOIN \"Address_tbl\" \"ADR\"\n ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item1, db) , expectedSql );
  // LEFT JOIN
  JoinClauseItem item2(JoinOperator::LeftJoin, ADR, CLI);
  expectedSql = "LEFT JOIN \"Address_tbl\" \"ADR\"\n ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( JoinClauseItemSqlTransform::getSql(item2, db) , expectedSql );
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
