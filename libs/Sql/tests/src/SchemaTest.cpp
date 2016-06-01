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
#include "SchemaTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/FieldTypeList.h"
#include "Mdt/Sql/Schema/FieldTypeListModel.h"
#include "Mdt/Sql/Schema/FieldTypeName.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/FieldList.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/SingleFieldPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyContainer.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/TableModel.h"
#include <QSqlDatabase>
#include <QComboBox>
#include <QTableView>
#include <QTreeView>

void SchemaTest::initTestCase()
{
  QSqlDatabase::addDatabase("QSQLITE", "SQLITE_1");
}

void SchemaTest::cleanupTestCase()
{
  QSqlDatabase::removeDatabase("SQLITE_1");
}

/*
 * Tests
 */

void SchemaTest::fieldTypeListTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::FieldTypeList;

  /*
   * Initial state
   */
  FieldTypeList list;
  QCOMPARE(list.size(), 0);
  /*
   * Add one element
   */
  list.append(FieldType::Boolean);
  QCOMPARE(list.size(), 1);
  QVERIFY(list.at(0) == FieldType::Boolean);
  for(auto ft : list){
    QVERIFY(ft == FieldType::Boolean);
  }
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
}

void SchemaTest::fieldTypeNameTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::FieldTypeName;

  /*
   * Field type -> name
   */
  QVERIFY(FieldTypeName::nameFromType(FieldType::UnknownType).isEmpty());
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Boolean), QString("BOOLEAN"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Integer), QString("INTEGER"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Float), QString("FLOAT"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Double), QString("DOUBLE"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Varchar), QString("VARCHAR"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Date), QString("DATE"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Time), QString("TIME"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::DateTime), QString("DATETIME"));
  /*
   * Name -> field type
   */
  QVERIFY(FieldTypeName::typeFromName("") == FieldType::UnknownType);
  QVERIFY(FieldTypeName::typeFromName("BOOLEAN") == FieldType::Boolean);
  QVERIFY(FieldTypeName::typeFromName("INTEGER") == FieldType::Integer);
  QVERIFY(FieldTypeName::typeFromName("FLOAT") == FieldType::Float);
  QVERIFY(FieldTypeName::typeFromName("DOUBLE") == FieldType::Double);
  QVERIFY(FieldTypeName::typeFromName("VARCHAR") == FieldType::Varchar);
  QVERIFY(FieldTypeName::typeFromName("DATE") == FieldType::Date);
  QVERIFY(FieldTypeName::typeFromName("TIME") == FieldType::Time);
  QVERIFY(FieldTypeName::typeFromName("DATETIME") == FieldType::DateTime);
  QVERIFY(FieldTypeName::typeFromName("abcd") == FieldType::UnknownType);
}

void SchemaTest::fiedTypeListModelTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::FieldTypeList;
  using Mdt::Sql::Schema::FieldTypeListModel;

  FieldTypeListModel model;
  QModelIndex index;
  FieldTypeList list;
  QComboBox cb;

  /*
   * Setup combobox
   */
  cb.setModel(&model);
  /*
   * Initial state
   */
  QCOMPARE(model.rowCount(), 0);
  /*
   * Populate model
   */
  list.clear();
  list.append(FieldType::Boolean);
  list.append(FieldType::Integer);
  list.append(FieldType::Varchar);
  model.setFieldTypeList(list);
  /*
   * Check
   */
  QCOMPARE(model.rowCount(), 3);
  // Row 0
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("BOOLEAN"));
  QVERIFY(model.fieldType(0) == FieldType::Boolean);
  // Row 1
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("INTEGER"));
  QVERIFY(model.fieldType(1) == FieldType::Integer);
  // Row 2
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("VARCHAR"));
  QVERIFY(model.fieldType(2) == FieldType::Varchar);
  // Get field type from invalid row
  QVERIFY(model.fieldType(-1) == FieldType::UnknownType);
  QVERIFY(model.fieldType(3) == FieldType::UnknownType);
  QVERIFY(model.fieldType(5) == FieldType::UnknownType);
  /*
   * Play
   */
//   cb.show();
//   while(cb.isVisible()){
//     QTest::qWait(500);
//   }
}

