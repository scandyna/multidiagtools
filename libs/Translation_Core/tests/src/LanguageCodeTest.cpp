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
#include "LanguageCodeTest.h"
#include "Mdt/Translation/LanguageCode.h"
#include "Mdt/Translation/LanguageCodeList.h"
#include <QStringList>

#include <QDebug>

using namespace Mdt::Translation;

void LanguageCodeTest::initTestCase()
{
}

void LanguageCodeTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void LanguageCodeTest::constructTest()
{
  LanguageCode lc0;
  QVERIFY(lc0.isNull());
  QVERIFY(lc0.toString().isEmpty());

  LanguageCode lcFr("fr");
  QVERIFY(!lcFr.isNull());
  QCOMPARE(lcFr.toString(), QString("fr"));
}

void LanguageCodeTest::fromStringTest()
{
  const auto lcFr = LanguageCode::fromString("fr");
  QVERIFY(!lcFr.isNull());
  QCOMPARE(lcFr.toString(), QString("fr"));
}

void LanguageCodeTest::fromLanguageCountryTest()
{
  QFETCH(QString, languageCountry);
  QFETCH(QString, expectedLanguageCode);

  QCOMPARE(LanguageCode::fromLanguageCountry(languageCountry).toString(), expectedLanguageCode);
}

void LanguageCodeTest::fromLanguageCountryTest_data()
{
  QTest::addColumn<QString>("languageCountry");
  QTest::addColumn<QString>("expectedLanguageCode");

  QTest::newRow("fr") << "fr" << "fr";
  QTest::newRow("fr_CH") << "fr_CH" << "fr";
}

void LanguageCodeTest::compareTest()
{
  LanguageCode lcFr1("fr");
  LanguageCode lcFr2("fr");
  LanguageCode lcEn("en");

  QVERIFY(lcFr1 == lcFr2);
  QVERIFY( !(lcFr1 != lcFr2) );
  QVERIFY( !(lcFr1 == lcEn) );
  QVERIFY(lcFr1 != lcEn);
}

void LanguageCodeTest::listAddTest()
{
  LanguageCodeList list;
  QCOMPARE(list.count(), 0);
  QVERIFY(list.isEmpty());
  // Add a language code
  list.addLanguageCode(LanguageCode("fr"));
  QCOMPARE(list.count(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.at(0).toString(), QString("fr"));
  // Try to add the same language code
  list.addLanguageCode(LanguageCode("fr"));
  QCOMPARE(list.count(), 1);
  // Add a other language code
  list.addLanguageCode(LanguageCode("en"));
  QCOMPARE(list.count(), 2);
  QCOMPARE(list.at(0).toString(), QString("fr"));
  QCOMPARE(list.at(1).toString(), QString("en"));
}

void LanguageCodeTest::listToStringListTest()
{
  LanguageCodeList list;
  list.addLanguageCode(LanguageCode("en"));
  list.addLanguageCode(LanguageCode("fr"));
  QCOMPARE( list.toStringList(), QStringList({"en","fr"}) );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  LanguageCodeTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
