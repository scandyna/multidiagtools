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
#include "FieldTest.h"
#include "Mdt/Entity/Field.h"
#include <QString>

#include <QDebug>

using namespace Mdt::Entity;

void FieldTest::initTestCase()
{
}

void FieldTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FieldTest::constructTest()
{
  QString str;
  /*
   * Default constructed
   */
  MDT_ENTITY_FIELD(QString, field0)
  QCOMPARE(field0.fieldName(), QString("field0"));
  str = field0;
  QVERIFY(str.isEmpty());
  /*
   * Construct with a value
   */
  MDT_ENTITY_FIELD(QString, field1, "Value 1")
  QCOMPARE(field1.fieldName(), QString("field1"));
  str = field1;
  QCOMPARE(str, QString("Value 1"));
  /*
   * Copy construct
   */
  auto field11 = field1;
  QCOMPARE(field11.fieldName(), QString("field1"));
  str = field11;
  QCOMPARE(str, QString("Value 1"));
  /*
   * Move construct
   */
  {
    MDT_ENTITY_FIELD(QString, tmpField, "Value tmp")
    auto field12 = std::move(tmpField);
    QCOMPARE(field12.fieldName(), QString("tmpField"));
    str = field12;
    QCOMPARE(str, QString("Value tmp"));
  }
}

void FieldTest::assignTest()
{
  QString str;
  /*
   * Copy assign
   */
  MDT_ENTITY_FIELD(QString, field1, "Value 1")
  QCOMPARE(field1.fieldName(), QString("field1"));
  str = field1;
  QCOMPARE(str, QString("Value 1"));
  // Create field and use it (avoid call of copy constructor)
  MDT_ENTITY_FIELD(QString, field11)
  str = field11;
  QVERIFY(str.isEmpty());
  field11 = field1;
  str = field11;
  QCOMPARE(str, QString("Value 1"));
  /*
   * Try move assign
   */
  {
    MDT_ENTITY_FIELD(QString, tmpField, "Value tmp")
    field11 = std::move(tmpField);
    str = field11;
    QCOMPARE(str, QString("Value tmp"));
  }
  /*
   * Assign values
   */
  // Copy assign
  QString value2 = "Value 2";
  field1 = value2;
  str = field1;
  QCOMPARE(str, QString("Value 2"));
  // Move assign
  field1 = std::move(QString::fromUtf8("Value 3"));
  str = field1;
  QCOMPARE(str, QString("Value 3"));
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
