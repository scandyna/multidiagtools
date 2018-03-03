/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "Mdt/CoreApplication.h"
#include "Mdt/Sql/Schema/ForeignKeyActionSqlTransform.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"
#include "Mdt/Sql/Schema/ForeignTable.h"
#include "Mdt/Sql/Schema/ForeignField.h"
#include "Mdt/Sql/Schema/ForeignFieldList.h"
#include "Mdt/Sql/Schema/ForeignKey.h"
#include "Mdt/Sql/Schema/ForeignKeyList.h"
#include "Mdt/Sql/Schema/Index.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
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


void SchemaForeignKeyTest::actionSqlTransformTest()
{
  using Sql::Schema::ForeignKeyAction;
  using Sql::Schema::ForeignKeyActionSqlTransform;

  /*
   * Action -> string
   */
  QCOMPARE(ForeignKeyActionSqlTransform::getSql(ForeignKeyAction::NoAction) , QString("NO ACTION"));
  QCOMPARE(ForeignKeyActionSqlTransform::getSql(ForeignKeyAction::Restrict) , QString("RESTRICT"));
  QCOMPARE(ForeignKeyActionSqlTransform::getSql(ForeignKeyAction::SetNull) , QString("SET NULL"));
  QCOMPARE(ForeignKeyActionSqlTransform::getSql(ForeignKeyAction::SetDefault) , QString("SET DEFAULT"));
  QCOMPARE(ForeignKeyActionSqlTransform::getSql(ForeignKeyAction::Cascade) , QString("CASCADE"));
  /*
   * String -> Action
   */
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("") == ForeignKeyAction::NoAction);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("NO ACTION") == ForeignKeyAction::NoAction);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("NO  ACTION") == ForeignKeyAction::NoAction);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("no action") == ForeignKeyAction::NoAction);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("No Action") == ForeignKeyAction::NoAction);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("RESTRICT") == ForeignKeyAction::Restrict);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("restrict") == ForeignKeyAction::Restrict);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("Restrict") == ForeignKeyAction::Restrict);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("SET NULL") == ForeignKeyAction::SetNull);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("SET  NULL") == ForeignKeyAction::SetNull);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("set null") == ForeignKeyAction::SetNull);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("Set Null") == ForeignKeyAction::SetNull);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("SET DEFAULT") == ForeignKeyAction::SetDefault);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("SET  DEFAULT") == ForeignKeyAction::SetDefault);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("set default") == ForeignKeyAction::SetDefault);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("Set Default") == ForeignKeyAction::SetDefault);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("CASCADE") == ForeignKeyAction::Cascade);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("cascade") == ForeignKeyAction::Cascade);
  QVERIFY(ForeignKeyActionSqlTransform::fromActionString("Cascade") == ForeignKeyAction::Cascade);
}

void SchemaForeignKeyTest::settingsTest()
{
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignKeyAction;

  /*
   * Default settings
   */
  ForeignKeySettings s;
  QVERIFY(!s.isIndexed());
  QVERIFY(s.onDeleteAction() == ForeignKeyAction::NoAction);
  QVERIFY(s.onUpdateAction() == ForeignKeyAction::NoAction);
  /*
   * Set
   */
  s.setIndexed(true);
  s.setOnDeleteAction(ForeignKeyAction::Restrict);
  s.setOnUpdateAction(ForeignKeyAction::Cascade);
  QVERIFY(s.isIndexed());
  QVERIFY(s.onDeleteAction() == ForeignKeyAction::Restrict);
  QVERIFY(s.onUpdateAction() == ForeignKeyAction::Cascade);
  /*
   * Clear
   */
  s.clear();
  QVERIFY(!s.isIndexed());
  QVERIFY(s.onDeleteAction() == ForeignKeyAction::NoAction);
  QVERIFY(s.onUpdateAction() == ForeignKeyAction::NoAction);
}

