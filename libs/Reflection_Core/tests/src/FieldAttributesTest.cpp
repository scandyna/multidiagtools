/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "Mdt/Reflection/FieldAttributes.h"

using namespace Mdt::Reflection;

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
  QVERIFY(!fa0.hasDefaultValue());
  QCOMPARE(fa0.maxLength(), 0);
}

void FieldAttributesTest::attributesVariadicConstructTest()
{
  FieldAttributes fa1(FieldFlag::IsRequired);
  QVERIFY(!fa1.isNull());
  QVERIFY(fa1.isRequired());
  QVERIFY(!fa1.hasDefaultValue());
  QCOMPARE(fa1.maxLength(), 0);

  FieldAttributes fa2(FieldMaxLength(250));
  QVERIFY(!fa2.isNull());
  QVERIFY(!fa2.isRequired());
  QVERIFY(!fa2.hasDefaultValue());
  QCOMPARE(fa2.maxLength(), 250);

  FieldAttributes fa3(FieldMaxLength(12));
  QVERIFY(!fa3.isNull());
  QVERIFY(!fa3.isRequired());
  QVERIFY(!fa3.hasDefaultValue());
  QCOMPARE(fa3.maxLength(), 12);

  FieldAttributes fa4(FieldMaxLength(15), FieldFlag::IsRequired);
  QVERIFY(!fa4.isNull());
  QVERIFY(fa4.isRequired());
  QVERIFY(!fa4.hasDefaultValue());
  QCOMPARE(fa4.maxLength(), 15);

  FieldAttributes fa5(FieldFlag::IsRequired, FieldMaxLength(367));
  QVERIFY(!fa5.isNull());
  QVERIFY(fa5.isRequired());
  QVERIFY(!fa5.hasDefaultValue());
  QCOMPARE(fa5.maxLength(), 367);

  FieldAttributes fa7(FieldFlag::HasDefaultValue);
  QVERIFY(!fa7.isNull());
  QVERIFY(!fa7.isRequired());
  QVERIFY(fa7.hasDefaultValue());
  QCOMPARE(fa7.maxLength(), 0);

  /*
   * Attributes are mostly called with comas ',' only,
   * including for flags. Check that this works.
   */
  FieldAttributes fa9(FieldMaxLength(20), FieldFlag::IsRequired);
  QVERIFY(!fa9.isNull());
  QVERIFY(fa9.isRequired());
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
