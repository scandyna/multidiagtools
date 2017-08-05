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
#include "BoostSpiritTraitsTest.h"
#include "Mdt/PlainText/BoostSpiritQtTraits.h"
#include "Mdt/PlainText/StringConstIterator.h"
#include <boost/spirit/include/qi.hpp>
#include <QString>

using namespace Mdt::PlainText;

void BoostSpiritTraitsTest::initTestCase()
{
}

void BoostSpiritTraitsTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void BoostSpiritTraitsTest::charParserQStringTest()
{
  using boost::spirit::standard_wide::char_;

  QFETCH(QString, sourceData);
  QString outputData;

  StringConstIterator first = sourceData.cbegin();
  StringConstIterator last = sourceData.cend();
  boost::spirit::qi::parse(first, last, *char_, outputData);
  QCOMPARE(outputData, sourceData);
}

void BoostSpiritTraitsTest::charParserQStringTest_data()
{
  QTest::addColumn<QString>("sourceData");

  QTest::newRow("") << "";
  QTest::newRow("A") << "A";
  QTest::newRow("AB") << "AB";
  // Only edit this part with a UTF-8 editor
  QTest::newRow("éöàäèü$£") << "éöàäèü$£";
}

void BoostSpiritTraitsTest::stringParserQStringTest()
{
  using boost::spirit::standard_wide::string;

  QString sourceData;
  QString outputData;

  sourceData = "abc";
  StringConstIterator first = sourceData.cbegin();
  StringConstIterator last = sourceData.cend();
  boost::spirit::qi::parse(first, last, string("abc"), outputData);
  QCOMPARE(outputData, sourceData);

  sourceData = "1234";
  first = sourceData.cbegin();
  last = sourceData.cend();
  outputData.clear();
  boost::spirit::qi::parse(first, last, string("1234"), outputData);
  QCOMPARE(outputData, QString("1234"));

  // Only edit this part with a UTF-8 editor
  sourceData = "éöàäèü$£";
  first = sourceData.cbegin();
  last = sourceData.cend();
  outputData.clear();
  boost::spirit::qi::parse(first, last, string(L"éöàäèü$£"), outputData);
  QCOMPARE(outputData, sourceData);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  BoostSpiritTraitsTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