void SchemaTest::collationTest()
{
  using Mdt::Sql::Schema::Collation;

  Collation collation;

  /*
   * Initial state
   */
  QVERIFY(collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::AnyCountry);
  QVERIFY(collation.language() == QLocale::AnyLanguage);
  QVERIFY(collation.isNull());
  /*
   * Simple set/get
   */
  collation.setCaseSensitive(false);
  collation.setCountry(QLocale::Switzerland);
  collation.setLanguage(QLocale::French);
  collation.setCharset("utf8");
  QVERIFY(!collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::Switzerland);
  QVERIFY(collation.language() == QLocale::French);
  QCOMPARE(collation.charset(), QString("utf8"));
  QVERIFY(!collation.isNull());
  /*
   * Clear
   */
  collation.clear();
  QVERIFY(collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::AnyCountry);
  QVERIFY(collation.language() == QLocale::AnyLanguage);
  QVERIFY(collation.charset().isEmpty());
  QVERIFY(collation.isNull());
}

void SchemaTest::fieldTest()
{
  using Mdt::Sql::Schema::Collation;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Field field;

  /*
   * Initial state
   */
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.isNull());
  /*
   * Simple set/get check
   */
  // Setup a required integer field
  field.setType(FieldType::Integer);
  QVERIFY(field.isNull());
  field.setName("Id_PK");
  QVERIFY(!field.isNull());
  field.setRequired(true);
  // Check
  QVERIFY(field.type() == FieldType::Integer);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.isRequired());
  /*
   * Clear
   */
  field.clear();
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(field.name().isEmpty());
  QVERIFY(!field.isRequired());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.collation().isNull());
  QVERIFY(field.isNull());
  /*
   * Simple set/get check
   */
  // Setup a text field
  field.setName("Name");
  QVERIFY(field.isNull());
  field.setType(FieldType::Varchar);
  QVERIFY(!field.isNull());
  field.setLength(50);
  field.setDefaultValue("Empty");
  field.setCaseSensitive(false);
  // Check
  QVERIFY(field.type() == FieldType::Varchar);
  QCOMPARE(field.name(), QString("Name"));
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), 50);
  QCOMPARE(field.defaultValue(), QVariant("Empty"));
  QVERIFY(!field.collation().isCaseSensitive());
  /*
   * Clear
   */
  field.clear();
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(field.name().isEmpty());
  QVERIFY(!field.isRequired());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.collation().isNull());
  QVERIFY(field.isNull());
}

void SchemaTest::fieldListTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldList;

  Field field;
  FieldList list;

  /*
   * Initial state
   */
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add one element
   */
  field.setName("Afield");
  list.append(field);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).name(), QString("Afield"));
  for(const auto & f : list){
    QCOMPARE(f.name(), QString("Afield"));
  }
  /*
   * Check index and contains methods.
   * Note: we use case insensitive search
   */
  QCOMPARE(list.fieldIndex("Afield"), 0);
  QCOMPARE(list.fieldIndex("AFIeld"), 0);
  QCOMPARE(list.fieldIndex(""), -1);
  QCOMPARE(list.fieldIndex("AAA"), -1);
  QVERIFY(list.contains("Afield"));
  QVERIFY(list.contains("AFIeld"));
  QVERIFY(!list.contains(""));
  QVERIFY(!list.contains("AAA"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
}

void SchemaTest::autoIncrementPrimaryKeyTest()
{
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::FieldType;

  AutoIncrementPrimaryKey pk;

  /*
   * Initial state
   */
  QVERIFY(pk.isNull());
  /*
   * Set/get
   */
  pk.setFieldName("Id_PK");
  QCOMPARE(pk.fieldName(), QString("Id_PK"));
  QVERIFY(pk.fieldType() == FieldType::Integer);
  QVERIFY(!pk.isNull());
  /*
   * Clear
   */
  pk.clear();
  QVERIFY(pk.fieldName().isEmpty());
  QVERIFY(pk.isNull());
}

void SchemaTest::singleFieldPrimaryKeyTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;

  SingleFieldPrimaryKey pk;

  /*
   * Initial state
   */
  QVERIFY(pk.fieldType() == FieldType::UnknownType);
  QCOMPARE(pk.fieldLength(), -1);
  QVERIFY(pk.collation().isCaseSensitive());
  QVERIFY(pk.isNull());
  /*
   * Set/get on Integer primary key
   */
  // Setup primary key
  pk.setFieldType(FieldType::Integer);
  QVERIFY(pk.isNull());
  pk.setFieldName("Id_PK");
  QVERIFY(!pk.isNull());
  // Check
  QVERIFY(pk.fieldType() == FieldType::Integer);
  QCOMPARE(pk.fieldName(), QString("Id_PK"));
  /*
   * Clear
   */
  pk.clear();
  QVERIFY(pk.fieldType() == FieldType::UnknownType);
  QVERIFY(pk.fieldName().isEmpty());
  QCOMPARE(pk.fieldLength(), -1);
  QVERIFY(pk.collation().isCaseSensitive());
  QVERIFY(pk.isNull());
  /*
   * Set/get on a text primary key
   */
  // Setup primary key
  pk.setFieldType(FieldType::Varchar);
  pk.setFieldName("Code_PK");
  pk.setFieldLength(50);
  // Check
  QVERIFY(!pk.isNull());
  QVERIFY(pk.fieldType() == FieldType::Varchar);
  QCOMPARE(pk.fieldName(), QString("Code_PK"));
  QCOMPARE(pk.fieldLength(), 50);
  QVERIFY(pk.collation().isCaseSensitive());
  /*
   * Clear
   */
  pk.clear();
  QVERIFY(pk.fieldType() == FieldType::UnknownType);
  QVERIFY(pk.fieldName().isEmpty());
  QCOMPARE(pk.fieldLength(), -1);
  QVERIFY(pk.collation().isCaseSensitive());
  QVERIFY(pk.isNull());
}

