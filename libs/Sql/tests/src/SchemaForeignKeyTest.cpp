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
#include "SchemaForeignKeyTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"
#include "Mdt/Sql/Schema/ForeignTable.h"
#include "Mdt/Sql/Schema/ForeignField.h"
#include "Mdt/Sql/Schema/ForeignFieldList.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"
#include <QString>
#include <QStringList>

namespace Sql = Mdt::Sql;

void SchemaForeignKeyTest::initTestCase()
{
}

void SchemaForeignKeyTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaForeignKeyTest::settingsTest()
{
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignKeyAction;

  /*
   * Default settings
   */
  ForeignKeySettings s;
  QVERIFY(!s.createIndex());
  QVERIFY(s.onDeleteAction() == ForeignKeyAction::NoAction);
  QVERIFY(s.onUpdateAction() == ForeignKeyAction::NoAction);
  /*
   * Set
   */
  s.setCreateIndex(true);
  s.setOnDeleteAction(ForeignKeyAction::Restrict);
  s.setOnUpdateAction(ForeignKeyAction::Cascade);
  QVERIFY(s.createIndex());
  QVERIFY(s.onDeleteAction() == ForeignKeyAction::Restrict);
  QVERIFY(s.onUpdateAction() == ForeignKeyAction::Cascade);
  /*
   * Clear
   */
  s.clear();
  QVERIFY(!s.createIndex());
  QVERIFY(s.onDeleteAction() == ForeignKeyAction::NoAction);
  QVERIFY(s.onUpdateAction() == ForeignKeyAction::NoAction);
}

QString foreignTableTestFunction(const Sql::Schema::ForeignTable & table)
{
  return table.tableName();
}

void SchemaForeignKeyTest::foreignTableTest()
{
  using Sql::Schema::ForeignTable;
  using Sql::Schema::Table;

  Schema::Client_tbl client;
  Table table;
  table.setTableName("B");

  // Must not compile
//   QCOMPARE( foreignTableTestFunction("Bad") , QString("Bad") );
//   QCOMPARE( foreignTableTestFunction(table) , QString("B") );
//   QCOMPARE( foreignTableTestFunction(client) , QString("Client_tbl") );

  QCOMPARE( foreignTableTestFunction(ForeignTable("A")) , QString("A") );
  QCOMPARE( foreignTableTestFunction(ForeignTable(table)) , QString("B") );
  QCOMPARE( foreignTableTestFunction(ForeignTable(client)) , QString("Client_tbl") );
}

QString foreignFieldTestFunction(const Sql::Schema::ForeignField & field)
{
  return field.fieldName();
}

void SchemaForeignKeyTest::foreignFieldTest()
{
  using Sql::Schema::ForeignField;
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;

  Schema::Client_tbl client;
  Field B;
  B.setName("B");
  B.setType(FieldType::Integer);

  // Must not compile
//   QCOMPARE( foreignFieldTestFunction("A") , QString("A") );
//   QCOMPARE( foreignFieldTestFunction(B) , QString("B") );
//   QCOMPARE( foreignFieldTestFunction(AutoIncrementPrimaryKey("C")) , QString("C") );
//   QCOMPARE( foreignFieldTestFunction(client.Id_PK()) , QString("Id_PK") );
//   QCOMPARE( foreignFieldTestFunction(client.Name()) , QString("Name") );

  QCOMPARE( foreignFieldTestFunction(ForeignField("A")) , QString("A") );
  QCOMPARE( foreignFieldTestFunction(ForeignField(B)) , QString("B") );
  QCOMPARE( foreignFieldTestFunction(ForeignField(AutoIncrementPrimaryKey("C"))) , QString("C") );
  QCOMPARE( foreignFieldTestFunction(ForeignField(client.Id_PK())) , QString("Id_PK") );
  QCOMPARE( foreignFieldTestFunction(ForeignField(client.Name())) , QString("Name") );
}

QStringList foreignFieldListTestFunction(const Sql::Schema::ForeignFieldList & list)
{
  return list.fieldNameList();
}

void SchemaForeignKeyTest::foreignFieldListTest()
{
  using Sql::Schema::ForeignFieldList;
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;

  Schema::Client_tbl client;
  Field field;
  field.setType(FieldType::Integer);
  QStringList list;

  // Must not compile
//   QCOMPARE( foreignFieldListTestFunction("Bad").size() , 1 );
//   QCOMPARE( foreignFieldListTestFunction(client.Id_PK()).size() , 1 );
//   QCOMPARE( foreignFieldListTestFunction(client.Name()).size() , 1 );

  // 1 field
  list = foreignFieldListTestFunction( ForeignFieldList("A") );
  QCOMPARE(list.size(), 1);
  QCOMPARE(list.at(0), QString("A"));
  // 2 fields
  field.setName("B");
  list = foreignFieldListTestFunction( ForeignFieldList(field, "C") );
  QCOMPARE(list.size(), 2);
  QCOMPARE(list.at(0), QString("B"));
  QCOMPARE(list.at(1), QString("C"));
  // 3 fields
  field.setName("E");
  list = foreignFieldListTestFunction( ForeignFieldList(AutoIncrementPrimaryKey("D"), field, "F") );
  QCOMPARE(list.size(), 3);
  QCOMPARE(list.at(0), QString("D"));
  QCOMPARE(list.at(1), QString("E"));
  QCOMPARE(list.at(2), QString("F"));
  // 4 fields
  field.setName("G");
  list = foreignFieldListTestFunction( ForeignFieldList(field, "H", client.Id_PK(), client.Name()) );
  QCOMPARE(list.size(), 4);
  QCOMPARE(list.at(0), QString("G"));
  QCOMPARE(list.at(1), QString("H"));
  QCOMPARE(list.at(2), QString("Id_PK"));
  QCOMPARE(list.at(3), QString("Name"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaForeignKeyTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
