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
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/SingleFieldPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyContainer.h"
#include <QSqlDatabase>
#include <QComboBox>

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
  cb.show();
  while(cb.isVisible()){
    QTest::qWait(500);
  }
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

void SchemaTest::autoIncrementPrimaryKeyTest()
{
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;

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
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::PrimaryKeyContainer;

  /// sandbox
  PrimaryKeyContainer pkc;
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
