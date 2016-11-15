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
#include "SelectStatementTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/SelectStatement.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Sql/TableName.h"
#include "Mdt/Sql/SelectStatementSqlTransform.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
// #include "Mdt/Sql/Schema/SingleFieldPrimaryKey.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"

namespace Sql = Mdt::Sql;

void SelectStatementTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void SelectStatementTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SelectStatementTest::selectOperatorTest()
{
  using Sql::SelectStatement;
  using Sql::SelectOperator;

  // Default
  Sql::SelectStatement stm;
  QVERIFY(stm.selectOperator() == SelectOperator::Select);
  // Set
  stm.setSelectOperator(SelectOperator::SelectDistinct);
  QVERIFY(stm.selectOperator() == SelectOperator::SelectDistinct);
}

void SelectStatementTest::addFieldTest()
{
  using Sql::SelectStatement;
  using Sql::SelectTable;
  using Sql::FieldName;
  using Sql::TableName;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::AutoIncrementPrimaryKey;
//   using Sql::Schema::SingleFieldPrimaryKey;

  SelectTable CLI(TableName("Client_tbl"), "CLI");
  Field field;
  field.setType(FieldType::Integer);
//   SingleFieldPrimaryKey sfpk;
//   sfpk.setFieldType(FieldType::Integer);
  SelectStatement stm;

  /*
   * Add fields using avaliable overloads
   */
  stm.addField(CLI, FieldName("A"), "A_alias");
  stm.addField(CLI, FieldName("B"));
  stm.addField(FieldName("C"), "C_alias");
  stm.addField(FieldName("D"));
  stm.addAllFields(CLI);
  stm.addAllFields();
  stm.addRawSqlFieldExpression("CustomField AS CustomAlias");
  QCOMPARE(stm.fieldList().size(), 7);
  QCOMPARE(stm.fieldList().tableNameAt(0), QString("CLI"));
  QCOMPARE(stm.fieldList().tableNameAt(1), QString("CLI"));
  QVERIFY(stm.fieldList().tableNameAt(2).isEmpty());
  QVERIFY(stm.fieldList().tableNameAt(3).isEmpty());
  QCOMPARE(stm.fieldList().tableNameAt(4), QString("CLI"));
  QVERIFY(stm.fieldList().tableNameAt(5).isEmpty());
  QVERIFY(stm.fieldList().tableNameAt(6).isEmpty());

//   field.setName("D");
//   stm.addField(CLI, field, "D_alias");
//   field.setName("E");
//   stm.addField(CLI, field);
//   stm.addField(CLI, AutoIncrementPrimaryKey("F"), "F_alias");
//   stm.addField(CLI, AutoIncrementPrimaryKey("G"));
//   sfpk.setFieldName("H");
//   stm.addField(CLI, sfpk, "H_alias");
//   sfpk.setFieldName("I");
//   stm.addField(CLI, sfpk);
//   /*
//    * Check
//    */
//   auto list = stm.fieldList();
//   QCOMPARE(list.size(), 9);
//   QCOMPARE(list.tableNameAt(0), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(0).fieldName(), QString("A"));
//   QCOMPARE(list.selectFieldAt(0).alias(), QString("A_alias"));
//   QCOMPARE(list.tableNameAt(1), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(1).fieldName(), QString("B"));
//   QCOMPARE(list.selectFieldAt(1).alias(), QString("B_alias"));
//   QCOMPARE(list.tableNameAt(2), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(2).fieldName(), QString("C"));
//   QVERIFY(list.selectFieldAt(2).alias().isEmpty());
//   QCOMPARE(list.tableNameAt(3), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(3).fieldName(), QString("D"));
//   QCOMPARE(list.selectFieldAt(3).alias(), QString("D_alias"));
//   QCOMPARE(list.tableNameAt(4), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(4).fieldName(), QString("E"));
//   QVERIFY(list.selectFieldAt(4).alias().isEmpty());
//   QCOMPARE(list.tableNameAt(5), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(5).fieldName(), QString("F"));
//   QCOMPARE(list.selectFieldAt(5).alias(), QString("F_alias"));
//   QCOMPARE(list.tableNameAt(6), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(6).fieldName(), QString("G"));
//   QVERIFY(list.selectFieldAt(6).alias().isEmpty());
//   QCOMPARE(list.tableNameAt(7), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(7).fieldName(), QString("H"));
//   QCOMPARE(list.selectFieldAt(7).alias(), QString("H_alias"));
//   QCOMPARE(list.tableNameAt(8), QString("CLI"));
//   QCOMPARE(list.selectFieldAt(8).fieldName(), QString("I"));
//   QVERIFY(list.selectFieldAt(8).alias().isEmpty());
}

