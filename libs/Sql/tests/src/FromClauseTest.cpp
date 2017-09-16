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
#include "Schema/Client.h"
#include "Schema/Address.h"
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
  using Sql::JoinClause;

  SelectTable table(TableName("Client_tbl"), "CLI");
  /*
   * Initial state
   */
  FromClause fromClause;
  QVERIFY(fromClause.isNull());
  QVERIFY(fromClause.table().isNull());
  /*
   * Set table
   */
  fromClause.setTable(table);
  QVERIFY(!fromClause.isNull());
  QVERIFY(!fromClause.table().isNull());
  QCOMPARE(fromClause.table().tableName(), QString("Client_tbl"));
}

void FromClauseTest::setJoinClauseTest()
{
  using Sql::FromClause;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;

  Schema::Client client;
  Schema::Address address;
  SelectTable CLI(client, "CLI");
  SelectTable CLI3(client, "CLI3");
  SelectTable ADR1(address, "ADR1");
  SelectTable ADR2(address, "ADR2");
  SelectTable ADR3(address, "ADR3");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId1(ADR1, address.Client_Id_FK());

  /*
   * Initial state
   */
  FromClause fromClause;
  QVERIFY(fromClause.isNull());
  QCOMPARE(fromClause.joinClauseItemList().size(), 0);
  /*
   * Set table
   */
  fromClause.setTable(CLI);
  QVERIFY(!fromClause.isNull());
  QCOMPARE(fromClause.table().tableName() , QString("Client_tbl"));
  QCOMPARE(fromClause.joinClauseItemList().size(), 0);
  /*
   * Join tables
   */
  fromClause.joinTableOn(JoinOperator::Join, ADR1, adrClientId1 == clientId);
  QCOMPARE(fromClause.joinClauseItemList().size(), 1);
  fromClause.joinTableOn(JoinOperator::Join, ADR2);
  QCOMPARE(fromClause.joinClauseItemList().size(), 2);
  fromClause.joinTableOn(JoinOperator::Join, ADR3, CLI3);
  QCOMPARE(fromClause.joinClauseItemList().size(), 3);
}

void FromClauseTest::setSqlStringTest()
{
  using Sql::FromClause;

  /*
   * Initial state
   */
  FromClause fromClause;
  QVERIFY(fromClause.isNull());
  /*
   * Set SQL string
   */
  fromClause.setSqlString("Client_tbl CLI, Address_tbl ADR");
  QVERIFY(!fromClause.isNull());
  QCOMPARE(fromClause.sqlString(), QString("Client_tbl CLI, Address_tbl ADR"));
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
  FromClause fromClause;
  QVERIFY(fromClause.isNull());
  /*
   * Set table
   */
  fromClause.setTable(table);
  QVERIFY(!fromClause.isNull());
  QCOMPARE(fromClause.table().tableName(), QString("A_tbl"));
  /*
   * Set SQL string
   */
  fromClause.setSqlString("A_tbl A, B_tbl B");
  QVERIFY(!fromClause.isNull());
  QCOMPARE(fromClause.sqlString(), QString("A_tbl A, B_tbl B"));
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
  using Sql::FromClause;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;
  using Sql::FromClauseSqlTransform;

  const auto db = mDatabase;
  QString expectedSql;
  Schema::Client client;
  Schema::Address address;
  SelectTable CLI(client, "CLI");
  SelectTable CLI3(client, "CLI3");
  SelectTable ADR1(address, "ADR1");
  SelectTable ADR2(address, "ADR2");
  SelectTable ADR3(address, "ADR3");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId1(ADR1, address.Client_Id_FK());
  FromClause fromClause;

  // Only from table
  fromClause.setTable(CLI);
  expectedSql = "FROM \"Client_tbl\" \"CLI\"";
  QCOMPARE( FromClauseSqlTransform::getSql(fromClause, db) , expectedSql );
  // Join table on expression
  fromClause.joinTableOn(JoinOperator::Join, ADR1, adrClientId1 == clientId);
  expectedSql = "FROM \"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR1\"\n"\
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( FromClauseSqlTransform::getSql(fromClause, db) , expectedSql );
  // Join table automatically
  fromClause.joinTableOn(JoinOperator::Join, ADR2);
  expectedSql = "FROM \"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR1\"\n"\
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"\n"\
                " JOIN \"Address_tbl\" \"ADR2\"\n"\
                "  ON \"ADR2\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( FromClauseSqlTransform::getSql(fromClause, db) , expectedSql );
  // Join table automatically
  fromClause.joinTableOn(JoinOperator::Join, ADR3, CLI3);
  expectedSql = "FROM \"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR1\"\n"\
                "  ON \"ADR1\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"\n"\
                " JOIN \"Address_tbl\" \"ADR2\"\n"\
                "  ON \"ADR2\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"\n"\
                " JOIN \"Address_tbl\" \"ADR3\"\n"\
                "  ON \"ADR3\".\"Client_Id_FK\"=\"CLI3\".\"Id_PK\"";
  QCOMPARE( FromClauseSqlTransform::getSql(fromClause, db) , expectedSql );
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

void FromClauseTest::joinClauseBenchmark()
{
  using Sql::FromClause;
  using Sql::JoinOperator;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;
  using Sql::FromClauseSqlTransform;

  QBENCHMARK{
    const auto db = mDatabase;
    QString expectedSql;
    Schema::Client client;
    Schema::Address address;
    SelectTable CLI(client, "CLI");
    SelectTable CLI3(client, "CLI3");
    SelectTable ADR1(address, "ADR1");
    SelectTable ADR2(address, "ADR2");
    SelectTable ADR3(address, "ADR3");
    JoinConstraintField clientId(CLI, client.Id_PK());
    JoinConstraintField adrClientId1(ADR1, address.Client_Id_FK());
    FromClause fromClause;

    fromClause.setTable(CLI);
    fromClause.joinTableOn(JoinOperator::Join, ADR1, adrClientId1 == clientId);
    fromClause.joinTableOn(JoinOperator::Join, ADR2);
    fromClause.joinTableOn(JoinOperator::Join, ADR3, CLI3);
    QVERIFY( !FromClauseSqlTransform::getSql(fromClause, db).isEmpty() );
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FromClauseTest test;

  return QTest::qExec(&test, argc, argv);
}
