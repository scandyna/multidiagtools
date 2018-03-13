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
#include "FieldAttributesTest.h"
#include "Mdt/Entity/FieldAttributes.h"

using namespace Mdt::Entity;

void FieldAttributesTest::initTestCase()
{
}

void FieldAttributesTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FieldAttributesTest::fieldMaxLengthTest()
{
  FieldMaxLength ml0;
  QCOMPARE(ml0.length(), 0);
  QVERIFY(ml0.isNull());

  FieldMaxLength ml1(1);
  QCOMPARE(ml1.length(), 1);
  QVERIFY(!ml1.isNull());
}

void FieldAttributesTest::attributesBasicTest()
{
  FieldAttributes fa0;
  QVERIFY(fa0.isNull());
  QVERIFY(!fa0.isRequired());
  QVERIFY(!fa0.isUnique());
  QVERIFY(!fa0.isPrimaryKey());
  QVERIFY(!fa0.hasDefaultValue());
  QCOMPARE(fa0.maxLength(), 0);
}

void FieldAttributesTest::attributesVariadicConstructTest()
{
  FieldAttributes fa1(FieldFlag::IsRequired);
  QVERIFY(!fa1.isNull());
  QVERIFY(fa1.isRequired());
  QVERIFY(!fa1.isUnique());
  QVERIFY(!fa1.isPrimaryKey());
  QVERIFY(!fa1.hasDefaultValue());
  QCOMPARE(fa1.maxLength(), 0);

  FieldAttributes fa2(FieldMaxLength(250));
  QVERIFY(!fa2.isNull());
  QVERIFY(!fa2.isRequired());
  QVERIFY(!fa2.isUnique());
  QVERIFY(!fa2.isPrimaryKey());
  QVERIFY(!fa2.hasDefaultValue());
  QCOMPARE(fa2.maxLength(), 250);

  FieldAttributes fa3(FieldFlag::IsUnique, FieldMaxLength(12));
  QVERIFY(!fa3.isNull());
  QVERIFY(!fa3.isRequired());
  QVERIFY(fa3.isUnique());
  QVERIFY(!fa3.isPrimaryKey());
  QVERIFY(!fa3.hasDefaultValue());
  QCOMPARE(fa3.maxLength(), 12);

  FieldAttributes fa4(FieldMaxLength(15), FieldFlag::IsRequired);
  QVERIFY(!fa4.isNull());
  QVERIFY(fa4.isRequired());
  QVERIFY(!fa4.isUnique());
  QVERIFY(!fa4.isPrimaryKey());
  QVERIFY(!fa4.hasDefaultValue());
  QCOMPARE(fa4.maxLength(), 15);

  FieldAttributes fa5(FieldFlag::IsUnique | FieldFlag::IsRequired, FieldMaxLength(367));
  QVERIFY(!fa5.isNull());
  QVERIFY(fa5.isRequired());
  QVERIFY(fa5.isUnique());
  QVERIFY(!fa5.isPrimaryKey());
  QVERIFY(!fa5.hasDefaultValue());
  QCOMPARE(fa5.maxLength(), 367);

  FieldAttributes fa6(FieldFlag::IsPrimaryKey);
  QVERIFY(!fa6.isNull());
  QVERIFY(!fa6.isRequired());
  QVERIFY(!fa6.isUnique());
  QVERIFY(fa6.isPrimaryKey());
  QVERIFY(!fa6.hasDefaultValue());
  QCOMPARE(fa6.maxLength(), 0);

  FieldAttributes fa7(FieldFlag::HasDefaultValue);
  QVERIFY(!fa7.isNull());
  QVERIFY(!fa7.isRequired());
  QVERIFY(!fa7.isUnique());
  QVERIFY(!fa7.isPrimaryKey());
  QVERIFY(fa7.hasDefaultValue());
  QCOMPARE(fa7.maxLength(), 0);

  /*
   * Attributes are mostly called with comas ',' only,
   * including for flags. Check that this works.
   */
  FieldAttributes fa8(FieldFlag::IsPrimaryKey, FieldFlag::IsUnique);
  QVERIFY(!fa8.isNull());
  QVERIFY(!fa8.isRequired());
  QVERIFY(fa8.isUnique());
  QVERIFY(fa8.isPrimaryKey());
  QVERIFY(!fa8.hasDefaultValue());
  QCOMPARE(fa8.maxLength(), 0);

  FieldAttributes fa9(FieldFlag::IsPrimaryKey, FieldMaxLength(20), FieldFlag::IsRequired);
  QVERIFY(!fa9.isNull());
  QVERIFY(fa9.isRequired());
  QVERIFY(!fa9.isUnique());
  QVERIFY(fa9.isPrimaryKey());
  QVERIFY(!fa9.hasDefaultValue());
  QCOMPARE(fa9.maxLength(), 20);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldAttributesTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
