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
#include "SelectFieldTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/SelectField.h"
#include "Mdt/Sql/SelectFieldItem.h"
#include "Mdt/Sql/SelectFieldItemSqlTransform.h"
#include "Mdt/Sql/SelectFieldList.h"
#include "Mdt/Sql/SelectFieldSqlTransform.h"
#include "Mdt/Sql/SelectFieldListSqlTransform.h"
#include "Mdt/Sql/FieldName.h"
#include "Schema/Client.h"

namespace Sql = Mdt::Sql;

void SelectFieldTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
}

void SelectFieldTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SelectFieldTest::selectFieldTest()
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

void SelectFieldTest::selectFieldSqlTransformTest()
{
  using Sql::SelectFieldSqlTransform;
  using Sql::SelectField;
  using Sql::FieldName;

  auto db = mDatabase;
  QString expectedSql;

  // Name and alias
  SelectField SF1("Name1", "NameAlias1");
  expectedSql = "\"Name1\" AS \"NameAlias1\"";
  QCOMPARE( SelectFieldSqlTransform::getSql(SF1, db) , expectedSql );
  // Name only
  SelectField SF2("Name2");
  expectedSql = "\"Name2\"";
  QCOMPARE( SelectFieldSqlTransform::getSql(SF2, db) , expectedSql );
}

void SelectFieldTest::selectFieldItemSqlTransformTest()
{
  using Sql::SelectFieldItemSqlTransform;
  using Sql::SelectFieldItem;
  using Sql::SelectAllField;
  using Sql::SelectFieldRawSql;
  using Sql::SelectTable;

  QString expectedSql;
  auto db = mDatabase;
  Schema::Client client;
  SelectTable Client(client);
  SelectTable CLI(client, "CLI");

  // Table name and field
  expectedSql = "\"Client_tbl\".\"Id_PK\"";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(Client, client.Id_PK()) , db ) , expectedSql );
  // Table alias and field
  expectedSql = "\"CLI\".\"Id_PK\"";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(CLI, client.Id_PK()) , db ) , expectedSql );
  // Table , field and field alias
  expectedSql = "\"CLI\".\"Id_PK\" AS \"Client_Id\"";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(CLI, client.Id_PK(), "Client_Id") , db ) , expectedSql );
  // Field
  expectedSql = "\"Id_PK\"";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(client.Id_PK()) , db ) , expectedSql );
  // Field and field alias
  expectedSql = "\"Id_PK\" AS \"Client_Id\"";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(client.Id_PK(), "Client_Id") , db ) , expectedSql );
  // Select all with table name
  expectedSql = "\"Client_tbl\".*";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(Client, SelectAllField()) , db ) , expectedSql );
  // Select all with table alias
  expectedSql = "\"CLI\".*";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(CLI, SelectAllField()) , db ) , expectedSql );
  // Select all (not table specified)
  expectedSql = "*";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(SelectAllField()) , db ) , expectedSql );
  // Custom SQL
  expectedSql = "CustomField AS CustomAlias";
  QCOMPARE( SelectFieldItemSqlTransform::getSql( SelectFieldItem(SelectFieldRawSql("CustomField AS CustomAlias")) , db ) , expectedSql );
}

void SelectFieldTest::selectFieldListTest()
{
  using Sql::SelectTable;
  using Sql::TableName;
  using Sql::FieldName;

  SelectTable CLI(TableName("Client_tbl"), "CLI");
  /*
   * Initial state
   */
  Sql::SelectFieldList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add fields
   */
  // Table and field
  list.addField(CLI, FieldName("FieldA"));
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.tableNameAt(0), QString("CLI"));
  // Field
  list.addField(FieldName("FieldB"));
  QCOMPARE(list.size(), 2);
  QVERIFY(list.tableNameAt(1).isEmpty());
  // All fields for table
  list.addAllFields(CLI);
  QCOMPARE(list.size(), 3);
  QCOMPARE(list.tableNameAt(2), QString("CLI"));
  // All fields (no table defined)
  list.addAllFields();
  QCOMPARE(list.size(), 4);
  QVERIFY(list.tableNameAt(3).isEmpty());
  // Custom SQL
  list.addRawSqlFieldExpression("CustomFieldExpr");
  QCOMPARE(list.size(), 5);
  QVERIFY(list.tableNameAt(4).isEmpty());
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());

  /*
   * Add 1 element
   */
//   list.append("Client_tbl", SelectField(FieldName("Id_PK")));
//   list.append("Client_tbl", SelectField("Id_PK"));
//   QCOMPARE(list.size(), 1);
//   QVERIFY(!list.isEmpty());
// //   QCOMPARE(list.selectFieldAt(0).fieldName(), QString("Id_PK"));
//   QCOMPARE(list.tableNameAt(0), QString("Client_tbl"));
//   /*
//    * Clear
//    */
//   list.clear();
//   QCOMPARE(list.size(), 0);
//   QVERIFY(list.isEmpty());
}

void SelectFieldTest::selectFieldListSqlTransformTest()
{
  using Sql::SelectFieldListSqlTransform;
  using Sql::FieldName;

  Sql::SelectFieldList list;
  QString expectedSql;
  auto db = mDatabase;

  // 1 field
  list.addField(FieldName("FieldA"));
  expectedSql = " \"FieldA\"";
  QCOMPARE( SelectFieldListSqlTransform::getSql(list, db) , expectedSql );
  // 2 fields
  list.addField(FieldName("FieldB"));
  expectedSql = " \"FieldA\",\n"
                " \"FieldB\"";
  QCOMPARE( SelectFieldListSqlTransform::getSql(list, db) , expectedSql );  
  // 3 fields
  list.addField(FieldName("FieldC"));
  expectedSql = " \"FieldA\",\n"\
                " \"FieldB\",\n"\
                " \"FieldC\"";
  QCOMPARE( SelectFieldListSqlTransform::getSql(list, db) , expectedSql );  
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SelectFieldTest test;

  return QTest::qExec(&test, argc, argv);
}
