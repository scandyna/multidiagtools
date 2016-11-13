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
#include "Mdt/Sql/Schema/ViewList.h"
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
