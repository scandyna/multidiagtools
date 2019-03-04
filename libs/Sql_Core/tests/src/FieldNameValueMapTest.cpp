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
#include "FieldNameValueMapTest.h"
#include "Mdt/Sql/FieldNameValueMap.h"
#include <QLatin1String>

using namespace Mdt::Sql;

/*
 * Tests
 */

void FieldNameValueMapTest::addGetTest()
{
  FieldNameValueMap map;
  QCOMPARE(map.size(), 0);
  QVERIFY(map.isEmpty());
  QVERIFY(!map.containsFieldName("A"));
  QCOMPARE(map.value("A"), QVariant());

  map.addValue(FieldName("A"), 1);
  QCOMPARE(map.size(), 1);
  QVERIFY(!map.isEmpty());
  QVERIFY(map.containsFieldName("A"));
  QVERIFY(map.containsFieldName("a"));
  QCOMPARE(map.value("A"), QVariant(1));
  QCOMPARE(map.value("a"), QVariant(1));
  QCOMPARE(map.fieldNameAt(0), QLatin1String("A"));
  QCOMPARE(map.valueAt(0), QVariant(1));

  map.addValue(FieldName("B"), 2);
  QVERIFY(map.containsFieldName("A"));
  QVERIFY(map.containsFieldName("B"));
  QCOMPARE(map.value("A"), QVariant(1));
  QCOMPARE(map.value("B"), QVariant(2));

  map.clear();
  QVERIFY(map.isEmpty());
}

void FieldNameValueMapTest::toFieldNameListTest()
{
  FieldNameValueMap map;

  map.addValue(FieldName("A"), 1);
  QCOMPARE(map.toFieldNameList(), QStringList({"A"}));

  map.addValue(FieldName("C"), 3);
  QCOMPARE(map.toFieldNameList(), QStringList({"A","C"}));

  map.addValue(FieldName("B"), 2);
  QCOMPARE(map.toFieldNameList(), QStringList({"A","C","B"}));
}

void FieldNameValueMapTest::toValueListTest()
{
  FieldNameValueMap map;

  map.addValue(FieldName("A"), 1);
  QCOMPARE(map.toValueList(), QVariantList({1}));

  map.addValue(FieldName("C"), 3);
  QCOMPARE(map.toValueList(), QVariantList({1,3}));

  map.addValue(FieldName("B"), 2);
  QCOMPARE(map.toValueList(), QVariantList({1,3,2}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldNameValueMapTest test;

  return QTest::qExec(&test, argc, argv);
}