void SchemaTest::primaryKeyTest()
{
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::PrimaryKey;

  PrimaryKey pk;

  /*
   * Initial state
   */
  QCOMPARE(pk.fieldCount(), 0);
  QVERIFY(pk.isNull());
  /*
   * Set/get
   */
  Field Id_PK;
  Id_PK.setName("Id_PK");
  pk.addField(Id_PK);
  QCOMPARE(pk.fieldCount(), 1);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.fieldNameList().at(0), QString("Id_PK"));
  /*
   * Clear
   */
  pk.clear();
  QCOMPARE(pk.fieldCount(), 0);
  QVERIFY(pk.isNull());
}

void SchemaTest::primaryKeyContainerTest()
{
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::PrimaryKeyContainer;

  /*
   * Setup fields
   */
  // Id_A
  Field Id_A;
  Id_A.setName("Id_A");
  Id_A.setType(FieldType::Integer);
  // Id_B
  Field Id_B;
  Id_B.setName("Id_B");
  Id_B.setType(FieldType::Integer);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  /*
   * Setup primary keys
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Code_PK
  SingleFieldPrimaryKey Code_PK;
  Code_PK.setFieldName("Code_PK");
  Code_PK.setFieldType(FieldType::Varchar);
  Code_PK.setFieldLength(50);
  // Primary key of Id_A and Id_B
  PrimaryKey Id_A_Id_B_PK;
  Id_A_Id_B_PK.addField(Id_A);
  Id_A_Id_B_PK.addField(Id_B);

  /*
   * Initial state
   */
  PrimaryKeyContainer container;
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::PrimaryKeyType);
  QVERIFY(container.fieldName().isEmpty());
  QVERIFY(container.fieldType() == FieldType::UnknownType);
  QCOMPARE(container.fieldLength(), -1);
  /*
   * AutoIncrementPrimaryKey
   */
  container.setPrimaryKey(Id_PK);
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::AutoIncrementPrimaryKeyType);
  QCOMPARE(container.fieldName(), QString("Id_PK"));
  QVERIFY(container.fieldType() == FieldType::Integer);
  QCOMPARE(container.fieldLength(), -1);
  /*
   * SingleFieldPrimaryKey
   */
  container.setPrimaryKey(Code_PK);
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::SingleFieldPrimaryKeyType);
  QCOMPARE(container.fieldName(), QString("Code_PK"));
  QVERIFY(container.fieldType() == FieldType::Varchar);
  QCOMPARE(container.fieldLength(), 50);
  /*
   * PrimaryKey
   */
  container.setPrimaryKey(Id_A_Id_B_PK);
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::PrimaryKeyType);
  QVERIFY(container.fieldName().isEmpty());
  QVERIFY(container.fieldType() == FieldType::UnknownType);
  QCOMPARE(container.fieldLength(), -1);
}