void SelectStatementTest::isNullTest()
{
  using Sql::SelectStatement;
  using Sql::SelectTable;
  using Sql::FieldName;
  using Sql::TableName;

  SelectTable CLI(TableName("Client_tbl"), "CLI");

  SelectStatement stm1;
  QVERIFY(stm1.isNull());
  stm1.addAllFields();
  QVERIFY(stm1.isNull());
  stm1.setFromTable(CLI);
  QVERIFY(!stm1.isNull());

  SelectStatement stm2;
  QVERIFY(stm2.isNull());
  stm2.setFromTable(CLI);
  QVERIFY(stm2.isNull());
  stm2.addAllFields();
  QVERIFY(!stm2.isNull());
}

void SelectStatementTest::simpleSelectTest()
{
  using Sql::SelectStatement;
  using Sql::SelectField;
  using Sql::SelectTable;
  using Sql::FieldName;
  using Sql::TableName;

  SelectTable CLI(TableName("Client_tbl"), "CLI");
  SelectStatement stm;

  stm.addField(CLI, FieldName("Id_PK"), "Client_Id");
  stm.addField(CLI, FieldName("Name"), "ClientName");
  stm.addField(CLI, FieldName("Remarks"));
  stm.setTable(CLI);
  QCOMPARE(stm.fieldList().size(), 3);
  QCOMPARE(stm.fromClause().table().tableName(), QString("Client_tbl"));
}

void SelectStatementTest::simpleSelectWithSchemaTest()
{
  using Sql::SelectStatement;
  using Sql::SelectField;
  using Sql::SelectTable;

  Schema::Client_tbl client;
  SelectTable CLI(client, "CLI");
  SelectStatement stm;

  stm.addField(CLI, client.Id_PK(), "Client_Id");
  stm.addField(CLI, client.Name());
  stm.setTable(CLI);
  QCOMPARE(stm.fieldList().size(), 2);
  QCOMPARE(stm.fromClause().table().tableName(), QString("Client_tbl"));
}

void SelectStatementTest::selectJoinTest()
{
  using Sql::SelectStatement;
  using Sql::SelectTable;
  using Sql::JoinConstraintField;
  using Sql::JoinOperator;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable CLI3(client, "CLI3");
  SelectTable CLI6(client, "CLI6");
  SelectTable ADR1(address, "ADR1");
  SelectTable ADR2(address, "ADR2");
  SelectTable ADR3(address, "ADR3");
  SelectTable ADR4(address, "ADR4");
  SelectTable ADR5(address, "ADR5");
  SelectTable ADR6(address, "ADR6");
  JoinConstraintField clientId(CLI, client.Id_PK());
  JoinConstraintField adrClientId1(ADR1, address.Client_Id_FK());
  JoinConstraintField adrClientId4(ADR4, address.Client_Id_FK());

  SelectStatement stm;
  stm.addField(CLI, client.Id_PK(), "Client_Id");
  stm.addField(CLI, client.Name());
  stm.addField(ADR1, address.Street(), "Street1");
  stm.setFromTable(CLI);
  stm.joinTable(ADR1, adrClientId1 == clientId);
  stm.joinTable(ADR2);
  stm.joinTable(ADR3, CLI3);
  stm.leftJoinTable(ADR4, adrClientId4 == clientId);
  stm.leftJoinTable(ADR5);
  stm.leftJoinTable(ADR6, CLI6);

  QCOMPARE(stm.fieldList().size(), 3);
  QCOMPARE(stm.fromClause().table().tableName(), QString("Client_tbl"));
  QCOMPARE(stm.fromClause().joinClauseItemList().size(), 6);
  QVERIFY(stm.fromClause().joinClauseItemList().at(0).joinOperator() == JoinOperator::Join);
  QVERIFY(stm.fromClause().joinClauseItemList().at(1).joinOperator() == JoinOperator::Join);
  QVERIFY(stm.fromClause().joinClauseItemList().at(2).joinOperator() == JoinOperator::Join);
  QVERIFY(stm.fromClause().joinClauseItemList().at(3).joinOperator() == JoinOperator::LeftJoin);
  QVERIFY(stm.fromClause().joinClauseItemList().at(4).joinOperator() == JoinOperator::LeftJoin);
  QVERIFY(stm.fromClause().joinClauseItemList().at(5).joinOperator() == JoinOperator::LeftJoin);
}

