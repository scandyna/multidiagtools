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
#include "SchemaTableTest.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/ForeignTable.h"
#include "Mdt/Sql/Schema/TableList.h"
#include "Schema/Client.h"
#include "Schema/Address.h"

namespace Sql = Mdt::Sql;

void SchemaTableTest::initTestCase()
{
}

void SchemaTableTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaTableTest::setAutoIncrementPrimaryKeyTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::PrimaryKeyType;
  using Sql::Schema::PrimaryKeyContainer;

  /*
   * Setup fields
   */
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  /*
   * Init table
   */
  Table table;
  QCOMPARE(table.fieldCount(), 0);
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::Unknown);
  /*
   * Add auto increment PK at first
   */
  // Add Id_PK
  table.setAutoIncrementPrimaryKey("Id_PK");
  QCOMPARE(table.fieldCount(), 1);
  // Add Name
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 2);
  // Check
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
  QVERIFY(table.isFieldUnique(0));
  QVERIFY(!table.isFieldUnique(1));
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
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(table.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
  /*
   * Add auto increment PK after a other field
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::Unknown);
  // Add Name
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 1);
  // Add Id_PK
  table.setAutoIncrementPrimaryKey("Id_PK", FieldType::Bigint);
  QCOMPARE(table.fieldCount(), 2);
  // Check
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Name"));
  QCOMPARE(table.fieldName(1), QString("Id_PK"));
  QVERIFY(!table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldRequired(0));
  QVERIFY(table.isFieldRequired(1));
  QVERIFY(!table.isFieldUnique(0));
  QVERIFY(table.isFieldUnique(1));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Varchar);
  QVERIFY(table.fieldType(1) == FieldType::Bigint);
  QCOMPARE(table.fieldLength(0), 100);
  QCOMPARE(table.fieldLength(1), -1);
  QCOMPARE(table.fieldIndex("Id_PK"), 1);
  QCOMPARE(table.fieldIndex("ID_PK"), 1);
  QCOMPARE(table.fieldIndex("Name"), 0);
  QCOMPARE(table.fieldIndex("NONE"), -1);
  QCOMPARE(table.fieldIndex(""), -1);
  QVERIFY(table.contains("Id_PK"));
  QVERIFY(table.contains("ID_PK"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(table.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
  /*
   * Set Id_PK as field first, then set it as auto increment primary key
   * Use the variant that will only check field type
   */
  // Setup Id_PK field
  Field Id_PK;
  Id_PK.setName("Id_PK");
  Id_PK.setType(FieldType::Smallint);
  // Setup table
  table.clear();
  table.addField(Id_PK);
  table.addField(Name);
  table.setAutoIncrementPrimaryKey("Id_PK");
  // Check
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
  QVERIFY(table.isFieldUnique(0));
  QVERIFY(!table.isFieldUnique(1));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Smallint);
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
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(table.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
  /*
   * Set Id_PK as field first, then set it as auto increment primary key
   * Use the variant that let us specify field type
   */
  Id_PK.setType(FieldType::Bigint);
  // Setup table
  table.clear();
  table.addField(Id_PK);
  table.addField(Name);
  table.setAutoIncrementPrimaryKey("Id_PK", FieldType::Bigint);
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
  QVERIFY(table.isFieldUnique(0));
  QVERIFY(!table.isFieldUnique(1));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Bigint);
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
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::AutoIncrementPrimaryKey);
  QCOMPARE(table.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
}

void SchemaTableTest::setSingleFieldPrimaryKeyTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::PrimaryKeyType;
  using Sql::Schema::PrimaryKeyContainer;

  /*
   * Setup fields
   */
  // Code_PK - Note: we not set it required, and check that Table set this flag automatically
  Field Code_PK;
  Code_PK.setName("Code_PK");
  Code_PK.setType(FieldType::Varchar);
  Code_PK.setLength(50);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  /*
   * Init table
   */
  Table table;
  QCOMPARE(table.fieldCount(), 0);
  /*
   * Add Code_PK as field and then set it as the primary key
   */
  // Setup table
  table.addField(Code_PK);
  table.addField(Name);
  table.setPrimaryKey(Code_PK);
  // Check
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Code_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
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
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldCount(), 1);
  /*
   * Set Code_PK as primary key and check that it is automatically added as field
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  // Setup table
  table.setPrimaryKey(Code_PK);
  table.addField(Name);
  // Check
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Code_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
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
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldCount(), 1);
  /*
   * Must not compile
   */
//   table.setPrimaryKey( Sql::Schema::AutoIncrementPrimaryKey("Id_PK") );
}

