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
#include "SettingsTest.h"
#include "Mdt/PlainText/CsvCommonSettings.h"
#include "Mdt/PlainText/CsvParserSettings.h"

using namespace Mdt::PlainText;

void SettingsTest::initTestCase()
{
}

void SettingsTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SettingsTest::csvCommonSettingsTest()
{
  CsvCommonSettings s;
  /*
   * Initial state
   */
  QCOMPARE(s.fieldSeparator(), ',');
  QCOMPARE(s.fieldProtection(), '\"');
  QVERIFY(s.isValid());
  /*
   * Set/get
   */
  s.setFieldSeparator(';');
  s.setFieldProtection('\'');
  QCOMPARE(s.fieldSeparator(), ';');
  QCOMPARE(s.fieldProtection(), '\'');
  /*
   * Validity
   */
  // Wrong setup
  s.setFieldSeparator(',');
  s.setFieldProtection(',');
  QVERIFY(!s.isValid());
  // Good setup
  s.setFieldSeparator(';');
  s.setFieldProtection('\'');
  QVERIFY(s.isValid());
  /*
   * Clear
   */
  s.clear();
  QCOMPARE(s.fieldSeparator(), ',');
  QCOMPARE(s.fieldProtection(), '\"');
  QVERIFY(s.isValid());
}

void SettingsTest::csvParserSettingsTest()
{
  CsvParserSettings s;
  /*
   * Initial state
   */
  QCOMPARE(s.fieldSeparator(), ',');
  QCOMPARE(s.fieldProtection(), '\"');
  QCOMPARE(s.parseExp(), true);
  QVERIFY(s.isValid());
  /*
   * Set/get
   */
  s.setParseExp(false);
  QVERIFY(!s.parseExp());
  s.setParseExp(true);
  QVERIFY(s.parseExp());
  /*
   * Validity
   */
  // Wrong setup
  s.setFieldSeparator(',');
  s.setFieldProtection('~');
  s.setParseExp(true);
  QVERIFY(!s.isValid());
  // Wrong setup
  s.setFieldSeparator('~');
  s.setFieldProtection('\'');
  s.setParseExp(true);
  QVERIFY(!s.isValid());
  // Good setup
  s.setFieldSeparator(';');
  s.setFieldProtection('\'');
  s.setParseExp(true);
  QVERIFY(s.isValid());
  /*
   * Clear
   */
  s.clear();
  QCOMPARE(s.fieldSeparator(), ',');
  QCOMPARE(s.fieldProtection(), '\"');
  QCOMPARE(s.parseExp(), true);
  QVERIFY(s.isValid());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SettingsTest test;

  return QTest::qExec(&test, argc, argv);
}
