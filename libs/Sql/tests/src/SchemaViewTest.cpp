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
#include "SchemaViewTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/View.h"
#include "Mdt/Sql/Schema/ViewSqlTransform.h"

#include "Mdt/Sql/Schema/TableTemplate.h"
#include "Mdt/Sql/Schema/JoinClause.h"
#include "Mdt/Sql/Schema/JoinHelper.h"
#include "Mdt/Sql/Schema/ViewList.h"
#include "Mdt/Sql/Schema/ForeignKey.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"
#include "Schema/ClientAddressView.h"
#include <QSqlDatabase>
#include <QComboBox>
#include <QTableView>
#include <QTreeView>

namespace Sql = Mdt::Sql;

void SchemaViewTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void SchemaViewTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaViewTest::selectOperatorTest()
{
  using Sql::SelectOperator;
  using Sql::Schema::View;

  // Default operator
  View view;
  QVERIFY(view.selectOperator() == SelectOperator::Select);
  // Set
  view.setSelectOperator(SelectOperator::SelectDistinct);
  QVERIFY(view.selectOperator() == SelectOperator::SelectDistinct);
}

void SchemaViewTest::addFieldTest()
{
  using Sql::Schema::View;
  using Sql::SelectTable;
  using Sql::FieldName;
  using Sql::TableName;

  SelectTable CLI(TableName("Client_tbl"), "CLI");
  View view;
  /*
   * Add fields using avaliable overloads
   */
  view.addField(CLI, FieldName("A"), "A_alias");
  view.addField(CLI, FieldName("B"));
  view.addField(FieldName("C"), "C_alias");
  view.addField(FieldName("D"));
  view.addAllFields(CLI);
  view.addAllFields();
  view.addRawSqlFieldExpression("CustomField AS CustomAlias");
  QCOMPARE(view.selectStatement().fieldList().size(), 7);
  QCOMPARE(view.selectStatement().fieldList().tableNameAt(0), QString("CLI"));
  QCOMPARE(view.selectStatement().fieldList().tableNameAt(1), QString("CLI"));
  QVERIFY(view.selectStatement().fieldList().tableNameAt(2).isEmpty());
  QVERIFY(view.selectStatement().fieldList().tableNameAt(3).isEmpty());
  QCOMPARE(view.selectStatement().fieldList().tableNameAt(4), QString("CLI"));
  QVERIFY(view.selectStatement().fieldList().tableNameAt(5).isEmpty());
  QVERIFY(view.selectStatement().fieldList().tableNameAt(6).isEmpty());
}

void SchemaViewTest::isNullTest()
{
  using Sql::Schema::View;
  using Sql::SelectTable;
  using Sql::FieldName;
  using Sql::TableName;

  SelectTable CLI(TableName("Client_tbl"), "CLI");

  View view1;
  QVERIFY(view1.isNull());
  view1.setName("view1");
  QVERIFY(view1.isNull());
  view1.setFromTable(CLI);
  QVERIFY(view1.isNull());
  view1.addAllFields();
  QVERIFY(!view1.isNull());

  View view2;
  QVERIFY(view2.isNull());
  view2.setFromTable(CLI);
  QVERIFY(view2.isNull());
  view2.setName("view2");
  QVERIFY(view2.isNull());
  view2.addAllFields();
  QVERIFY(!view2.isNull());

  View view3;
  QVERIFY(view3.isNull());
  view3.setFromTable(CLI);
  QVERIFY(view3.isNull());
  view3.addAllFields();
  QVERIFY(view3.isNull());
  view3.setName("view3");
  QVERIFY(!view3.isNull());
}

