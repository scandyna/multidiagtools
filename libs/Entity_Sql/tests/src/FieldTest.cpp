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
#include "FieldTest.h"
#include "Mdt/Entity/SqlField.h"
#include "Mdt/Entity/Def.h"
// #include "Mdt/Entity/Field.h"
#include "Mdt/Sql/Schema/Field.h"

using namespace Mdt::Entity;
using namespace Mdt::Sql::Schema;

void FieldTest::initTestCase()
{
}

void FieldTest::cleanupTestCase()
{
}

/*
 * Entities
 */

struct TestCaseDataStruct
{
  qlonglong id;
  QString name;
  double qty;
};

MDT_ENTITY_DEF(
  (TestCaseDataStruct),
  TestCase,
  (id, FieldFlag::IsPrimaryKey),
  (name, FieldFlag::IsRequired, FieldMaxLength(100)),
  (qty)
)

/*
 * Tests
 */

void FieldTest::qmetaTypeFromEntityFieldTest()
{
  /*
   * QCOMPARE does not accept the comma in the template arguments,
   * we have to use a variable.
   */
  QMetaType::Type qmetaType;

  qmetaType = SqlField::qmetaTypeFromEntityField<TestCaseDataStruct, TestCaseDef::idField>();
  QCOMPARE( qmetaType, QMetaType::LongLong );

  qmetaType = SqlField::qmetaTypeFromEntityField<TestCaseDataStruct, TestCaseDef::nameField>();
  QCOMPARE( qmetaType, QMetaType::QString );

  qmetaType = SqlField::qmetaTypeFromEntityField<TestCaseDataStruct, TestCaseDef::qtyField>();
  QCOMPARE( qmetaType, QMetaType::Double );
}

void FieldTest::sqlFieldTypeFromEntityFieldTest()
{
  Mdt::Sql::Schema::FieldType sqlFieldType;
  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  sqlFieldType = SqlField::sqlFieldTypeFromEntityField<TestCaseDataStruct, TestCaseDef::idField>(fieldTypeMap);
  QCOMPARE(sqlFieldType, FieldType::Bigint);

  sqlFieldType = SqlField::sqlFieldTypeFromEntityField<TestCaseDataStruct, TestCaseDef::nameField>(fieldTypeMap);
  QCOMPARE(sqlFieldType, FieldType::Varchar);

  sqlFieldType = SqlField::sqlFieldTypeFromEntityField<TestCaseDataStruct, TestCaseDef::qtyField>(fieldTypeMap);
  QCOMPARE(sqlFieldType, FieldType::Double);
}

void FieldTest::sqlFieldFromEntityFieldTest()
{
  const auto fieldTypeMap = Mdt::Sql::Schema::FieldTypeMap::make();

  const auto id = SqlField::fromEntityField<TestCaseDataStruct, TestCaseDef::idField>(fieldTypeMap);
  QCOMPARE(id.type(), FieldType::Bigint);
  QCOMPARE(id.name(), QString("id"));
  QVERIFY(!id.isRequired());  // Required flag for primary key is handled by Mdt::Sql::Schema::Table
  QVERIFY(!id.isUnique());    // Unique flag for primary key is handled by Mdt::Sql::Schema::Table
  QVERIFY(id.defaultValue().isNull());
  QVERIFY(id.length() < 1);
  QVERIFY(!id.isNull());

  const auto name = SqlField::fromEntityField<TestCaseDataStruct, TestCaseDef::nameField>(fieldTypeMap);
  QCOMPARE(name.type(), FieldType::Varchar);
  QCOMPARE(name.name(), QString("name"));
  QVERIFY(name.isRequired());
  QVERIFY(!name.isUnique());
  QVERIFY(name.defaultValue().isNull());
  QCOMPARE(name.length(), 100);
  QVERIFY(!name.isNull());

  const auto qty = SqlField::fromEntityField<TestCaseDataStruct, TestCaseDef::qtyField>(fieldTypeMap);
  QCOMPARE(qty.type(), FieldType::Double);
  QCOMPARE(qty.name(), QString("qty"));
  QVERIFY(!qty.isRequired());
  QVERIFY(!qty.isUnique());
  QVERIFY(qty.defaultValue().isNull());
  QVERIFY(qty.length() < 1);
  QVERIFY(!qty.isNull());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