void SchemaTableTest::setPrimaryKeyTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::PrimaryKeyType;
  using Sql::Schema::PrimaryKeyContainer;

  /*
   * Setup fields
   * Note: we never set required flag explicitly,
   *       so we can check that Table does it for members of primary key
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
   * Init table
   */
  Table table;
  QCOMPARE(table.fieldCount(), 0);
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::Unknown);
  /*
   * - Add Id_A
   * - Add Name
   * - Set Id_A as primary key
   */
  table.addField(Id_A);
  table.addField(Name);
  table.setPrimaryKey(Id_A);
  // Check
  QCOMPARE(table.fieldCount(), 2);
  QCOMPARE(table.fieldName(0), QString("Id_A"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(table.fieldType(0) == FieldType::Integer);
  QVERIFY(table.fieldType(1) == FieldType::Varchar);
  QCOMPARE(table.fieldLength(0), -1);
  QCOMPARE(table.fieldLength(1), 100);
  QCOMPARE(table.fieldIndex("Id_A"), 0);
  QCOMPARE(table.fieldIndex("ID_A"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QVERIFY(table.contains("Id_A"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldCount(), 1);
  /*
   * - Add Id_A
   * - Add Name
   * - Add Id_B
   * - Set Id_A and Id_B as primary key
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  table.addField(Id_A);
  table.addField(Name);
  table.addField(Id_B);
  table.setPrimaryKey(Id_A, Id_B);
  // Check
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.fieldName(0), QString("Id_A"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QCOMPARE(table.fieldName(2), QString("Id_B"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldPartOfPrimaryKey(2));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
  QVERIFY(table.isFieldRequired(2));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(!table.isFieldAutoIncrement(2));
  QVERIFY(table.fieldType(0) == FieldType::Integer);
  QVERIFY(table.fieldType(1) == FieldType::Varchar);
  QVERIFY(table.fieldType(2) == FieldType::Integer);
  QCOMPARE(table.fieldLength(0), -1);
  QCOMPARE(table.fieldLength(1), 100);
  QCOMPARE(table.fieldLength(2), -1);
  QCOMPARE(table.fieldIndex("Id_A"), 0);
  QCOMPARE(table.fieldIndex("ID_A"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QCOMPARE(table.fieldIndex("Id_B"), 2);
  QVERIFY(table.contains("Id_A"));
  QVERIFY(table.contains("Id_B"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldCount(), 2);
  /*
   * - Set Id_A and Id_B as primary key
   * - Add Name
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  table.setPrimaryKey(Id_A, Id_B);
  table.addField(Name);
  // Check
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.fieldName(0), QString("Id_A"));
  QCOMPARE(table.fieldName(1), QString("Id_B"));
  QCOMPARE(table.fieldName(2), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(table.isFieldRequired(1));
  QVERIFY(!table.isFieldRequired(2));
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
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldCount(), 2);
  /*
   * - Add Name
   * - Set Id_A and Id_B as primary key
   */
  table.clear();
  QCOMPARE(table.fieldCount(), 0);
  table.addField(Name);
  table.setPrimaryKey(Id_A, Id_B);
  // Check
  QCOMPARE(table.fieldCount(), 3);
  QCOMPARE(table.fieldName(0), QString("Name"));
  QCOMPARE(table.fieldName(1), QString("Id_A"));
  QCOMPARE(table.fieldName(2), QString("Id_B"));
  QVERIFY(!table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldPartOfPrimaryKey(2));
  QVERIFY(!table.isFieldRequired(0));
  QVERIFY(table.isFieldRequired(1));
  QVERIFY(table.isFieldRequired(2));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(!table.isFieldAutoIncrement(2));
  QVERIFY(table.fieldType(0) == FieldType::Varchar);
  QVERIFY(table.fieldType(1) == FieldType::Integer);
  QVERIFY(table.fieldType(2) == FieldType::Integer);
  QCOMPARE(table.fieldLength(0), 100);
  QCOMPARE(table.fieldLength(1), -1);
  QCOMPARE(table.fieldLength(2), -1);
  QCOMPARE(table.fieldIndex("Name"), 0);
  QCOMPARE(table.fieldIndex("Id_A"), 1);
  QCOMPARE(table.fieldIndex("Id_a"), 1);
  QCOMPARE(table.fieldIndex("Id_B"), 2);
  QVERIFY(table.contains("Id_A"));
  QVERIFY(table.contains("Id_B"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldCount(), 2);
}

void SchemaTableTest::setPrimaryKeyContainerTest()
{
  using namespace Mdt::Sql::Schema;

  PrimaryKeyContainer pkContainer;
  /*
   * Setup fields
   * Note: we never set required flag explicitly,
   *       so we can check that Table does it for members of primary key
   */
  // Id_PK
  Field Id_PK;
  Id_PK.setName("Id_PK");
  Id_PK.setType(FieldType::Bigint);
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
   * Note: only field names are keeped in primary keys
   */
  // Auto increment primary key
  AutoIncrementPrimaryKey aicPk("Id_PK", FieldType::Bigint);
  // Primary key
  PrimaryKey pk;
  pk.setFieldNameList({"Id_A","Id_B"});
  /*
   * Init table
   */
  Table table;
  table.addField(Id_PK);
  table.addField(Id_A);
  table.addField(Id_B);
  table.addField(Name);
  QCOMPARE(table.fieldCount(), 4);
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::Unknown);
  /*
   * Setup table with a auto increment primary key
   */
  pkContainer.setPrimaryKey(aicPk);
  table.setPrimaryKeyContainer(pkContainer);
  QCOMPARE(table.fieldCount(), 4);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Id_A"));
  QCOMPARE(table.fieldName(2), QString("Id_B"));
  QCOMPARE(table.fieldName(3), QString("Name"));
  QVERIFY(table.isFieldPartOfPrimaryKey(0));
  QVERIFY(!table.isFieldPartOfPrimaryKey(1));
  QVERIFY(!table.isFieldPartOfPrimaryKey(2));
  QVERIFY(!table.isFieldPartOfPrimaryKey(3));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(!table.isFieldRequired(1));
  QVERIFY(!table.isFieldRequired(2));
  QVERIFY(!table.isFieldRequired(3));
  QVERIFY(table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(!table.isFieldAutoIncrement(2));
  QVERIFY(!table.isFieldAutoIncrement(3));
  QVERIFY(table.fieldType(0) == FieldType::Bigint);
  QVERIFY(table.fieldType(1) == FieldType::Integer);
  QVERIFY(table.fieldType(2) == FieldType::Integer);
  QVERIFY(table.fieldType(3) == FieldType::Varchar);
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::AutoIncrementPrimaryKey);
  /*
   * Setup table with a primary key
   */
  pkContainer.setPrimaryKey(pk);
  table.setPrimaryKeyContainer(pkContainer);
  QCOMPARE(table.fieldCount(), 4);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Id_A"));
  QCOMPARE(table.fieldName(2), QString("Id_B"));
  QCOMPARE(table.fieldName(3), QString("Name"));
  QVERIFY(!table.isFieldPartOfPrimaryKey(0));
  QVERIFY(table.isFieldPartOfPrimaryKey(1));
  QVERIFY(table.isFieldPartOfPrimaryKey(2));
  QVERIFY(!table.isFieldPartOfPrimaryKey(3));
  QVERIFY(table.isFieldRequired(0));
  QVERIFY(table.isFieldRequired(1));
  QVERIFY(table.isFieldRequired(2));
  QVERIFY(!table.isFieldRequired(3));
  QVERIFY(!table.isFieldAutoIncrement(0));
  QVERIFY(!table.isFieldAutoIncrement(1));
  QVERIFY(!table.isFieldAutoIncrement(2));
  QVERIFY(!table.isFieldAutoIncrement(3));
  QVERIFY(table.fieldType(0) == FieldType::Bigint);
  QVERIFY(table.fieldType(1) == FieldType::Integer);
  QVERIFY(table.fieldType(2) == FieldType::Integer);
  QVERIFY(table.fieldType(3) == FieldType::Varchar);
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::PrimaryKey);
  QCOMPARE(table.primaryKey().fieldCount(), 2);
}

void SchemaTableTest::setForeignKeySingleFieldTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignKeyAction;

  Schema::Client client;
  /*
   * Setup fields
   */
  Field Client_Id_FK;
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(FieldType::Integer);
  Client_Id_FK.setRequired(true);
  /*
   * Setup foreign key settings
   */
  ForeignKeySettings fkSettings;
  fkSettings.setIndexed(true);
  fkSettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  /*
   * Setup table
   */
  Table table;
  table.setTableName("Address_tbl");
  table.setAutoIncrementPrimaryKey("Id_PK");
  table.addForeignKey(Client_Id_FK, ForeignTable(client), ForeignField(client.Id_PK()), fkSettings);
  /*
   * Check
   */
  // Check that field was added to table
  QCOMPARE( table.fieldCount() , 2 );
  QCOMPARE( table.fieldName(1) , QString("Client_Id_FK") );
  QVERIFY( table.fieldType(1) == FieldType::Integer );
  QVERIFY( table.isFieldRequired(1) );
  // Get foreign key referencing client and check
  auto fk = table.foreignKeyReferencing(client);
  QCOMPARE( fk.tableName() , QString("Address_tbl") );
  QCOMPARE( fk.foreignTableName() , QString("Client_tbl") );
  // Check also getting foreign key referencing by table
  auto fk1 = table.foreignKeyReferencing(client.toTable());
  QCOMPARE( fk1.tableName() , QString("Address_tbl") );
  QCOMPARE( fk1.foreignTableName() , QString("Client_tbl") );
  // Check also getting foreign key referencing by table name
  auto fk2 = table.foreignKeyReferencing(client.tableName());
  QCOMPARE( fk2.tableName() , QString("Address_tbl") );
  QCOMPARE( fk2.foreignTableName() , QString("Client_tbl") );
  /*
   * Add a other foreign key on Client_Id_FK
   */
  table.addForeignKey(Client_Id_FK, ForeignTable("OtherTable"), ForeignField("OtherField"), fkSettings);
  // Check that no other field was added
  QCOMPARE( table.fieldCount() , 2 );
  QCOMPARE( table.fieldName(1) , QString("Client_Id_FK") );
  // Check that foreign key was correctly added
  fk = table.foreignKeyReferencing("OtherTable");
  QCOMPARE( fk.tableName() , QString("Address_tbl") );
  QCOMPARE( fk.foreignTableName() , QString("OtherTable") );
}

void SchemaTableTest::setForeignKeyMultipleFieldsTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldList;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignFieldList;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignKeyAction;

  /*
   * Setup fields for child table
   */
  Field startConnectionId;
  startConnectionId.setType(FieldType::Integer);
  startConnectionId.setName("Start_Connection_Id");
  Field endConnectionId;
  endConnectionId.setType(FieldType::Integer);
  endConnectionId.setName("End_Connection_Id");
  /*
   * Setup parent table
   */
  Table articleLink;
  articleLink.setTableName("ArticleLink_tbl");
  articleLink.setPrimaryKey(startConnectionId, endConnectionId);
  /*
   * Setup foreign key settings
   */
  ForeignKeySettings fkSettings;
  fkSettings.setIndexed(true);
  fkSettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  /*
   * Setup child table
   *
   * Note: we not set primary key,
   *  so we can check that adding foreign key adds fields
   */
  Table link;
  link.setTableName("Link_tbl");
  link.addForeignKey( FieldList(startConnectionId, endConnectionId) , ForeignTable(articleLink) , ForeignFieldList(startConnectionId, endConnectionId) , fkSettings );
  /*
   * Check
   */
  // Check that fields were added to table
  QCOMPARE( link.fieldCount() , 2 );
  QCOMPARE( link.fieldName(0) , QString("Start_Connection_Id") );
  QVERIFY( link.fieldType(0) == FieldType::Integer );
  QCOMPARE( link.fieldName(1) , QString("End_Connection_Id") );
  QVERIFY( link.fieldType(1) == FieldType::Integer );
  // Get foreign key referencing articleLink and check
  auto fk = link.foreignKeyReferencing(articleLink);
  QCOMPARE( fk.tableName() , QString("Link_tbl") );
  QCOMPARE( fk.foreignTableName() , QString("ArticleLink_tbl") );
  // Check also getting foreign key referencing by table name
  fk = link.foreignKeyReferencing(articleLink.tableName());
  QCOMPARE( fk.tableName() , QString("Link_tbl") );
  QCOMPARE( fk.foreignTableName() , QString("ArticleLink_tbl") );
  /*
   * Add a other foreign key on Start_Connection_Id
   */
  link.addForeignKey(startConnectionId, ForeignTable("OtherTable"), ForeignField("OtherField"), fkSettings);
  // Check that no other field was added
  QCOMPARE( link.fieldCount() , 2 );
  QCOMPARE( link.fieldName(0) , QString("Start_Connection_Id") );
  // Check that foreign key was correctly added
  fk = link.foreignKeyReferencing("OtherTable");
  QCOMPARE( fk.tableName() , QString("Link_tbl") );
  QCOMPARE( fk.foreignTableName() , QString("OtherTable") );
}

void SchemaTableTest::tableTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::Field;
  using Sql::Schema::FieldType;
  using Sql::Schema::PrimaryKeyType;
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::PrimaryKey;
  using Sql::Schema::PrimaryKeyContainer;
  using Sql::Schema::ForeignKeySettings;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignKey;
  using Sql::Schema::Index;

  ForeignKey fk;
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
  Name.setUnique(true);
  // Remarks
  Field Remarks;
  Remarks.setName("Remarks");
  Remarks.setType(FieldType::Varchar);
  Remarks.setLength(150);
  Remarks.setDefaultValue("Default remark");
  // Connector_Id_FK
  Field Connector_Id_FK;
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(FieldType::Integer);
  Connector_Id_FK.setRequired(true);
  // Init Connector_tbl
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.setAutoIncrementPrimaryKey("Id_PK");
  /*
   * Setup index
   */
  Index index;
  index.addField(Name);

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
  table.setAutoIncrementPrimaryKey("Id_PK");
  QVERIFY(!table.isNull());
  table.addField(Name);
  table.addField(Remarks);
  table.addForeignKey(Connector_Id_FK, ForeignTable(Connector_tbl), ForeignField("Id_PK"), ForeignKeySettings());
  table.addIndex(index);
  // Check
  QCOMPARE(table.fieldCount(), 4);
  QCOMPARE(table.fieldName(0), QString("Id_PK"));
  QCOMPARE(table.fieldName(1), QString("Name"));
  QCOMPARE(table.fieldName(2), QString("Remarks"));
  QCOMPARE(table.fieldName(3), QString("Connector_Id_FK"));
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
  QVERIFY(table.isFieldUnique(0));
  QVERIFY(table.isFieldUnique(1));
  QVERIFY(!table.isFieldUnique(2));
  QVERIFY(table.fieldDefaultValue(0).isNull());
  QVERIFY(table.fieldDefaultValue(1).isNull());
  QCOMPARE(table.fieldDefaultValue(2), QVariant("Default remark"));
  QCOMPARE(table.fieldIndex("Id_PK"), 0);
  QCOMPARE(table.fieldIndex("ID_PK"), 0);
  QCOMPARE(table.fieldIndex("Name"), 1);
  QCOMPARE(table.fieldIndex("NONE"), -1);
  QCOMPARE(table.fieldIndex(""), -1);
  QVERIFY(table.contains("Id_PK"));
  QVERIFY(table.contains("ID_PK"));
  QVERIFY(table.contains("Name"));
  QVERIFY(!table.contains(""));
  QCOMPARE(table.field(1).name(), QString("Name"));
  QCOMPARE(table.field(2).name(), QString("Remarks"));
  QCOMPARE(table.foreignKeyList().size(), 1);
  QCOMPARE(table.foreignKeyList().at(0).parentTableName(), QString("Connector_tbl"));
  QCOMPARE(table.foreignKeyList().at(0).childTableName(), QString("Client_tbl"));
  QCOMPARE(table.indexList().size(), 1);
  QCOMPARE(table.indexList().at(0).tableName(), QString("Client_tbl"));
  fk = table.foreignKeyReferencing("Connector_tbl");
  QVERIFY(!fk.isNull());
  QCOMPARE(fk.parentTableName(), QString("Connector_tbl"));
  QCOMPARE(fk.childTableName(), QString("Client_tbl"));
  /*
   * Check that updating table name is reflected to foreign keys and indexes
   */
  table.setTableName("NewTableName_tbl");
  QCOMPARE(table.foreignKeyList().size(), 1);
  QCOMPARE(table.foreignKeyList().at(0).childTableName(), QString("NewTableName_tbl"));
  QCOMPARE(table.indexList().size(), 1);
  QCOMPARE(table.indexList().at(0).tableName(), QString("NewTableName_tbl"));
  /*
   * Clear
   */
  table.clear();
  QVERIFY(!table.isTemporary());
  QVERIFY(table.tableName().isEmpty());
  QCOMPARE(table.fieldCount(), 0);
  QVERIFY(table.primaryKeyType() == PrimaryKeyType::Unknown);
  QCOMPARE(table.foreignKeyList().size(), 0);
  QCOMPARE(table.indexList().size(), 0);
  QVERIFY(table.isNull());
}

void SchemaTableTest::tableListTest()
{
  using Sql::Schema::Table;
  using Sql::Schema::TableList;

  Schema::Client client;
  TableList list;

  /*
   * Initial state
   */
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  list.append(client);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).tableName(), client.tableName());
  for(const auto & table : list){
    QCOMPARE(table.tableName(), client.tableName());
  }
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaTableTest test;

  return QTest::qExec(&test, argc, argv);
}