void SchemaViewTest::joinTest()
{
  using Sql::Schema::View;
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

  View view;
  view.addField(CLI, client.Id_PK(), "Client_Id");
  view.addField(CLI, client.Name());
  view.addField(ADR1, address.Street(), "Street1");
  view.setFromTable(CLI);
  view.joinTable(ADR1, adrClientId1 == clientId);
  view.joinTable(ADR2);
  view.joinTable(ADR3, CLI3);
  view.leftJoinTable(ADR4, adrClientId4 == clientId);
  view.leftJoinTable(ADR5);
  view.leftJoinTable(ADR6, CLI6);

  QCOMPARE(view.selectStatement().fieldList().size(), 3);
  QCOMPARE(view.selectStatement().fromClause().table().tableName(), QString("Client_tbl"));
  QCOMPARE(view.selectStatement().fromClause().joinClauseItemList().size(), 6);
  QVERIFY(view.selectStatement().fromClause().joinClauseItemList().at(0).joinOperator() == JoinOperator::Join);
  QVERIFY(view.selectStatement().fromClause().joinClauseItemList().at(1).joinOperator() == JoinOperator::Join);
  QVERIFY(view.selectStatement().fromClause().joinClauseItemList().at(2).joinOperator() == JoinOperator::Join);
  QVERIFY(view.selectStatement().fromClause().joinClauseItemList().at(3).joinOperator() == JoinOperator::LeftJoin);
  QVERIFY(view.selectStatement().fromClause().joinClauseItemList().at(4).joinOperator() == JoinOperator::LeftJoin);
  QVERIFY(view.selectStatement().fromClause().joinClauseItemList().at(5).joinOperator() == JoinOperator::LeftJoin);
}

void SchemaViewTest::dropViewSqlTransformTest()
{
  using Sql::Schema::ViewSqlTransform;
  using Sql::Schema::View;

  auto db = mDatabase;
  QString expectedSql;

  View view1;
  view1.setName("view1");
  expectedSql = "DROP VIEW IF EXISTS \"view1\";";
  QCOMPARE( ViewSqlTransform::getSqlToDropView(view1, db) , expectedSql );
}

void SchemaViewTest::createViewSqlTransformTest()
{
  using Sql::Schema::ViewSqlTransform;
  using Sql::Schema::View;
  using Sql::SelectTable;
  using Sql::TableName;

  Schema::Client_tbl client;
  Schema::Address_tbl address;
  auto db = mDatabase;
  QString expectedSql;
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");

  View view1;
  view1.setName("view1");
  view1.addAllFields();
  view1.setFromTable(CLI);
  expectedSql = "CREATE VIEW \"view1\" AS\n"\
                "SELECT\n"\
                " *\n"\
                "FROM \"Client_tbl\" \"CLI\";";
  QCOMPARE( ViewSqlTransform::getSqlToCreateView(view1, db) , expectedSql );

  View view2;
  view2.setName("view2");
  view2.addAllFields();
  view2.setFromTable(CLI);
  view2.joinTable(ADR);
  expectedSql = "CREATE VIEW \"view2\" AS\n"\
                "SELECT\n"\
                " *\n"\
                "FROM \"Client_tbl\" \"CLI\"\n"\
                " JOIN \"Address_tbl\" \"ADR\"\n"\
                "  ON \"ADR\".\"Client_Id_FK\"=\"CLI\".\"Id_PK\";";
  QCOMPARE( ViewSqlTransform::getSqlToCreateView(view2, db) , expectedSql );
}


void SchemaViewTest::mainTableFieldTest()
{
  using Mdt::Sql::Schema::MainTableField;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;

  /*
   * Setup fields
   */
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  SingleFieldPrimaryKey Code_PK;
  Code_PK.setFieldName("Code_PK");
  Field Name;
  Name.setName("Name");

  /*
   * Check
   */
  QCOMPARE(MainTableField(Id_PK).fieldName(), QString("Id_PK"));
  QCOMPARE(MainTableField(Code_PK).fieldName(), QString("Code_PK"));
  QCOMPARE(MainTableField(Name).fieldName(), QString("Name"));
  QCOMPARE(MainTableField("A").fieldName(), QString("A"));
}

void SchemaViewTest::tableToJoinFieldTest()
{
  using Mdt::Sql::Schema::TableToJoinField;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;

  /*
   * Setup fields
   */
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  SingleFieldPrimaryKey Code_PK;
  Code_PK.setFieldName("Code_PK");
  Field Name;
  Name.setName("Name");

  /*
   * Check
   */
  QCOMPARE(TableToJoinField(Id_PK).fieldName(), QString("Id_PK"));
  QCOMPARE(TableToJoinField(Code_PK).fieldName(), QString("Code_PK"));
  QCOMPARE(TableToJoinField(Name).fieldName(), QString("Name"));
  QCOMPARE(TableToJoinField("A").fieldName(), QString("A"));
}

