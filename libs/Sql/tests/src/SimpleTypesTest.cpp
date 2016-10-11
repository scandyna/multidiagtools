/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "SimpleTypesTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Sql/TableName.h"

void SimpleTypesTest::initTestCase()
{
}

void SimpleTypesTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SimpleTypesTest::fieldNameTest()
{
  using Mdt::Sql::FieldName;

  FieldName fieldA("A");
  QCOMPARE(fieldA.toString(), QString("A"));
}

void SimpleTypesTest::tableNameTest()
{
  using Mdt::Sql::TableName;

  TableName tableA("A");
  QCOMPARE(tableA.toString(), QString("A"));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SimpleTypesTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