void SchemaTest::tablePrimaryKeyTest()
{
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;

  /*
   * Setup fields
   */
  // Id_A
  Field Id_A;
  Id_A.setName("Id_A");
  Id_A.setType(FieldType::Integer);
  // Id_B
  Field Id_B;
  Id_B.setName("Id_B");
  Id_B.setType(FieldType::Integer);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  /*
   * Setup primary keys
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Code_PK
  SingleFieldPrimaryKey Code_PK;
  Code_PK.setFieldName("Code_PK");
  Code_PK.setFieldType(FieldType::Varchar);
  Code_PK.setFieldLength(50);
  // Primary key of Id_A and Id_B
  PrimaryKey Id_A_Id_B_PK;
  Id_A_Id_B_PK.addField(Id_A);
  Id_A_Id_B_PK.addField(Id_B);
  /*
   * Init table
   */
  Table table;
  QCOMPARE(table.fieldCount(), 0);
  /*
   * Add auto increment PK at first
   */
  // Add Id_PK
  table.setPrimaryKey(Id_PK);
  QCOMPARE(table.fieldCount(), 1);
  // Add Name
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 2);
  // Check - Note: PK with field definition allways appear as first field in table
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Integer);
  QVERIFY(table.fieldType(1) == FieldType::Varchar);
  QCOMPARE(table.fieldLength(0), -1);
  QCOMPARE(table.fieldLength(1), 100);
  QCOMPARE(table.fieldIndex("Id_PK"), 0);
  QCOMPARE(table.fieldIndex("ID_PK"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QCOMPARE(table.fieldIndex("NONE"), -1);
  QCOMPARE(table.fieldIndex(""), -1);
  QVERIFY(table.contains("Id_PK"));
  QVERIFY(table.contains("ID_PK"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  /*
   * Add auto increment PK after a other field
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  // Add Name
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 1);
  // Add Id_PK
  table.setPrimaryKey(Id_PK);
  QCOMPARE(table.fieldCount(), 2);
  // Check - Note: PK with field definition allways appear as first field in table
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Integer);
  QVERIFY(table.fieldType(1) == FieldType::Varchar);
  QCOMPARE(table.fieldLength(0), -1);
  QCOMPARE(table.fieldLength(1), 100);
  QCOMPARE(table.fieldIndex("Id_PK"), 0);
  QCOMPARE(table.fieldIndex("ID_PK"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QCOMPARE(table.fieldIndex("NONE"), -1);
  QCOMPARE(table.fieldIndex(""), -1);
  QVERIFY(table.contains("Id_PK"));
  QVERIFY(table.contains("ID_PK"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  /*
   * Add single field PK at first
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  // Add Code_PK
  table.setPrimaryKey(Code_PK);
  QCOMPARE(table.fieldCount(), 1);
  // Add Name
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 2);
  // Check - Note: PK with field definition allways appear as first field in table
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Code_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Varchar);
  QVERIFY(table.fieldType(1) == FieldType::Varchar);
  QCOMPARE(table.fieldLength(0), 50);
  QCOMPARE(table.fieldLength(1), 100);
  QCOMPARE(table.fieldIndex("Code_PK"), 0);
  QCOMPARE(table.fieldIndex("Code_pk"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QCOMPARE(table.fieldIndex("NONE"), -1);
  QCOMPARE(table.fieldIndex(""), -1);
  QVERIFY(table.contains("Code_PK"));
  QVERIFY(table.contains("Code_pk"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  /*
   * Add single field PK after a other field
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  // Add Name
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 1);
  // Add Code_PK
  table.setPrimaryKey(Code_PK);
  QCOMPARE(table.fieldCount(), 2);
  // Check - Note: PK with field definition allways appear as first field in table
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Code_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Varchar);
  QVERIFY(table.fieldType(1) == FieldType::Varchar);
  QCOMPARE(table.fieldLength(0), 50);
  QCOMPARE(table.fieldLength(1), 100);
  QCOMPARE(table.fieldIndex("Code_PK"), 0);
  QCOMPARE(table.fieldIndex("Code_pk"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QCOMPARE(table.fieldIndex("NONE"), -1);
  QCOMPARE(table.fieldIndex(""), -1);
  QVERIFY(table.contains("Code_PK"));
  QVERIFY(table.contains("Code_pk"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  /*
   * Add primary key (must be added when fields are added)
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  // Add Id_A
  table.addField(Id_A);
  QCOMPARE(table.fieldCount(), 1);
  // Add Id_B
  table.addField(Id_B);
  QCOMPARE(table.fieldCount(), 2);
  // Add Name
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 3);
  // Add primary key
  table.setPrimaryKey(Id_A_Id_B_PK);
  // Check
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.fieldName(0), QString("Id_A"));
  QCOMPARE(table.fieldName(1), QString("Id_B"));
  QCOMPARE(table.fieldName(2), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(!table.isFieldAutoIncrement(2));
  QVERIFY(table.fieldType(0) == FieldType::Integer);
  QVERIFY(table.fieldType(1) == FieldType::Integer);
  QVERIFY(table.fieldType(2) == FieldType::Varchar);
  QCOMPARE(table.fieldLength(0), -1);
  QCOMPARE(table.fieldLength(1), -1);
  QCOMPARE(table.fieldLength(2), 100);
  QCOMPARE(table.fieldIndex("Id_A"), 0);
  QCOMPARE(table.fieldIndex("ID_A"), 0);
  QCOMPARE(table.fieldIndex("Id_B"), 1);
  QCOMPARE(table.fieldIndex("Name"), 2);
  QVERIFY(table.contains("Id_A"));
  QVERIFY(table.contains("Id_B"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
}

void SchemaTest::tablePrimaryKeyAicBenchmark()
{
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;

  /*
   * Setup fields
   */
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  /*
   * Setup primary key
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");

  QBENCHMARK{
    Table table;
    table.setPrimaryKey(Id_PK);
    table.addField(Name);
    QCOMPARE(table.fieldCount(), 2);
    QCOMPARE(table.fieldName(0), QString("Id_PK"));
    QCOMPARE(table.fieldName(1), QString("Name"));
    QVERIFY(table.isFieldPartOfPrimaryKey(0));
    QVERIFY(!table.isFieldPartOfPrimaryKey(1));
    QVERIFY(table.isFieldAutoIncrement(0));
    QVERIFY(!table.isFieldAutoIncrement(1));
    QVERIFY(table.fieldType(0) == FieldType::Integer);
    QVERIFY(table.fieldType(1) == FieldType::Varchar);
    QCOMPARE(table.fieldLength(0), -1);
    QCOMPARE(table.fieldLength(1), 100);
    QCOMPARE(table.fieldIndex("Id_PK"), 0);
    QCOMPARE(table.fieldIndex("Name"), 1);
  }
}

void SchemaTest::tablePrimaryKeyMcBenchmark()
{
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::PrimaryKey;

  /*
   * Setup fields
   */
  // Id_A
  Field Id_A;
  Id_A.setName("Id_A");
  Id_A.setType(FieldType::Integer);
  // Id_B
  Field Id_B;
  Id_B.setName("Id_B");
  Id_B.setType(FieldType::Integer);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  /*
   * Setup primary key
   */
  // Primary key of Id_A and Id_B
  PrimaryKey Id_A_Id_B_PK;
  Id_A_Id_B_PK.addField(Id_A);
  Id_A_Id_B_PK.addField(Id_B);

  QBENCHMARK{
    Table table;
    table.addField(Id_A);
    table.addField(Id_B);
    table.addField(Name);
    table.setPrimaryKey(Id_A_Id_B_PK);
    QCOMPARE(table.fieldCount(), 3);
    QCOMPARE(table.fieldName(0), QString("Id_A"));
    QCOMPARE(table.fieldName(1), QString("Id_B"));
    QCOMPARE(table.fieldName(2), QString("Name"));
    QVERIFY(table.isFieldPartOfPrimaryKey(0));
    QVERIFY(table.isFieldPartOfPrimaryKey(1));
    QVERIFY(!table.isFieldPartOfPrimaryKey(2));
    QVERIFY(!table.isFieldAutoIncrement(0));
    QVERIFY(!table.isFieldAutoIncrement(1));
    QVERIFY(!table.isFieldAutoIncrement(2));
    QVERIFY(table.fieldType(0) == FieldType::Integer);
    QVERIFY(table.fieldType(1) == FieldType::Integer);
    QVERIFY(table.fieldType(2) == FieldType::Varchar);
    QCOMPARE(table.fieldLength(0), -1);
    QCOMPARE(table.fieldLength(1), -1);
    QCOMPARE(table.fieldLength(2), 100);
    QCOMPARE(table.fieldIndex("Id_A"), 0);
    QCOMPARE(table.fieldIndex("Id_B"), 1);
    QCOMPARE(table.fieldIndex("Name"), 2);
  }
}

void SchemaTest::tableTest()
{
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;

  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  Name.setRequired(true);
  // Remarks
  Field Remarks;
  Remarks.setName("Remarks");
  Remarks.setType(FieldType::Varchar);
  Remarks.setLength(150);

  /*
   * Initial state
   */
  Table table;
  QVERIFY(!table.isTemporary());
  QCOMPARE(table.fieldCount(), 0);
  QVERIFY(table.isNull());
  /*
   * Setup a simple table
   */
  table.setTableName("Client_tbl");
  QVERIFY(table.isNull());
  table.setPrimaryKey(Id_PK);
  QVERIFY(!table.isNull());
  table.addField(Name);
  table.addField(Remarks);
  // Check
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QCOMPARE(table.fieldName(2), QString("Remarks"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(table.fieldType(0) == FieldType::Integer);
  QVERIFY(table.fieldType(1) == FieldType::Varchar);
  QVERIFY(table.fieldType(2) == FieldType::Varchar);
  QCOMPARE(table.fieldLength(0), -1);
  QCOMPARE(table.fieldLength(1), 100);
  QCOMPARE(table.fieldLength(2), 150);
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(table.isFieldRequired(1));
  QVERIFY(!table.isFieldRequired(2));
  QCOMPARE(table.fieldIndex("Id_PK"), 0);
  QCOMPARE(table.fieldIndex("ID_PK"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QCOMPARE(table.fieldIndex("NONE"), -1);
  QCOMPARE(table.fieldIndex(""), -1);
  QVERIFY(table.contains("Id_PK"));
  QVERIFY(table.contains("ID_PK"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  /*
   * Clear
   */
  table.clear();
  QVERIFY(!table.isTemporary());
  QVERIFY(table.tableName().isEmpty());
  QCOMPARE(table.fieldCount(), 0);
  QVERIFY(table.isNull());
}

void SchemaTest::tableModelTest()
{
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::TableModel;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;

  QModelIndex index;
  /*
   * Initial state
   */
  TableModel model;
  QCOMPARE(model.rowCount(), 0);
  /*
   * Setup views
   */
  QTableView tableView;
  tableView.setModel(&model);
  tableView.resize(400, 200);
  QTreeView treeView;
  treeView.setModel(&model);
  treeView.resize(400, 200);
  QComboBox comboBox;
  comboBox.setModel(&model);
  comboBox.setModelColumn(TableModel::FieldNameColumn);
  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  /*
   * Setup a table
   */
  Table Client_tbl;
  Client_tbl.setTableName("Client_tbl");
  Client_tbl.setPrimaryKey(Id_PK);
  Client_tbl.addField(Name);
  /*
   * Set to model and check
   */
  QCOMPARE(model.columnCount(), 5);
  model.setTable(Client_tbl);
  // Id_PK
  index = model.index(0, TableModel::FieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Id_PK"));
  index = model.index(0, TableModel::FieldTypeColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("INTEGER"));
  index = model.index(0, TableModel::PkFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("X"));
  index = model.index(0, TableModel::AiFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("X"));
  // Name
  index = model.index(1, TableModel::FieldNameColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Name"));
  index = model.index(1, TableModel::FieldTypeColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("VARCHAR(100)"));
  index = model.index(1, TableModel::PkFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(""));
  index = model.index(1, TableModel::AiFlagColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(""));

  /// \todo Check
  /*
   * Play
   */
  tableView.show();
  treeView.show();
  comboBox.show();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