void SchemaForeignKeyTest::foreignKeySettingsTest()
{
  using Sql::Schema::ForeignKeyAction;
  using Sql::Schema::ForeignKey;
  using Sql::Schema::ForeignKeySettings;

  /*
   * Default settings
   */
  ForeignKey fk;
  QVERIFY(fk.onDeleteAction() == ForeignKeyAction::NoAction);
  QVERIFY(fk.onUpdateAction() == ForeignKeyAction::NoAction);
  QVERIFY(!fk.isIndexed());
  /*
   * Set
   */
  fk.setOnDeleteAction(ForeignKeyAction::Restrict);
  fk.setOnUpdateAction(ForeignKeyAction::Cascade);
  fk.setIndexed(true);
  QVERIFY(fk.onDeleteAction() == ForeignKeyAction::Restrict);
  QVERIFY(fk.onUpdateAction() == ForeignKeyAction::Cascade);
  QVERIFY(fk.isIndexed());
  /*
   * Clear
   */
  fk.clear();
  QVERIFY(fk.onDeleteAction() == ForeignKeyAction::NoAction);
  QVERIFY(fk.onUpdateAction() == ForeignKeyAction::NoAction);
  /*
   * Set settings
   */
  ForeignKeySettings settings;
  settings.setOnDeleteAction(ForeignKeyAction::SetDefault);
  settings.setOnUpdateAction(ForeignKeyAction::SetNull);
  settings.setIndexed(true);
  fk.setSettings(settings);
  QVERIFY(fk.onDeleteAction() == ForeignKeyAction::SetDefault);
  QVERIFY(fk.onUpdateAction() == ForeignKeyAction::SetNull);
  QVERIFY(fk.isIndexed());
}

void SchemaForeignKeyTest::foreignKeyTableNameTest()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::Table;

  Schema::Client client;
  Schema::Address address;
  ForeignKey fk;
  /*
   * Set table and foreign table
   */
  // Use string setters
  fk.setTableName("A");
  fk.setForeignTableName("FA");
  QCOMPARE(fk.tableName(), QString("A"));
  QCOMPARE(fk.childTableName(), QString("A"));
  QCOMPARE(fk.foreignTableName(), QString("FA"));
  QCOMPARE(fk.parentTableName(), QString("FA"));
  // Use Table setters
  Table B;
  B.setTableName("B");
  Table FB;
  FB.setTableName("FB");
  fk.setTable(B);
  fk.setForeignTable(FB);
  QCOMPARE(fk.tableName(), QString("B"));
  QCOMPARE(fk.childTableName(), QString("B"));
  QCOMPARE(fk.foreignTableName(), QString("FB"));
  QCOMPARE(fk.parentTableName(), QString("FB"));
  // Use TableTemplate setters
  fk.setTable(address);
  fk.setForeignTable(client);
  QCOMPARE(fk.tableName(), QString("Address_tbl"));
  QCOMPARE(fk.childTableName(), QString("Address_tbl"));
  QCOMPARE(fk.foreignTableName(), QString("Client_tbl"));
  QCOMPARE(fk.parentTableName(), QString("Client_tbl"));
  /*
   * Clear
   */
  fk.clear();
  QVERIFY(fk.tableName().isEmpty());
  QVERIFY(fk.childTableName().isEmpty());
  QVERIFY(fk.foreignTableName().isEmpty());
  QVERIFY(fk.parentTableName().isEmpty());
  /*
   * Set child table and parent table
   */
  // Use string setters
  fk.setChildTableName("CE");
  fk.setParentTableName("PE");
  QCOMPARE(fk.tableName(), QString("CE"));
  QCOMPARE(fk.childTableName(), QString("CE"));
  QCOMPARE(fk.foreignTableName(), QString("PE"));
  QCOMPARE(fk.parentTableName(), QString("PE"));
  // Use Table setters
  Table CF;
  CF.setTableName("CF");
  Table PF;
  PF.setTableName("PF");
  fk.setChildTable(CF);
  fk.setParentTable(PF);
  QCOMPARE(fk.tableName(), QString("CF"));
  QCOMPARE(fk.childTableName(), QString("CF"));
  QCOMPARE(fk.foreignTableName(), QString("PF"));
  QCOMPARE(fk.parentTableName(), QString("PF"));
  // Use TableTemplate setters
  fk.setChildTable(address);
  fk.setParentTable(client);
  QCOMPARE(fk.tableName(), QString("Address_tbl"));
  QCOMPARE(fk.childTableName(), QString("Address_tbl"));
  QCOMPARE(fk.foreignTableName(), QString("Client_tbl"));
  QCOMPARE(fk.parentTableName(), QString("Client_tbl"));
}

