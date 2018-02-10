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
#include "SchemaFieldTest.h"
#include "Mdt/Sql/Schema/Collation.h"
#include "Mdt/Sql/Schema/FieldLength.h"
#include "Mdt/Sql/Schema/FieldTypeFlags.h"
#include "Mdt/Sql/Schema/FieldTypeList.h"
#include "Mdt/Sql/Schema/FieldTypeName.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/FieldList.h"
#include "Mdt/Sql/Schema/FieldIndexList.h"
#include <QStringList>
#include <algorithm>

// #include <QDebug>

namespace Sql = Mdt::Sql;

void SchemaFieldTest::initTestCase()
{
}

void SchemaFieldTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaFieldTest::fieldLengthTest()
{
  using Sql::Schema::FieldLength;

  FieldLength l0;
  QVERIFY(l0.isNull());
  QCOMPARE(l0.value(), 0);

  FieldLength l00(0);
  QVERIFY(l00.isNull());
  QCOMPARE(l00.value(), 0);

  FieldLength l1(1);
  QVERIFY(!l1.isNull());
  QCOMPARE(l1.value(), 1);
}

void SchemaFieldTest::fieldTypeFlagsTest()
{
  using Sql::Schema::FieldTypeFlag;
  using Sql::Schema::FieldTypeFlags;

  FieldTypeFlags flags0;
  QVERIFY(flags0.testFlag(FieldTypeFlag::NoFlag));
  QVERIFY(!flags0.testFlag(FieldTypeFlag::IsUnsigned));

  FieldTypeFlags flagsUnsigned = FieldTypeFlag::IsUnsigned;
  QVERIFY(!flagsUnsigned.testFlag(FieldTypeFlag::NoFlag));
  QVERIFY(flagsUnsigned.testFlag(FieldTypeFlag::IsUnsigned));
}

void SchemaFieldTest::fieldTypeListTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldTypeList;

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

void SchemaFieldTest::fieldTypeNameTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldTypeName;

  /*
   * Field type -> name
   */
  QVERIFY(FieldTypeName::nameFromType(FieldType::UnknownType).isEmpty());
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Boolean), QString("BOOLEAN"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Smallint), QString("SMALLINT"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Integer), QString("INTEGER"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Bigint), QString("BIGINT"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Float), QString("FLOAT"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Double), QString("DOUBLE"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Char), QString("CHAR"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Varchar), QString("VARCHAR"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Text), QString("TEXT"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Blob), QString("BLOB"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Date), QString("DATE"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::Time), QString("TIME"));
  QCOMPARE(FieldTypeName::nameFromType(FieldType::DateTime), QString("DATETIME"));
  /*
   * Name -> field type
   */
  QVERIFY(FieldTypeName::typeFromName("") == FieldType::UnknownType);
  QVERIFY(FieldTypeName::typeFromName("BOOLEAN") == FieldType::Boolean);
  QVERIFY(FieldTypeName::typeFromName("SMALLINT") == FieldType::Smallint);
  QVERIFY(FieldTypeName::typeFromName("INTEGER") == FieldType::Integer);
  QVERIFY(FieldTypeName::typeFromName("BIGINT") == FieldType::Bigint);
  QVERIFY(FieldTypeName::typeFromName("FLOAT") == FieldType::Float);
  QVERIFY(FieldTypeName::typeFromName("DOUBLE") == FieldType::Double);
  QVERIFY(FieldTypeName::typeFromName("CHAR") == FieldType::Char);
  QVERIFY(FieldTypeName::typeFromName("VARCHAR") == FieldType::Varchar);
  QVERIFY(FieldTypeName::typeFromName("TEXT") == FieldType::Text);
  QVERIFY(FieldTypeName::typeFromName("BLOB") == FieldType::Blob);
  QVERIFY(FieldTypeName::typeFromName("DATE") == FieldType::Date);
  QVERIFY(FieldTypeName::typeFromName("TIME") == FieldType::Time);
  QVERIFY(FieldTypeName::typeFromName("DATETIME") == FieldType::DateTime);
  QVERIFY(FieldTypeName::typeFromName("abcd") == FieldType::UnknownType);
}

void SchemaFieldTest::fieldTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::Field;

  /*
   * Initial state
   */
  Field field;
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(!field.isRequired());
  QVERIFY(!field.isUnique());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.isNull());
  /*
   * Simple set/get check
   */
  // Setup a required and unique integer field
  field.setType(FieldType::Integer);
  QVERIFY(field.isNull());
  field.setName("Id_PK");
  QVERIFY(!field.isNull());
  field.setRequired(true);
  field.setUnique(true);
  // Check
  QVERIFY(field.type() == FieldType::Integer);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.isRequired());
  QVERIFY(field.isUnique());
  /*
   * Clear
   */
  field.clear();
  QVERIFY(field.type() == FieldType::UnknownType);
  QVERIFY(field.name().isEmpty());
  QVERIFY(!field.isRequired());
  QVERIFY(!field.isUnique());
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
  QVERIFY(!field.isUnique());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.collation().isNull());
  QVERIFY(field.isNull());
}

