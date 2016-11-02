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
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Sql/TableName.h"
#include "Mdt/Sql/SelectField.h"
#include "Mdt/Sql/SelectFieldList.h"
#include "Mdt/Sql/SelectTable.h"
#include "Mdt/Sql/Schema/Table.h"

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

void SimpleTypesTest::fieldNameTest()
{
  using Sql::FieldName;

  FieldName fieldA("A");
  QCOMPARE(fieldA.toString(), QString("A"));
}

void SimpleTypesTest::tableNameTest()
{
  using Sql::TableName;

  TableName tableA("A");
  QCOMPARE(tableA.toString(), QString("A"));
}

void SimpleTypesTest::selectFieldTest()
{
  using Sql::SelectField;
  using Sql::FieldName;

  SelectField SF1("Name");
  QCOMPARE(SF1.fieldName(), QString("Name"));
  QVERIFY(SF1.alias().isEmpty());
  SelectField SF2("Name", "AliasName");
  QCOMPARE(SF2.fieldName(), QString("Name"));
  QCOMPARE(SF2.alias(), QString("AliasName"));

}

void SimpleTypesTest::selectFieldListTest()
{
  using Sql::SelectField;
  using Sql::FieldName;

  /*
   * Initial state
   */
  Sql::SelectFieldList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
//   list.append("Client_tbl", SelectField(FieldName("Id_PK")));
  list.append("Client_tbl", SelectField("Id_PK"));
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.selectFieldAt(0).fieldName(), QString("Id_PK"));
  QCOMPARE(list.tableNameAt(0), QString("Client_tbl"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
}

void SimpleTypesTest::selectTableTest()
{
  using Sql::SelectTable;
  using Sql::TableName;
  using Sql::Schema::Table;

  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Schema::Client_tbl client;

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
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::Field;
  using Sql::Schema::ForeignKey;
  using Sql::Schema::ParentTableFieldName;
  using Sql::Schema::ChildTableFieldName;

  ForeignKey fk;
  /*
   * Check with Schema::Table
   */
  // Setup common fields
  AutoIncrementPrimaryKey Id_PK("Id_PK");
  // Setup parent table
  Table Parent;
  Parent.setTableName("Parent_tbl");
  Parent.setPrimaryKey(Id_PK);
  // Setup child table
  Table Child;
  Child.setTableName("Child_tbl");
  Field ParentId;
  ParentId.setName("Parent_Id_FK");
  Child.setPrimaryKey(Id_PK);
  Child.addField(ParentId);
  ForeignKey fkParentId;
  fkParentId.setParentTable(Parent);
  fkParentId.addKeyFields( ParentTableFieldName(Id_PK), ChildTableFieldName(ParentId) );
  Child.addForeignKey(fkParentId);
  // Setup parent select table and check
  SelectTable P(Parent, "P");
  QCOMPARE(P.aliasOrTableName(), QString("P"));

  // Setup child select table and check
  SelectTable C(Child, "C");
  QCOMPARE(C.aliasOrTableName(), QString("C"));
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
  Schema::Client_tbl client;
  SelectTable CLI(client, "CLI");
  QCOMPARE(CLI.aliasOrTableName(), QString("CLI"));
  
  // Address
  Schema::Address_tbl address;
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