QString foreignTableTestFunction(const Sql::Schema::ForeignTable & table)
{
  return table.tableName();
}

void SchemaForeignKeyTest::foreignTableTest()
{
  using Sql::Schema::ForeignTable;
  using Sql::Schema::Table;

  Schema::Client client;
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

  Schema::Client client;
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

  Schema::Client client;
  Field field;
  field.setType(FieldType::Integer);
  QStringList list;

  // Must not compile
//   QCOMPARE( foreignFieldListTestFunction("Bad").size() , 1 );
//   QCOMPARE( foreignFieldListTestFunction(client.Id_PK()).size() , 1 );
//   QCOMPARE( foreignFieldListTestFunction(client.Name()).size() , 1 );

  // Check attributes
  QCOMPARE( ForeignFieldList("A").size(), 1 );
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

void SchemaForeignKeyTest::foreignKeySetFieldsTest()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldList;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignFieldList;

  Schema::Client client;
  Schema::Address address;
  ForeignKey fk;
  /*
   * Set 1 field relation
   */
  fk.setFields(address.Client_Id_FK(), ForeignField(client.Id_PK()));
  QCOMPARE( fk.fieldNameList(), QStringList({"Client_Id_FK"}) );
  QCOMPARE( fk.childTableFieldNameList(), QStringList({"Client_Id_FK"}) );
  QCOMPARE( fk.foreignTableFieldNameList(), QStringList({"Id_PK"}) );
  QCOMPARE( fk.parentTableFieldNameList(), QStringList({"Id_PK"}) );
  // Check also setting fields again
  fk.setFields(address.Client_Id_FK(), ForeignField(client.Id_PK()));
  QCOMPARE( fk.fieldNameList(), QStringList({"Client_Id_FK"}) );
  QCOMPARE( fk.childTableFieldNameList(), QStringList({"Client_Id_FK"}) );
  QCOMPARE( fk.foreignTableFieldNameList(), QStringList({"Id_PK"}) );
  QCOMPARE( fk.parentTableFieldNameList(), QStringList({"Id_PK"}) );
  /*
   * Clear
   */
  fk.clear();
  QVERIFY(fk.fieldNameList().isEmpty());
  QVERIFY(fk.childTableFieldNameList().isEmpty());
  QVERIFY(fk.foreignTableFieldNameList().isEmpty());
  QVERIFY(fk.parentTableFieldNameList().isEmpty());
  /*
   * Set 2 fields relation
   */
  // Setup fields
  Field Id_A;
  Id_A.setType(FieldType::Integer);
  Id_A.setName("Id_A");
  Field Id_A_FK;
  Id_A_FK.setType(FieldType::Integer);
  Id_A_FK.setName("Id_A_FK");
  Field Id_B;
  Id_B.setType(FieldType::Integer);
  Id_B.setName("Id_B");
  Field Id_B_FK;
  Id_B_FK.setType(FieldType::Integer);
  Id_B_FK.setName("Id_B_FK");
  // Check
  fk.setFields( FieldList(Id_A_FK, Id_B_FK) , ForeignFieldList(Id_A, Id_B) );
  QCOMPARE( fk.fieldNameList(), QStringList({"Id_A_FK", "Id_B_FK"}) );
  QCOMPARE( fk.childTableFieldNameList(), QStringList({"Id_A_FK", "Id_B_FK"}) );
  QCOMPARE( fk.foreignTableFieldNameList(), QStringList({"Id_A", "Id_B"}) );
  QCOMPARE( fk.parentTableFieldNameList(), QStringList({"Id_A", "Id_B"}) );
  // Check also setting fields again
  fk.setFields( FieldList(Id_A_FK, Id_B_FK) , ForeignFieldList(Id_A, Id_B) );
  QCOMPARE( fk.fieldNameList(), QStringList({"Id_A_FK", "Id_B_FK"}) );
  QCOMPARE( fk.childTableFieldNameList(), QStringList({"Id_A_FK", "Id_B_FK"}) );
  QCOMPARE( fk.foreignTableFieldNameList(), QStringList({"Id_A", "Id_B"}) );
  QCOMPARE( fk.parentTableFieldNameList(), QStringList({"Id_A", "Id_B"}) );
  /*
   * Clear
   */
  fk.clear();
  QVERIFY( fk.fieldNameList().isEmpty() );
  QVERIFY( fk.foreignTableFieldNameList().isEmpty() );
  /*
   * Check adding couple of fields
   */
  fk.addFieldNames("A", ForeignField("FA"));
  QCOMPARE( fk.fieldNameList(), QStringList({"A"}) );
  QCOMPARE( fk.childTableFieldNameList(), QStringList({"A"}) );
  QCOMPARE( fk.foreignTableFieldNameList(), QStringList({"FA"}) );
  QCOMPARE( fk.parentTableFieldNameList(), QStringList({"FA"}) );
  fk.addFieldNames("B", ForeignField("FB"));
  QCOMPARE( fk.fieldNameList(), QStringList({"A","B"}) );
  QCOMPARE( fk.childTableFieldNameList(), QStringList({"A","B"}) );
  QCOMPARE( fk.foreignTableFieldNameList(), QStringList({"FA","FB"}) );
  QCOMPARE( fk.parentTableFieldNameList(), QStringList({"FA","FB"}) );
}

