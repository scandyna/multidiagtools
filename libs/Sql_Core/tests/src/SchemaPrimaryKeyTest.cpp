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
#include "SchemaPrimaryKeyTest.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyContainer.h"
#include "Mdt/Sql/Schema/PrimaryKeyFieldIndexList.h"
#include "Mdt/Sql/Schema/FieldIndexList.h"
#include "Mdt/Sql/Schema/FieldList.h"

namespace Sql = Mdt::Sql;

void SchemaPrimaryKeyTest::initTestCase()
{
}

void SchemaPrimaryKeyTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaPrimaryKeyTest::autoIncrementPrimaryKeyIsValidFieldTypeTest()
{
  using namespace Mdt::Sql::Schema;

  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::UnknownType));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Boolean));
  QVERIFY( AutoIncrementPrimaryKey::isValidFieldType(FieldType::Smallint));
  QVERIFY( AutoIncrementPrimaryKey::isValidFieldType(FieldType::Integer));
  QVERIFY( AutoIncrementPrimaryKey::isValidFieldType(FieldType::Bigint));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Float));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Double));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Char));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Varchar));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Text));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Blob));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Date));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::Time));
  QVERIFY(!AutoIncrementPrimaryKey::isValidFieldType(FieldType::DateTime));
}

void SchemaPrimaryKeyTest::autoIncrementPrimaryKeyTest()
{
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::FieldType;

  /*
   * Default constructed initial state
   */
  AutoIncrementPrimaryKey pk;
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
  /*
   * Construct with field name
   */
  AutoIncrementPrimaryKey pk2("Id_PK_2");
  QCOMPARE(pk2.fieldName(), QString("Id_PK_2"));
  QVERIFY(pk2.fieldType() == FieldType::Integer);
  QVERIFY(!pk2.isNull());
  /*
   * Set alternate field type
   */
  AutoIncrementPrimaryKey pk3("Id_PK_3", FieldType::Smallint);
  QCOMPARE(pk3.fieldName(), QString("Id_PK_3"));
  QVERIFY(pk3.fieldType() == FieldType::Smallint);
  pk3.setFieldType(FieldType::Bigint);
  QCOMPARE(pk3.fieldName(), QString("Id_PK_3"));
  QVERIFY(pk3.fieldType() == FieldType::Bigint);
}

void SchemaPrimaryKeyTest::primaryKeyTest()
{
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::PrimaryKey;

  /*
   * Mutable usage
   */
  // Initial state
  PrimaryKey pk;
  QCOMPARE(pk.fieldCount(), 0);
  QVERIFY(pk.isNull());
  // Set/get
  Field Id_PK;
  Id_PK.setName("Id_PK");
  Id_PK.setType(FieldType::Integer);
  pk.addField(Id_PK);
  QCOMPARE(pk.fieldCount(), 1);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.fieldNameList().at(0), QString("Id_PK"));
  // Clear
  pk.clear();
  QCOMPARE(pk.fieldCount(), 0);
  QVERIFY(pk.isNull());
  /*
   * Construct with variadic arguments
   */
  // Setup fields
  Field fieldA, fieldB, fieldC;
  fieldA.setName("A");
  fieldA.setType(FieldType::Integer);
  fieldB.setName("B");
  fieldB.setType(FieldType::Integer);
  fieldC.setName("C");
  fieldC.setType(FieldType::Integer);
  // 1 field
  PrimaryKey pk1(fieldA);
  QCOMPARE(pk1.fieldCount(), 1);
  QCOMPARE(pk1.fieldNameList().at(0), QString("A"));
  // 2 fields
  PrimaryKey pk2(fieldA, fieldB);
  QCOMPARE(pk2.fieldCount(), 2);
  QCOMPARE(pk2.fieldNameList().at(0), QString("A"));
  QCOMPARE(pk2.fieldNameList().at(1), QString("B"));
  // 3 fields
  PrimaryKey pk3(fieldA, fieldB, fieldC);
  QCOMPARE(pk3.fieldCount(), 3);
  QCOMPARE(pk3.fieldNameList().at(0), QString("A"));
  QCOMPARE(pk3.fieldNameList().at(1), QString("B"));
  QCOMPARE(pk3.fieldNameList().at(2), QString("C"));
  /*
   * Check contains()
   */
  QVERIFY(pk3.contains("A"));
  QVERIFY(pk3.contains("a"));
  QVERIFY(!pk3.contains("Z"));
  QVERIFY(!pk3.contains(""));
  /*
   * Set field name list
   */
  pk3.setFieldNameList({"E","F"});
  QCOMPARE(pk3.fieldCount(), 2);
  QCOMPARE(pk3.fieldNameList().at(0), QString("E"));
  QCOMPARE(pk3.fieldNameList().at(1), QString("F"));
  /*
   * Must not compile
   */
