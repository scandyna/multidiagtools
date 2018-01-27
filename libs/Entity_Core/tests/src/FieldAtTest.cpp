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
#include "FieldAtTest.h"
#include "Mdt/Entity/FieldAt.h"
#include "Mdt/Entity/Def.h"

using namespace Mdt::Entity;

void FieldAtTest::initTestCase()
{
}

void FieldAtTest::cleanupTestCase()
{
}

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
  double qty;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired | FieldFlag::IsUnique),
  (description, FieldMaxLength(250)),
  (qty)
)

/*
 * Tests
 */

void FieldAtTest::fieldCountTest()
{
  ArticleDef articleDef;

  QCOMPARE(fieldCount(articleDef), 3);
  QCOMPARE(fieldCount<ArticleDef>(), 3);
}

void FieldAtTest::fieldNameAtTest()
{
  ArticleDef articleDef;

  QCOMPARE(fieldNameAt(articleDef, 0), QString("id"));
  QCOMPARE(fieldNameAt(articleDef, 1), QString("description"));
  QCOMPARE(fieldNameAt(articleDef, 2), QString("qty"));
}

void FieldAtTest::fieldAttributesAtTest()
{
  ArticleDef articleDef;

  const auto idAttributes = fieldAttributesAt(articleDef, 0);
  QVERIFY(idAttributes.isRequired());
  QVERIFY(idAttributes.isUnique());
  QVERIFY(idAttributes.maxLength() < 1);

  const auto descriptionAttributes = fieldAttributesAt(articleDef, 1);
  QVERIFY(!descriptionAttributes.isRequired());
  QVERIFY(!descriptionAttributes.isUnique());
  QCOMPARE(descriptionAttributes.maxLength(), 250);

  const auto qtyAttributes = fieldAttributesAt(articleDef, 2);
  QVERIFY(!qtyAttributes.isRequired());
  QVERIFY(!qtyAttributes.isUnique());
  QVERIFY(qtyAttributes.maxLength() < 1);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldAtTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