void SchemaForeignKeyTest::foreignKeyIsNullTest()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::FieldList;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignFieldList;

  Schema::Client client;
  Schema::Address address;
  /*
   * Initial state
   */
  ForeignKey fk;
  QVERIFY(fk.isNull());
  /*
   * Set tables
   */
  fk.setTable(address);
  QVERIFY(fk.isNull());
  fk.setForeignTable(client);
  QVERIFY(fk.isNull());
  /*
   * Set fields
   */
  fk.setFields(address.Client_Id_FK(), ForeignField(client.Id_PK()));
  QVERIFY(!fk.isNull());
  /*
   * Clear
   */
  fk.clear();
  QVERIFY(fk.isNull());
}

void SchemaForeignKeyTest::foreignKeyGetIndexTest()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::Index;
  using Sql::Schema::FieldList;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignFieldList;

  Schema::Client client;
  Schema::Address address;
  ForeignKey fk;
  Index index;

  fk.setTable(address);
  fk.setForeignTable(client);
  fk.setFields(address.Client_Id_FK(), ForeignField(client.Id_PK()));
  // Check
  index = fk.getIndex();
  QCOMPARE(index.tableName(), QString("Address_tbl"));
  QCOMPARE(index.fieldCount(), 1);
  QCOMPARE(index.fieldName(0), QString("Client_Id_FK"));
  QCOMPARE(index.name(), QString("Address_tbl_Client_Id_FK_index"));
  index = fk.getChildTableIndex();
  QCOMPARE(index.tableName(), QString("Address_tbl"));
  QCOMPARE(index.fieldCount(), 1);
  QCOMPARE(index.fieldName(0), QString("Client_Id_FK"));
  QCOMPARE(index.name(), QString("Address_tbl_Client_Id_FK_index"));
}