//   PrimaryKey badPk1("A");
//   PrimaryKey badPk2(Sql::Schema::AutoIncrementPrimaryKey("A"));
}

void SchemaPrimaryKeyTest::primaryKeyContainerTest()
{
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::PrimaryKey;
  using Sql::Schema::PrimaryKeyContainer;
  using Sql::Schema::PrimaryKeyType;

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

  /*
   * Initial state
   */
  PrimaryKeyContainer container;
  QVERIFY(container.primaryKeyType() == PrimaryKeyType::Unknown);
  QVERIFY(container.isNull());
  QVERIFY(container.fieldName().isEmpty());
  ///qDebug() << "List: " << container.getFieldNameList();
  QVERIFY(container.fieldNameList().isEmpty());
  QVERIFY(container.fieldType() == FieldType::UnknownType);
  /*
   * AutoIncrementPrimaryKey
   */
  container.setPrimaryKey(Id_PK);
  QVERIFY(container.primaryKeyType() == PrimaryKeyType::AutoIncrementPrimaryKey);
  QVERIFY(!container.isNull());
  QCOMPARE(container.fieldName(), QString("Id_PK"));
  QCOMPARE(container.fieldNameList(), QStringList({"Id_PK"}));
  QVERIFY(container.fieldType() == FieldType::Integer);
  QCOMPARE(container.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
  /*
   * PrimaryKey
   */
  // Variadic field
  container.setPrimaryKey(Id_A, Id_B);
  QVERIFY(container.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QVERIFY(!container.isNull());
  QVERIFY(container.fieldName().isEmpty());
  QCOMPARE(container.fieldNameList(), QStringList({"Id_A","Id_B"}));
  QVERIFY(container.fieldType() == FieldType::UnknownType);
  QCOMPARE(container.primaryKey().fieldCount(), 2);
  // PrimaryKey
  PrimaryKey pk(Id_B);
  container.setPrimaryKey(pk);
  QVERIFY(container.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QVERIFY(!container.isNull());
  QCOMPARE(container.primaryKey().fieldCount(), 1);
  QCOMPARE(container.fieldNameList(), QStringList({"Id_B"}));
  /*
   * Clear
   */
  container.clear();
  QVERIFY(container.primaryKeyType() == PrimaryKeyType::Unknown);
  QVERIFY(container.isNull());
}

void SchemaPrimaryKeyTest::fieldIndexListTest()
{
  using namespace Mdt::Sql::Schema;

  /*
   * Initial state
   */
  PrimaryKeyFieldIndexList list;
  QCOMPARE(list.count(), 0);
  QVERIFY(list.isEmpty());
  QVERIFY(!list.containsFieldIndex(0));
  QVERIFY(!list.containsFieldIndex(5));
  /*
   * Add a field index
   */
  list.addFieldIndex(2);
  QCOMPARE(list.count(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.fieldIndexAt(0), 2);
  QVERIFY(!list.containsFieldIndex(0));
  QVERIFY( list.containsFieldIndex(2));
  QVERIFY(!list.containsFieldIndex(5));
  /*
   * Set a field index list
   */
  FieldIndexList indexList;
  indexList.addFieldIndex(5);
  indexList.addFieldIndex(3);
  list.setFieldIndexList(indexList);
  QCOMPARE(list.count(), 2);
  QCOMPARE(list.fieldIndexAt(0), 5);
  QCOMPARE(list.fieldIndexAt(1), 3);
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.count(), 0);
  QVERIFY(list.isEmpty());
}

void SchemaPrimaryKeyTest::fieldIndexListToPrimaryKeyTest()
{
  using namespace Mdt::Sql::Schema;

  /*
   * Create a field list
   */
  FieldList fieldList;
  Field field;
  field.setType(FieldType::Integer);
  field.setName("Id_PK");
  fieldList.addField(field);
  field.setName("Id_A");
  fieldList.addField(field);
  field.setName("Id_B");
  fieldList.addField(field);
  /*
   * To auto increment primary key
   */
  PrimaryKeyFieldIndexList fieldIndexList;
  fieldIndexList.addFieldIndex(0);
  auto aicPk = fieldIndexList.toAutoIncrementPrimaryKey(fieldList);
  QCOMPARE(aicPk.fieldName(), QString("Id_PK"));
  /*
   * To primary key
   */
  fieldIndexList.clear();
  fieldIndexList.addFieldIndex(1);
  fieldIndexList.addFieldIndex(2);
  auto pk = fieldIndexList.toPrimaryKey(fieldList);
  QCOMPARE(pk.fieldCount(), 2);
  QCOMPARE(pk.fieldNameList(), QStringList({"Id_A","Id_B"}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaPrimaryKeyTest test;

  return QTest::qExec(&test, argc, argv);
}
