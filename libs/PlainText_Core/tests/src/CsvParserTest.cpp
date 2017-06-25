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
#include "CsvParserTest.h"
#include "Mdt/PlainText/CsvStringParser.h"

using namespace Mdt::PlainText;

void CsvParserTest::initTestCase()
{
}

void CsvParserTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void CsvParserTest::stringParserReadLineTest()
{
  QFAIL("Not complete");
}

void CsvParserTest::stringParserReadLineTest_data()
{
  buildParserTestData();
}

void CsvParserTest::stringParserReadAllTest()
{
  QFAIL("Not complete");
}

void CsvParserTest::stringParserReadAllTest_data()
{
  buildParserTestData();
}

void CsvParserTest::buildParserTestData()
{

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  CsvParserTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