void SchemaForeignKeyTest::foreignKeyListAddTest()
{
  using Sql::Schema::ForeignKeyList;
  using Sql::Schema::ForeignKey;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldList;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignFieldList;
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignKeyAction;

  Schema::Client client;
  Schema::Address address;
  ForeignKeySettings settings;
  settings.setOnDeleteAction(ForeignKeyAction::Restrict);
  /*
   * Initial state
   */
  ForeignKeyList list;
  QCOMPARE(list.size(), 0);
  /*
   * Add 1 field foreign key
   */
  list.addForeignKey(address.tableName(), address.Client_Id_FK(), ForeignTable(client), ForeignField(client.Id_PK()), settings);
  QCOMPARE( list.size(), 1 );
  QCOMPARE( list.at(0).tableName() , QString("Address_tbl") );
  QCOMPARE( list.at(0).foreignTableName(), QString("Client_tbl") );
  QCOMPARE( list.at(0).fieldNameList(), QStringList({"Client_Id_FK"}) );
  QCOMPARE( list.at(0).foreignTableFieldNameList() , QStringList({"Id_PK"}) );
  QVERIFY( list.at(0).onDeleteAction() == ForeignKeyAction::Restrict );
  /*
   * Add 2 fields foreign key
   */
  // Setup fields
  Field Id_A;
  Id_A.setType(FieldType::Integer);
  Id_A.setName("Id_A");
  Field Id_A_FK;
  Id_A_FK.setType(FieldType::Integer);
  Id_A_FK.setName("Id_A_FK");
  Field Id_B;
  Id_B.setType(FieldType::Integer);
  Id_B.setName("Id_B");
  Field Id_B_FK;
  Id_B_FK.setType(FieldType::Integer);
  Id_B_FK.setName("Id_B_FK");
  // Check
  list.addForeignKey("Table", FieldList(Id_A_FK, Id_B_FK) , ForeignTable("ForeignTable"), ForeignFieldList(Id_A, Id_B), settings);
  QCOMPARE( list.size(), 2 );
  QCOMPARE( list.at(1).tableName() , QString("Table") );
  QCOMPARE( list.at(1).foreignTableName(), QString("ForeignTable") );
  QCOMPARE( list.at(1).fieldNameList(), QStringList({"Id_A_FK", "Id_B_FK"}) );
  QCOMPARE( list.at(1).foreignTableFieldNameList() , QStringList({"Id_A", "Id_B"}) );
  QVERIFY( list.at(1).onDeleteAction() == ForeignKeyAction::Restrict );
  /*
   * Setup a foreign key and add it
   */
  ForeignKey fk;
  fk.setTableName("Table2");
  fk.setForeignTableName("ForeignTable2");
  fk.setFields(Id_A_FK, ForeignField(Id_A));
  fk.setOnDeleteAction(ForeignKeyAction::SetDefault);
  list.append(fk);
  QCOMPARE( list.size(), 3 );
  QCOMPARE( list.at(2).tableName() , QString("Table2") );
  QCOMPARE( list.at(2).foreignTableName(), QString("ForeignTable2") );
  QCOMPARE( list.at(2).fieldNameList(), QStringList({"Id_A_FK"}) );
  QCOMPARE( list.at(2).foreignTableFieldNameList() , QStringList({"Id_A"}) );
  QVERIFY( list.at(2).onDeleteAction() == ForeignKeyAction::SetDefault );
  /*
   * Clear
   */
  list.clear();
  QCOMPARE( list.size(), 0 );
}

void SchemaForeignKeyTest::foreignKeyListUpdateTableTest()
{
  using Sql::Schema::ForeignKeyList;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignKeySettings;

  Schema::Client client;
  Schema::Address address;
  ForeignKeyList list;

  list.addForeignKey("", address.Client_Id_FK(), ForeignTable(client), ForeignField(client.Id_PK()), ForeignKeySettings());
  list.addForeignKey("", address.Client_Id_FK(), ForeignTable(client), ForeignField(client.Id_PK()), ForeignKeySettings());
  QCOMPARE( list.size(), 2 );
  QVERIFY( list.at(0).tableName().isEmpty() );
  QVERIFY( list.at(1).tableName().isEmpty() );
  list.updateTableName("TableA");
  QCOMPARE( list.at(0).tableName() , QString("TableA") );
  QCOMPARE( list.at(1).tableName() , QString("TableA") );
  list.updateChildTableName("TableB");
  QCOMPARE( list.at(0).tableName() , QString("TableB") );
  QCOMPARE( list.at(1).tableName() , QString("TableB") );
}

