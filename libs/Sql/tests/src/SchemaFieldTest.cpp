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
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/Collation.h"
#include "Mdt/Sql/Schema/FieldTypeList.h"
#include "Mdt/Sql/Schema/FieldTypeListModel.h"
#include "Mdt/Sql/Schema/FieldTypeName.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/FieldList.h"
#include <QStringList>
#include <QComboBox>
#include <QTableView>
#include <QTreeView>

#include <QDebug>

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

void SchemaFieldTest::fiedTypeListModelTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldTypeList;
  using Sql::Schema::FieldTypeListModel;

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

void SchemaFieldTest::fieldListFieldIndexBenchmark()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldList;

  const int N = 10;
  FieldList list = getFieldList(N, FieldType::Integer);

  QBENCHMARK{
    for(int i = 0; i < N; ++i){
      QCOMPARE( list.fieldIndex(QString::number(i)) , i );
    }
  }
}

void SchemaFieldTest::fieldListToStringListTest()
{
  using Sql::Schema::FieldType;
  using Sql::Schema::FieldList;

  const int N = 100;
  FieldList list = getFieldList(N, FieldType::Integer);
  QStringList nameList;

  QBENCHMARK{
    nameList = list.toFieldNameList();
  }
  for(int i = 0; i < N; ++i){
    QCOMPARE(nameList.at(i) , list.at(i).name());
  }
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
