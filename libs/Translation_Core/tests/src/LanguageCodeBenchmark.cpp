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
#include "LanguageCodeBenchmark.h"
#include "Mdt/Translation/LanguageCode.h"
#include "Mdt/Translation/LanguageCodeList.h"

using namespace Mdt::Translation;

void LanguageCodeBenchmark::initTestCase()
{
}

void LanguageCodeBenchmark::cleanupTestCase()
{
}

/*
 * Benchmarks
 */

void LanguageCodeBenchmark::toStringBenchmark()
{
  QString result;
  LanguageCode lc("fr");
  QBENCHMARK{
    result = lc.toString();
  }
  QCOMPARE(result, QString("fr"));
}

void LanguageCodeBenchmark::listAddBenchmark()
{
  LanguageCodeList list;

  QBENCHMARK{
    list.addLanguageCode(LanguageCode("fr"));
  }
  QCOMPARE(list.count(), 1);
  QCOMPARE(list.at(0).toString(), QString("fr"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  LanguageCodeBenchmark test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