void SchemaViewTest::joinKeyTest()
{
  using Mdt::Sql::Schema::JoinKey;
  using Mdt::Sql::Schema::JoinOperator;

  /*
   * Initial state
   */
  JoinKey key;
  QVERIFY(key.constraintOperator() == JoinOperator::On);
  QVERIFY(key.fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  QVERIFY(key.isNull());
  /*
   * Set/get
   */
  key.setMainTableFieldName("Id_PK");
  QVERIFY(key.isNull());
  key.setTableToJoinFieldName("Client_Id_FK");
  QVERIFY(!key.isNull());
  key.setConstraintOperator(JoinOperator::And);
  key.setFieldComparisonOperator(JoinOperator::MtfLessThanTdjf);
  // Check
  QVERIFY(!key.isNull());
  QCOMPARE(key.mainTableFieldName(), QString("Id_PK"));
  QCOMPARE(key.tableToJoinFieldName(), QString("Client_Id_FK"));
  QVERIFY(key.constraintOperator() == JoinOperator::And);
  QVERIFY(key.fieldComparisonOperator() == JoinOperator::MtfLessThanTdjf);
  /*
   * Clear
   */
  key.clear();
  QVERIFY(key.mainTableFieldName().isEmpty());
  QVERIFY(key.tableToJoinFieldName().isEmpty());
  QVERIFY(key.constraintOperator() == JoinOperator::On);
  QVERIFY(key.fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  QVERIFY(key.isNull());
}

void SchemaViewTest::joinKeyListTest()
{
  using Mdt::Sql::Schema::JoinKey;
  using Mdt::Sql::Schema::JoinKeyList;
  using Mdt::Sql::Schema::JoinOperator;

  /*
   * Initial state
   */
  JoinKeyList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  JoinKey key;
  key.setMainTableFieldName("A");
  key.setTableToJoinFieldName("B");
  list.append(key);
  // Check
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).mainTableFieldName(), QString("A"));
  QCOMPARE(list.at(0).tableToJoinFieldName(), QString("B"));
  for(const auto & key : list){
    QCOMPARE(key.mainTableFieldName(), QString("A"));
    QCOMPARE(key.tableToJoinFieldName(), QString("B"));
  }
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
}