void SelectStatementTest::selectFromSqlTransformTest()
{
  using Sql::SelectStatementSqlTransform;
  using Sql::SelectStatement;
  using Sql::SelectOperator;
  using Sql::SelectTable;
  using Sql::FieldName;

  Schema::Client_tbl client;
  SelectTable CLI(client, "CLI");
  auto db = mDatabase;
  QString expectedSql;

  // Check with some simple statement
  SelectStatement stm1;
  stm1.addField(CLI, client.Id_PK(), "Client_Id");
  stm1.addField(CLI, client.Name());
  stm1.addField(FieldName("Remarks"));
  stm1.setFromTable(CLI);
  expectedSql = "SELECT\n"\
                " \"CLI\".\"Id_PK\" AS \"Client_Id\",\n"\
                " \"CLI\".\"Name\",\n"\
                " \"Remarks\"\n"\
                "FROM \"Client_tbl\" \"CLI\"";
  QCOMPARE( SelectStatementSqlTransform::getSql(stm1, db) , expectedSql );
  // Check with SELECT DISTINCT and *
  SelectStatement stm2;
  stm2.setSelectOperator(SelectOperator::SelectDistinct);
  stm2.addAllFields();
  stm2.setFromTable(CLI);
  expectedSql = "SELECT DISTINCT\n"\
                " *\n"\
                "FROM \"Client_tbl\" \"CLI\"";
  QCOMPARE( SelectStatementSqlTransform::getSql(stm2, db) , expectedSql );
}

void SelectStatementTest::selectFromJoinSqlTransformTest()
{
  using Sql::SelectStatementSqlTransform;
  using Sql::SelectStatement;
  using Sql::SelectTable;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");
  auto db = mDatabase;
  QString expectedSql;

  SelectStatement stm1;
  stm1.addField(CLI, client.Name());
  stm1.addAllFields(ADR);
  stm1.setFromTable(CLI);
  stm1.joinTable(ADR);
  expectedSql = "SELECT\n"\
                " \"CLI\".\"Name\",\n"\
                " \"ADR\".*\n"\
                "FROM \"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR\"\n"\
                "  ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\"";
  QCOMPARE( SelectStatementSqlTransform::getSql(stm1, db) , expectedSql );
}

void SelectStatementTest::selectFromWhereSqlTransformTest()
{
  using Sql::SelectStatementSqlTransform;
  using Sql::SelectStatement;
  using Sql::SelectTable;
  using Sql::WhereField;

  Schema::Client_tbl client;
  SelectTable CLI(client, "CLI");
  WhereField clientName(CLI, client.Name());
  auto db = mDatabase;
  QString expectedSql;

  SelectStatement stm1;
  stm1.addAllFields();
  stm1.setFromTable(CLI);
  stm1.setWhereExpression(clientName != "Name 1");
  expectedSql = "SELECT\n"\
                " *\n"\
                "FROM \"Client_tbl\" \"CLI\"\n"\
                "WHERE \"CLI\".\"Name\"<>'Name 1'";
  QCOMPARE( SelectStatementSqlTransform::getSql(stm1, db) , expectedSql );
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SelectStatementTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
