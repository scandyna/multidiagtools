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
#include "FieldNameValueMapBenchmark.h"
#include "Mdt/Sql/FieldNameValueMap.h"

using namespace Mdt::Sql;

/*
 * Benchmarks
 */

void FieldNameValueMapBenchmark::addValueBenchmark()
{
  FieldNameValueMap map;

  QBENCHMARK_ONCE{
    map.addValue(FieldName("FirstName"), "A");
  }
  QCOMPARE(map.value("FirstName"), QVariant("A"));
}

void FieldNameValueMapBenchmark::valueBenchmark()
{
  FieldNameValueMap map;
  QVariant value;

  map.addValue(FieldName("FirstName"), "A");
  QBENCHMARK{
    value = map.value("FirstName");
  }
  QCOMPARE(value, QVariant("A"));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldNameValueMapBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
