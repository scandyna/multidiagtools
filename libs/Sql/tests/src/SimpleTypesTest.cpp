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
#include "SimpleTypesTest.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Sql/TableName.h"
#include "Mdt/Sql/SelectField.h"
#include "Mdt/Sql/SelectFieldList.h"
#include "Mdt/Sql/SelectTable.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/ForeignTable.h"

namespace Sql = Mdt::Sql;

void SimpleTypesTest::initTestCase()
{
}

void SimpleTypesTest::cleanupTestCase()
{
}

/*
 * Tests
 */

QString fieldNameTestFunction(const Sql::FieldName & fn)
{
  return fn.toString();
}

QString tableNameTestFunction(const Sql::TableName & tn)
{
  return tn.toString();
}

void SimpleTypesTest::tableNameTest()
{
  using Sql::TableName;

  // Must not compile
//   QCOMPARE(tableNameTestFunction("A"), QString("A"));

  // From explicit table name
  QCOMPARE(tableNameTestFunction(TableName("B")), QString("B"));
  TableName tableA("A");
  QCOMPARE(tableA.toString(), QString("A"));
}

void SimpleTypesTest::selectTableTest()
{
  using Sql::SelectTable;
  using Sql::TableName;
  using Sql::Schema::Table;

  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Schema::Client client;

  // Must not compile
//   SelectTable Bug("Bug");

  /*
   * Create based on a Table
   */
  // No alias
  SelectTable CNR1(Connector_tbl);
  QCOMPARE(CNR1.tableName(), QString("Connector_tbl"));
  QVERIFY(CNR1.alias().isEmpty());
  QCOMPARE(CNR1.aliasOrTableName(), QString("Connector_tbl"));
  // With alias
  SelectTable CNR2(Connector_tbl, "CNR");
  QCOMPARE(CNR2.tableName(), QString("Connector_tbl"));
  QCOMPARE(CNR2.alias(), QString("CNR"));
  QCOMPARE(CNR2.aliasOrTableName(), QString("CNR"));
  /*
   * Create based on a TableTemplate
   */
  // No alias
  SelectTable CLI1(client);
  QCOMPARE(CLI1.tableName(), QString("Client_tbl"));
  QVERIFY(CLI1.alias().isEmpty());
  QCOMPARE(CLI1.aliasOrTableName(), QString("Client_tbl"));
  // With alias
  SelectTable CLI2(client, "CLI");
  QCOMPARE(CLI2.tableName(), QString("Client_tbl"));
  QCOMPARE(CLI2.alias(), QString("CLI"));
  QCOMPARE(CLI2.aliasOrTableName(), QString("CLI"));
  /*
   * Create based on a TableName
   */
  // No alias
  SelectTable CNX1(TableName("Connection_tbl"));
  QCOMPARE(CNX1.tableName(), QString("Connection_tbl"));
  QVERIFY(CNX1.alias().isEmpty());
  QCOMPARE(CNX1.aliasOrTableName(), QString("Connection_tbl"));
  // With alias
  SelectTable CNX2(TableName("Connection_tbl"), "CNX");
  QCOMPARE(CNX2.tableName(), QString("Connection_tbl"));
  QCOMPARE(CNX2.alias(), QString("CNX"));
  QCOMPARE(CNX2.aliasOrTableName(), QString("CNX"));
  /*
   * Check mutable usage
   */
  SelectTable st;
  QVERIFY(st.isNull());
  st.setTableName("TableName");
  QVERIFY(!st.isNull());
  QCOMPARE(st.tableName(), QString("TableName"));
  QVERIFY(st.alias().isEmpty());
  QCOMPARE(st.aliasOrTableName(), QString("TableName"));
  st.setAlias("Alias");
  QCOMPARE(st.aliasOrTableName(), QString("Alias"));
  // Clear
  st.clear();
  QVERIFY(st.tableName().isEmpty());
  QVERIFY(st.alias().isEmpty());
  QVERIFY(st.isNull());
}

void SimpleTypesTest::selectTableForeignKeyTest()
{
  using Sql::SelectTable;
  using Sql::TableName;
  using Sql::Schema::Table;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::ForeignKey;
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;

  ForeignKey fk;
  /*
   * Check with Schema::Table
   */
  // Setup parent table
  Table Parent;
  Parent.setTableName("Parent_tbl");
  Parent.setAutoIncrementPrimaryKey("Id_PK");
  // Setup child table
  Table Child;
  Child.setTableName("Child_tbl");
  Field ParentId;
  ParentId.setType(FieldType::Integer);
  ParentId.setName("Parent_Id_FK");
  Child.setAutoIncrementPrimaryKey("Id_PK");
  Child.addForeignKey(ParentId, ForeignTable(Parent), ForeignField("Id_PK"), ForeignKeySettings());
  // Setup parent select table and check
  SelectTable P(Parent, "P");
  QCOMPARE(P.aliasOrTableName(), QString("P"));
  QVERIFY(!P.hasForeignKey());
  // Setup child select table and check
  SelectTable C(Child, "C");
  QCOMPARE(C.aliasOrTableName(), QString("C"));
  QVERIFY(C.hasForeignKey());
  fk = C.foreignKeyReferencing(P);
  QVERIFY(!fk.isNull());
  QCOMPARE(fk.parentTableName(), P.tableName());
  QCOMPARE(fk.childTableName(), C.tableName());
  QCOMPARE(fk.parentTableFieldNameList().size(), 1);
  QCOMPARE(fk.parentTableFieldNameList().at(0), QString("Id_PK"));
  QCOMPARE(fk.childTableFieldNameList().size(), 1);
  QCOMPARE(fk.childTableFieldNameList().at(0), QString("Parent_Id_FK"));
  /*
   * Check with table template
   */
  // Client
  Schema::Client client;
  SelectTable CLI(client, "CLI");
  QCOMPARE(CLI.aliasOrTableName(), QString("CLI"));
  // Address
  Schema::Address address;
  SelectTable ADR(address, "ADR");
  QCOMPARE(ADR.aliasOrTableName(), QString("ADR"));
  fk = ADR.foreignKeyReferencing(CLI);
  QVERIFY(!fk.isNull());
  QCOMPARE(fk.parentTableName(), CLI.tableName());
  QCOMPARE(fk.childTableName(), ADR.tableName());
  QCOMPARE(fk.parentTableFieldNameList().size(), 1);
  QCOMPARE(fk.parentTableFieldNameList().at(0), client.Id_PK().fieldName());
  QCOMPARE(fk.childTableFieldNameList().size(), 1);
  QCOMPARE(fk.childTableFieldNameList().at(0), address.Client_Id_FK().name());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SimpleTypesTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