void SchemaFieldTest::fieldListMutatingTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::Field;
  using Sql::Schema::FieldList;

  FieldList list;
  Field field;
  field.setType(FieldType::Integer);
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
  // Check addField()
  field.setName("B");
  list.addField(field);
  QCOMPARE(list.size(), 2);
  QCOMPARE(list.at(0).name(), QString("Afield"));
  QCOMPARE(list.at(1).name(), QString("B"));
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
   * Update element
   */
  QCOMPARE(list.at(0).name(), QString("Afield"));
  list[0].setName("OtherFieldName");
  QCOMPARE(list.at(0).name(), QString("OtherFieldName"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
}

QStringList fieldListVarArgConstructTestFunction(const Sql::Schema::FieldList & list)
{
  return list.toFieldNameList();
}

void SchemaFieldTest::fieldListVarArgConstructTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::Field;
  using Sql::Schema::FieldList;

  QStringList nameList;
  /*
   * Setup fields
   */
  Field A;
  A.setType(FieldType::Integer);
  A.setName("A");
  Field B;
  B.setType(FieldType::Integer);
  B.setName("B");
  Field C;
  C.setType(FieldType::Integer);
  C.setName("C");
  /*
   * Check
   */
  // Must not compile
//   nameList = fieldListVarArgConstructTestFunction("A");

  // 0 field
  nameList = fieldListVarArgConstructTestFunction( FieldList() );
  QCOMPARE(nameList.size(), 0);
  // 1 field
  nameList = fieldListVarArgConstructTestFunction( FieldList(A) );
  QCOMPARE(nameList.size(), 1);
  QCOMPARE(nameList.at(0), QString("A"));
  // 2 fields
  nameList = fieldListVarArgConstructTestFunction( FieldList(A, B) );
  QCOMPARE(nameList.size(), 2);
  QCOMPARE(nameList.at(0), QString("A"));
  QCOMPARE(nameList.at(1), QString("B"));
  // 3 fields
  nameList = fieldListVarArgConstructTestFunction( FieldList(A, B, C) );
  QCOMPARE(nameList.size(), 3);
  QCOMPARE(nameList.at(0), QString("A"));
  QCOMPARE(nameList.at(1), QString("B"));
  QCOMPARE(nameList.at(2), QString("C"));
}

void SchemaFieldTest::fieldListToFieldNameList()
{
  using Sql::Schema::FieldType;

  auto list = getFieldList(3 , FieldType::Integer);
  QStringList nameList = list.toFieldNameList();
  QCOMPARE(nameList.size(), 3);
  QCOMPARE(nameList.at(0), QString("0"));
  QCOMPARE(nameList.at(1), QString("1"));
  QCOMPARE(nameList.at(2), QString("2"));
}

void SchemaFieldTest::fieldListToStringListTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldList;

  const int N = 100;
  FieldList list = getFieldList(N, FieldType::Integer);
  QStringList nameList;

  nameList = list.toFieldNameList();
  for(int i = 0; i < N; ++i){
    QCOMPARE(nameList.at(i) , list.at(i).name());
  }
}

void SchemaFieldTest::fieldIndexListTest()
{
  using namespace Mdt::Sql::Schema;

  /*
   * Initial state
   */
  FieldIndexList list;
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
   * Clear
   */
  list.clear();
  QCOMPARE(list.count(), 0);
  QVERIFY(list.isEmpty());
}

void SchemaFieldTest::fieldIndexListStlTest()
{
  using namespace Mdt::Sql::Schema;

  FieldIndexList list;
  list.addFieldIndex(3);
  list.addFieldIndex(5);

  QVERIFY( std::find(list.begin(), list.end(), 22) == list.end() );
  QVERIFY( std::find(list.begin(), list.end(), 3) != list.end() );
  QVERIFY( std::find(list.cbegin(), list.cend(), 22) == list.end() );
  QVERIFY( std::find(list.cbegin(), list.cend(), 5) != list.end() );
}

/*
 * Helper functions
 */

Sql::Schema::FieldList SchemaFieldTest::getFieldList(int N, Sql::Schema::FieldType type)
{
  using Sql::Schema::FieldType;
  using Sql::Schema::Field;
  using Sql::Schema::FieldList;

  FieldList list;

  for(int i = 0; i < N; ++i){
    Field field;
    field.setType(type);
    field.setName( QString::number(i) );
    list.append(field);
  }

  return list;
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaFieldTest test;

  return QTest::qExec(&test, argc, argv);
}
