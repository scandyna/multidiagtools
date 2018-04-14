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
#include "SchemaFieldBenchmark.h"
#include "Mdt/Sql/Schema/Field.h"

using namespace Mdt::Sql::Schema;

void SchemaFieldBenchmark::initTestCase()
{
}

void SchemaFieldBenchmark::cleanupTestCase()
{
}

/*
 * Benchmarks
 */

void SchemaFieldBenchmark::fieldListFieldIndexBenchmark()
{
  const int N = 10;
  FieldList list = getFieldList(N, FieldType::Integer);

  QBENCHMARK{
    for(int i = 0; i < N; ++i){
      QCOMPARE( list.fieldIndex(QString::number(i)) , i );
    }
  }
}

void SchemaFieldBenchmark::fieldListToStringListBenchmark()
{
  const int N = 100;
  FieldList list = getFieldList(N, FieldType::Integer);
  QStringList nameList;

  QBENCHMARK{
    nameList = list.toFieldNameList();
  }
  for(int i = 0; i < N; ++i){
    QCOMPARE(nameList.at(i) , list.at(i).name());
  }
}

/*
 * Helpers
 */

FieldList SchemaFieldBenchmark::getFieldList(int N, FieldType type)
{
  FieldList list;

  for(int i = 0; i < N; ++i){
    Field field;
    field.setType(type);
    field.setName( QString::number(i) );
    list.append(field);
  }

  return list;
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaFieldBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
