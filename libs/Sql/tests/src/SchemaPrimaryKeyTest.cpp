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
#include "SchemaPrimaryKeyTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/AutoIncrementPrimaryKey.h"
#include "Mdt/Sql/Schema/SingleFieldPrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKey.h"
#include "Mdt/Sql/Schema/PrimaryKeyContainer.h"

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
}

void SchemaPrimaryKeyTest::singleFieldPrimaryKeyTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::Field;
  using Sql::Schema::SingleFieldPrimaryKey;

  SingleFieldPrimaryKey pk;

  /*
   * Initial state
   */
  QVERIFY(pk.fieldType() == FieldType::UnknownType);
  QCOMPARE(pk.fieldLength(), -1);
  /// \todo Check what should be default
  QVERIFY(!pk.collation().isCaseSensitive());
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
  /// \todo Check what should be default
  QVERIFY(!pk.collation().isCaseSensitive());
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
  QVERIFY(!pk.collation().isCaseSensitive());
  /*
   * Clear
   */
  pk.clear();
  QVERIFY(pk.fieldType() == FieldType::UnknownType);
  QVERIFY(pk.fieldName().isEmpty());
  QCOMPARE(pk.fieldLength(), -1);
  /// \todo Check what should be default
  QVERIFY(!pk.collation().isCaseSensitive());
  QVERIFY(pk.isNull());
}

void SchemaPrimaryKeyTest::primaryKeyTest()
{
  using Sql::Schema::Field;
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
  fieldB.setName("B");
  fieldC.setName("C");
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
  using Sql::Schema::SingleFieldPrimaryKey;
  using Sql::Schema::PrimaryKey;
  using Sql::Schema::PrimaryKeyContainer;

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
  QCOMPARE(container.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
  /*
   * SingleFieldPrimaryKey
   */
  container.setPrimaryKey(Code_PK);
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::SingleFieldPrimaryKeyType);
  QCOMPARE(container.fieldName(), QString("Code_PK"));
  QVERIFY(container.fieldType() == FieldType::Varchar);
  QCOMPARE(container.fieldLength(), 50);
  QCOMPARE(container.singleFieldPrimaryKey().fieldName(), QString("Code_PK"));
  /*
   * PrimaryKey
   */
  container.setPrimaryKey(Id_A_Id_B_PK);
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::PrimaryKeyType);
  QVERIFY(container.fieldName().isEmpty());
  QVERIFY(container.fieldType() == FieldType::UnknownType);
  QCOMPARE(container.fieldLength(), -1);
  QCOMPARE(container.primaryKey().fieldCount(), 2);
  /*
   * Clear
   */
  container.clear();
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::PrimaryKeyType);
  QCOMPARE(container.primaryKey().fieldCount(), 0);
  /*
   * Set PrimaryKey with list of fields
   */
  // Switch first to AutoIncrementPrimaryKey
  container.setPrimaryKey( AutoIncrementPrimaryKey("Id_PK") );
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::AutoIncrementPrimaryKeyType);
  // Set PrimaryKey
  container.setPrimaryKey(Id_A, Id_B);
  QVERIFY(container.primaryKeyType() == PrimaryKeyContainer::PrimaryKeyType);
  QVERIFY(container.fieldName().isEmpty());
  QVERIFY(container.fieldType() == FieldType::UnknownType);
  QCOMPARE(container.fieldLength(), -1);
  QCOMPARE(container.primaryKey().fieldCount(), 2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaPrimaryKeyTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