void SchemaForeignKeyTest::foreignKeyListGettersTest()
{
  using Sql::Schema::ForeignKeyList;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldList;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignFieldList;
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignKeyAction;

  /*
   * Setup fields
   */
  Field Id;
  Id.setType(FieldType::Integer);
  Id.setName("Id");
  Field Id_FK;
  Id_FK.setType(FieldType::Integer);
  Id_FK.setName("Id_FK");
  /*
   * Initial state
   */
  ForeignKeyList list;
  QCOMPARE( list.size(), 0 );
  QVERIFY( list.isEmpty() );
  /*
   * Add and check
   */
  list.addForeignKey("A", Id_FK, ForeignTable("FA"), ForeignField(Id), ForeignKeySettings());
  QCOMPARE( list.size(), 1 );
  QVERIFY( !list.isEmpty() );
  QVERIFY( !list.at(0).isNull() );
  list.addForeignKey("B", Id_FK, ForeignTable("FB"), ForeignField(Id), ForeignKeySettings());
  QCOMPARE( list.size(), 2 );
  QVERIFY( !list.isEmpty() );
  QVERIFY( !list.at(0).isNull() );
  QVERIFY( !list.at(1).isNull() );
  /*
   * Check foreignKeyReferencing()
   */
  QCOMPARE( list.foreignKeyReferencing("FA").tableName() , QString("A") );
  QCOMPARE( list.foreignKeyReferencing("FA").foreignTableName() , QString("FA") );
  QCOMPARE( list.foreignKeyReferencing("fa").foreignTableName() , QString("FA") );
  QCOMPARE( list.foreignKeyReferencing("Fa").foreignTableName() , QString("FA") );
  QCOMPARE( list.foreignKeyReferencing("FB").tableName() , QString("B") );
  QCOMPARE( list.foreignKeyReferencing("FB").foreignTableName() , QString("FB") );
  QCOMPARE( list.foreignKeyReferencing("fb").foreignTableName() , QString("FB") );
  QCOMPARE( list.foreignKeyReferencing("Fb").foreignTableName() , QString("FB") );
  QVERIFY( list.foreignKeyReferencing("").tableName().isEmpty() );
  QVERIFY( list.foreignKeyReferencing("").foreignTableName().isEmpty() );
  QVERIFY( list.foreignKeyReferencing("A").tableName().isEmpty() );
  QVERIFY( list.foreignKeyReferencing("A").foreignTableName().isEmpty() );
  QVERIFY( list.foreignKeyReferencing("B").tableName().isEmpty() );
  QVERIFY( list.foreignKeyReferencing("B").foreignTableName().isEmpty() );
  QVERIFY( list.foreignKeyReferencing("K").tableName().isEmpty() );
  QVERIFY( list.foreignKeyReferencing("K").foreignTableName().isEmpty() );
  /*
   * Clear
   */
  list.clear();
  QCOMPARE( list.size(), 0 );
  QVERIFY( list.isEmpty() );
}

void SchemaForeignKeyTest::foreignKeyListIteratorsTest()
{
  using Sql::Schema::ForeignKeyList;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignKeySettings;

  Schema::Client client;
  Schema::Address address;
  ForeignKeyList list;

  list.addForeignKey(address.tableName(), address.Client_Id_FK(), ForeignTable(client), ForeignField(client.Id_PK()), ForeignKeySettings());
  for(const auto & fk : list){
    QCOMPARE( fk.tableName() , QString("Address_tbl") );
    QCOMPARE( fk.foreignTableName() , QString("Client_tbl") );
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaForeignKeyTest test;

  return QTest::qExec(&test, argc, argv);
}