void SchemaViewTest::joinClauseTest()
{
  using Mdt::Sql::Schema::MainTableField;
  using Mdt::Sql::Schema::TableToJoinField;
  using Mdt::Sql::Schema::JoinClause;
  using Mdt::Sql::Schema::JoinOperator;
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::TableName;

  Schema::Client_tbl client;
  ViewTable CLI(client, "CLI");
  ViewTable ADR(TableName("Address_tbl"), "ADR");

  /*
   * Initial state
   */
  JoinClause join;
  QVERIFY(join.joinOperator() == JoinOperator::Join);
  /*
   * Set - common way
   */
  join.setJoinOperator(JoinOperator::LeftJoin);
  join.setMainTable(CLI);
  join.setTableToJoin(ADR);
  join.addKey(MainTableField(client.Id_PK()), TableToJoinField("Client_Id_FK"));
  // Check
  QVERIFY(join.joinOperator() == JoinOperator::LeftJoin);
  QCOMPARE(join.mainTable().tableName(), QString("Client_tbl"));
  QCOMPARE(join.tableToJoin().tableName(), QString("Address_tbl"));
  QCOMPARE(join.keyList().size(), 1);
  QVERIFY(join.keyList().at(0).constraintOperator() == JoinOperator::On);
  QVERIFY(join.keyList().at(0).fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  QCOMPARE(join.keyList().at(0).mainTableFieldName(), QString("Id_PK"));
  QCOMPARE(join.keyList().at(0).tableToJoinFieldName(), QString("Client_Id_FK"));
  /*
   * Clear
   */
  join.clear();
  QVERIFY(join.joinOperator() == JoinOperator::Join);
  QVERIFY(join.mainTable().isNull());
  QVERIFY(join.tableToJoin().isNull());
  QVERIFY(join.keyList().isEmpty());
  /*
   * Check add key overloads
   */
  // Full explicit version
  join.addKey(JoinOperator::On, MainTableField("A"), JoinOperator::LessThan, TableToJoinField("B"));
  QCOMPARE(join.keyList().size(), 1);
  QVERIFY(join.keyList().at(0).constraintOperator() == JoinOperator::On);
  QVERIFY(join.keyList().at(0).fieldComparisonOperator() == JoinOperator::MtfLessThanTdjf);
  join.clear();
  // 3 args version
  join.addKey(MainTableField("A"), JoinOperator::LessThan, TableToJoinField("B"));
  join.addKey(MainTableField("C"), JoinOperator::GreaterThan, TableToJoinField("D"));
  QCOMPARE(join.keyList().size(), 2);
  QVERIFY(join.keyList().at(0).constraintOperator() == JoinOperator::On);
  QVERIFY(join.keyList().at(0).fieldComparisonOperator() == JoinOperator::MtfLessThanTdjf);
  QVERIFY(join.keyList().at(1).constraintOperator() == JoinOperator::And);
  QVERIFY(join.keyList().at(1).fieldComparisonOperator() == JoinOperator::MtfGreaterThanTdjf);
  join.clear();
  // Simplest version
  join.addKey(MainTableField("A"), TableToJoinField("B"));
  join.addKey(MainTableField("C"), TableToJoinField("D"));
  QCOMPARE(join.keyList().size(), 2);
  QVERIFY(join.keyList().at(0).constraintOperator() == JoinOperator::On);
  QVERIFY(join.keyList().at(0).fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  QVERIFY(join.keyList().at(1).constraintOperator() == JoinOperator::And);
  QVERIFY(join.keyList().at(1).fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  join.clear();
}

void SchemaViewTest::joinClauseListTest()
{
  using Mdt::Sql::Schema::MainTableField;
  using Mdt::Sql::Schema::TableToJoinField;
  using Mdt::Sql::Schema::JoinClause;
  using Mdt::Sql::Schema::JoinClauseList;
  using Mdt::Sql::Schema::JoinOperator;
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::TableName;

  Schema::Client_tbl client;
  ViewTable CLI(client, "CLI");
  ViewTable ADR(TableName("Address_tbl"), "ADR");
  JoinClause join;

  /*
   * Initial state
   */
  JoinClauseList list;
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  join.setMainTable(CLI);
  join.setTableToJoin(ADR);
  list.append(join);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).mainTable().alias(), QString("CLI"));
  for(const auto & _join : list){
    QCOMPARE(_join.mainTable().alias(), QString("CLI"));
  }
  /*
   * Clear
   */
  list.clear();
  QVERIFY(list.isEmpty());
}

void SchemaViewTest::joinHelperJoinFromFkTest()
{
  using Mdt::Sql::Schema::JoinHelper;
  using Mdt::Sql::Schema::JoinClause;
  using Mdt::Sql::Schema::JoinOperator;
  using Mdt::Sql::Schema::ForeignKey;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::ParentTableFieldName;
  using Mdt::Sql::Schema::ChildTableFieldName;
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::Schema::View;

  ForeignKey fk;
  JoinClause join;
  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Connector_Id_FK
  Field Connector_Id_FK;
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(FieldType::Integer);
  Connector_Id_FK.setRequired(true);
  /*
   * Init Connector_tbl
   */
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.setPrimaryKey(Id_PK);
  /*
   * Init Contact_tbl
   */
  Table Contact_tbl;
  Contact_tbl.setTableName("Contact_tbl");
  Contact_tbl.setPrimaryKey(Id_PK);
  Contact_tbl.addField(Connector_Id_FK);
  fk.clear();
  fk.setParentTable(Connector_tbl);
  fk.addKeyFields(ParentTableFieldName(Id_PK), ChildTableFieldName(Connector_Id_FK));
  Contact_tbl.addForeignKey(fk);
  /*
   * Setup View tables
   */
  ViewTable CNR(Connector_tbl, "CNR");
  ViewTable CNX(Contact_tbl, "CNX");
  /*
   * Initi a basic view
   */
  View ConnectorView;
  ConnectorView.setName("ConnectorView");
  ConnectorView.setTable(CNR);

  /*
   * Check with Connector_tbl as main table
   * Form: FROM Connector_tbl CNR
   *        JOIN Contact_tbl CNX
   *         ON CNR.Id_PK = CNX.Connector_Id_FK
   */
  join = JoinHelper::joinClauseFromTables(Connector_tbl, CNR, Contact_tbl, CNX);
  QVERIFY(join.joinOperator() == JoinOperator::Join);
  QCOMPARE(join.mainTable().tableName(), QString("Connector_tbl"));
  QCOMPARE(join.mainTable().alias(), QString("CNR"));
  QCOMPARE(join.tableToJoin().tableName(), QString("Contact_tbl"));
  QCOMPARE(join.tableToJoin().alias(), QString("CNX"));
  QCOMPARE(join.keyList().size(), 1);
  QVERIFY(join.keyList().at(0).constraintOperator() == JoinOperator::On);
  QCOMPARE(join.keyList().at(0).mainTableFieldName(), QString("Id_PK"));
  QVERIFY(join.keyList().at(0).fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  QCOMPARE(join.keyList().at(0).tableToJoinFieldName(), QString("Connector_Id_FK"));
  /*
   * Same as above, but using a view to specifiy main table
   */
  join = JoinHelper::joinClauseFromTables(Connector_tbl, ConnectorView, Contact_tbl, CNX);
  QVERIFY(join.joinOperator() == JoinOperator::Join);
  QCOMPARE(join.mainTable().tableName(), QString("Connector_tbl"));
  QCOMPARE(join.mainTable().alias(), QString("CNR"));
  QCOMPARE(join.tableToJoin().tableName(), QString("Contact_tbl"));
  QCOMPARE(join.tableToJoin().alias(), QString("CNX"));
  QCOMPARE(join.keyList().size(), 1);
  QVERIFY(join.keyList().at(0).constraintOperator() == JoinOperator::On);
  QCOMPARE(join.keyList().at(0).mainTableFieldName(), QString("Id_PK"));
  QVERIFY(join.keyList().at(0).fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  QCOMPARE(join.keyList().at(0).tableToJoinFieldName(), QString("Connector_Id_FK"));
  /*
   * Check specify a LEFT JOIN operator
   */
  join = JoinHelper::joinClauseFromTables(Connector_tbl, CNR, Contact_tbl, CNX, JoinOperator::LeftJoin);
  QVERIFY(join.joinOperator() == JoinOperator::LeftJoin);
  /*
   * Check with Contact_tbl as main table
   * Form: FROM Contact_tbl CNX
   *        JOIN Connector_tbl CNR
   *         ON CNX.Connector_Id_FK = CNR.Id_PK
   */
  join = JoinHelper::joinClauseFromTables(Contact_tbl, CNX, Connector_tbl, CNR);
  QVERIFY(join.joinOperator() == JoinOperator::Join);
  QCOMPARE(join.mainTable().tableName(), QString("Contact_tbl"));
  QCOMPARE(join.tableToJoin().tableName(), QString("Connector_tbl"));
  QCOMPARE(join.keyList().size(), 1);
  QVERIFY(join.keyList().at(0).constraintOperator() == JoinOperator::On);
  QCOMPARE(join.keyList().at(0).mainTableFieldName(), QString("Connector_Id_FK"));
  QVERIFY(join.keyList().at(0).fieldComparisonOperator() == JoinOperator::MtfEqualTdjf);
  QCOMPARE(join.keyList().at(0).tableToJoinFieldName(), QString("Id_PK"));
}

void SchemaViewTest::viewTableTest()
{
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::TableName;

  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Schema::Client_tbl client;

  /*
   * Create based on a Table
   */
  // No alias
  ViewTable CNR1(Connector_tbl);
  QCOMPARE(CNR1.tableName(), QString("Connector_tbl"));
  QVERIFY(CNR1.alias().isEmpty());
  QCOMPARE(CNR1.aliasOrTableName(), QString("Connector_tbl"));
  // With alias
  ViewTable CNR2(Connector_tbl, "CNR");
  QCOMPARE(CNR2.tableName(), QString("Connector_tbl"));
  QCOMPARE(CNR2.alias(), QString("CNR"));
  QCOMPARE(CNR2.aliasOrTableName(), QString("CNR"));
  /*
   * Create based on a TableTemplate
   */
  // No alias
  ViewTable CLI1(client);
  QCOMPARE(CLI1.tableName(), QString("Client_tbl"));
  QVERIFY(CLI1.alias().isEmpty());
  QCOMPARE(CLI1.aliasOrTableName(), QString("Client_tbl"));
  // With alias
  ViewTable CLI2(client, "CLI");
  QCOMPARE(CLI2.tableName(), QString("Client_tbl"));
  QCOMPARE(CLI2.alias(), QString("CLI"));
  QCOMPARE(CLI2.aliasOrTableName(), QString("CLI"));
  /*
   * Create based on a TableName
   */
  // No alias
  ViewTable CNX1(TableName("Connection_tbl"));
  QCOMPARE(CNX1.tableName(), QString("Connection_tbl"));
  QVERIFY(CNX1.alias().isEmpty());
  QCOMPARE(CNX1.aliasOrTableName(), QString("Connection_tbl"));
  // With alias
  ViewTable CNX2(TableName("Connection_tbl"), "CNX");
  QCOMPARE(CNX2.tableName(), QString("Connection_tbl"));
  QCOMPARE(CNX2.alias(), QString("CNX"));
  QCOMPARE(CNX2.aliasOrTableName(), QString("CNX"));
  /*
   * Check mutable usage
   */
  ViewTable vt;
  QVERIFY(vt.isNull());
  vt.setTableName("TableName");
  QVERIFY(!vt.isNull());
  QCOMPARE(vt.tableName(), QString("TableName"));
  QVERIFY(vt.alias().isEmpty());
  QCOMPARE(vt.aliasOrTableName(), QString("TableName"));
  vt.setAlias("Alias");
  QCOMPARE(vt.aliasOrTableName(), QString("Alias"));
  // Clear
  vt.clear();
  QVERIFY(vt.tableName().isEmpty());
  QVERIFY(vt.alias().isEmpty());
  QVERIFY(vt.isNull());
}

void SchemaViewTest::viewTest()
{
  using Mdt::Sql::FieldName;
  using Mdt::Sql::Schema::View;
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::TableName;
  using Mdt::Sql::Schema::JoinClause;
  using Mdt::Sql::Schema::JoinOperator;
  using Mdt::Sql::Schema::MainTableField;
  using Mdt::Sql::Schema::TableToJoinField;

  Schema::Client_tbl client;
  ViewTable ClientTv(client);
  ViewTable CLI(client, "CLI");
  ViewTable ADR(TableName("Address_tbl"), "ADR");
  View view;

  /*
   * Initial state
   */
//   QVERIFY(view.selectOperator() == View::Select);
  QVERIFY(view.isNull());
  /*
   * Simple (single table) view - No alias for table name
   */
  view.clear();
  view.setName("Simple_view");
//   view.setSelectOperator(View::SelectDistinct);
  view.setTable(ClientTv);
  view.addSelectField(ClientTv, client.Id_PK());
  view.addSelectField(ClientTv, client.Name(), "ClientName");
  // Check
  QVERIFY(!view.isNull());
  QCOMPARE(view.name(), QString("Simple_view"));
//   QVERIFY(view.selectOperator() == View::SelectDistinct);
  QCOMPARE(view.tableName(), QString("Client_tbl"));
  QVERIFY(view.tableNameAlias().isEmpty());
  QCOMPARE(view.selectFieldList().size(), 2);
  QCOMPARE(view.selectFieldList().tableNameAt(0), QString("Client_tbl"));
  QCOMPARE(view.selectFieldList().selectFieldAt(0).fieldName(), QString("Id_PK"));
  QCOMPARE(view.selectFieldList().tableNameAt(1), QString("Client_tbl"));
  QCOMPARE(view.selectFieldList().selectFieldAt(1).fieldName(), QString("Name"));
  /*
   * Clear
   */
  view.clear();
  QVERIFY(view.name().isEmpty());
//   QVERIFY(view.selectOperator() == View::Select);
  QVERIFY(view.tableName().isEmpty());
  QVERIFY(view.selectFieldList().isEmpty());
  QVERIFY(view.joinClauseList().isEmpty());
  QVERIFY(view.isNull());
  /*
   * Simple (single table) view - With table name alias
   */
  view.clear();
  view.setName("Simple_view");
//   view.setSelectOperator(View::SelectDistinct);
  view.setTable(CLI);
  view.addSelectField(CLI, client.Id_PK());
  view.addSelectField(CLI, client.Name(), "ClientName");
  // Check
  QVERIFY(!view.isNull());
  QCOMPARE(view.name(), QString("Simple_view"));
//   QVERIFY(view.selectOperator() == View::SelectDistinct);
  QCOMPARE(view.tableName(), QString("Client_tbl"));
  QCOMPARE(view.tableNameAlias(), QString("CLI"));
  QCOMPARE(view.selectFieldList().size(), 2);
  QCOMPARE(view.selectFieldList().tableNameAt(0), QString("CLI"));
  QCOMPARE(view.selectFieldList().selectFieldAt(0).fieldName(), QString("Id_PK"));
  QCOMPARE(view.selectFieldList().tableNameAt(1), QString("CLI"));
  QCOMPARE(view.selectFieldList().selectFieldAt(1).fieldName(), QString("Name"));
  /*
   * Clear
   */
  view.clear();
  QVERIFY(view.name().isEmpty());
//   QVERIFY(view.selectOperator() == View::Select);
  QVERIFY(view.tableName().isEmpty());
  QVERIFY(view.selectFieldList().isEmpty());
  QVERIFY(view.joinClauseList().isEmpty());
  QVERIFY(view.isNull());
  /*
   * View with a join
   */
  view.clear();
  view.setName("CLI_ADR_view");
  view.setTable(CLI);
  view.addSelectField(CLI, client.Id_PK());
  view.addSelectField(CLI, client.Name(), "ClientName");
  view.addSelectAllFields(ADR);
  JoinClause join;
  join.setMainTable(CLI);
  join.setTableToJoin(ADR);
  join.addKey(MainTableField(client.Id_PK()), TableToJoinField("Client_Id_FK"));
  view.addJoinClause(join);
  // Check
  QVERIFY(!view.isNull());
  QCOMPARE(view.name(), QString("CLI_ADR_view"));
//   QVERIFY(view.selectOperator() == View::Select);
  QCOMPARE(view.tableName(), QString("Client_tbl"));
  QCOMPARE(view.selectFieldList().size(), 3);
  QCOMPARE(view.selectFieldList().tableNameAt(0), QString("CLI"));
  QCOMPARE(view.selectFieldList().selectFieldAt(0).fieldName(), QString("Id_PK"));
  QCOMPARE(view.selectFieldList().tableNameAt(1), QString("CLI"));
  QCOMPARE(view.selectFieldList().selectFieldAt(1).fieldName(), QString("Name"));
  QCOMPARE(view.selectFieldList().tableNameAt(2), QString("ADR"));
  QCOMPARE(view.selectFieldList().selectFieldAt(2).fieldName(), QString("*"));
  QCOMPARE(view.joinClauseList().size(), 1);
  QCOMPARE(view.joinClauseList().at(0).mainTable().alias(), QString("CLI"));
  QCOMPARE(view.joinClauseList().at(0).tableToJoin().alias(), QString("ADR"));
  /*
   * Clear
   */
  view.clear();
  QVERIFY(view.name().isEmpty());
//   QVERIFY(view.selectOperator() == View::Select);
  QVERIFY(view.tableName().isEmpty());
  QVERIFY(view.selectFieldList().isEmpty());
  QVERIFY(view.joinClauseList().isEmpty());
  QVERIFY(view.isNull());
}

void SchemaViewTest::viewListTest()
{
  using Mdt::Sql::Schema::View;
  using Mdt::Sql::Schema::ViewList;

  Schema::ClientAdrressView clientAddressView;
  /*
   * Initials state
   */
  ViewList list;
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  list.append(clientAddressView);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).name(), clientAddressView.name());
  for(const auto & view : list){
    QCOMPARE(view.name(), clientAddressView.name());
  }
  /*
   * Clear
   */
  list.clear();
  QVERIFY(list.isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaViewTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
