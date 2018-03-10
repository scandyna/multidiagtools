/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "FieldDefTest.h"
#include "Mdt/Entity/FieldDef.h"

/*
 * Note: to check that the macro works properly,
 * we not declare using namespace Mdt::Entity
 */

void FieldDefTest::initTestCase()
{
}

void FieldDefTest::cleanupTestCase()
{
}

/*
 * Tests
 */

MDT_ENTITY_FIELD_DEF( (field0) )
MDT_ENTITY_FIELD_DEF_INSTANCE( (field0) )

MDT_ENTITY_FIELD_DEF( (field1, FieldFlag::IsRequired) )
MDT_ENTITY_FIELD_DEF_INSTANCE( (field1, FieldFlag::IsRequired) )

MDT_ENTITY_FIELD_DEF( (field2, FieldFlag::IsRequired | FieldFlag::IsUnique, FieldMaxLength(250)) )
MDT_ENTITY_FIELD_DEF_INSTANCE( (field2) )

namespace A{
  MDT_ENTITY_FIELD_DEF( (field3) )
  MDT_ENTITY_FIELD_DEF_INSTANCE( (field3) )
}

MDT_ENTITY_FIELD_DEF( (field4) )
MDT_ENTITY_FIELD_DEF_GENERATE_INSTANCE_FUNCTION( (field4) )

struct S5Def
{
  MDT_ENTITY_FIELD_DEF( (field5, FieldFlag::IsUnique, FieldMaxLength(12)) )
  MDT_ENTITY_FIELD_DEF_GENERATE_MEMBER_VARIABLE( (field5) )
  MDT_ENTITY_FIELD_DEF_GENERATE_INSTANCE_FUNCTION( (field5) )
};

void FieldDefTest::defUsageTest()
{
  QCOMPARE(field0.fieldName(), QString("field0"));
  QVERIFY(!field0.fieldAttributes().isRequired());
  QVERIFY(!field0.fieldAttributes().isUnique());
  QCOMPARE(field0.fieldAttributes().maxLength(), 0);

  QCOMPARE(field1.fieldName(), QString("field1"));
  QVERIFY(field1.fieldAttributes().isRequired());
  QVERIFY(!field1.fieldAttributes().isUnique());
  QCOMPARE(field1.fieldAttributes().maxLength(), 0);

  QCOMPARE(field2.fieldName(), QString("field2"));
  QVERIFY(field2.fieldAttributes().isRequired());
  QVERIFY(field2.fieldAttributes().isUnique());
  QCOMPARE(field2.fieldAttributes().maxLength(), 250);

  QCOMPARE(A::field3.fieldName(), QString("field3"));

  QCOMPARE(field4().fieldName(), QString("field4"));
  QVERIFY(!field4().fieldAttributes().isRequired());
  QVERIFY(!field4().fieldAttributes().isUnique());
  QCOMPARE(field4().fieldAttributes().maxLength(), 0);

  S5Def s5;
  constexpr auto f5 = s5.field5();
  QCOMPARE(f5.fieldName(), QString("field5"));
  constexpr auto fa5 = s5.field5().fieldAttributes();
  QVERIFY(!fa5.isRequired());
  QVERIFY(fa5.isUnique());
  QCOMPARE(fa5.maxLength(), 12);
  const auto mf5 = s5.m_field5;
  QCOMPARE(mf5.fieldName(), QString("field5"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldDefTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
