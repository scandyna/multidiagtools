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
#include "FieldNameTest.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Sql/FieldNameList.h"

namespace Sql = Mdt::Sql;

void FieldNameTest::initTestCase()
{
}

void FieldNameTest::cleanupTestCase()
{
}

/*
 * Tests
 */

QString fieldNameTestFunction(const Sql::FieldName & fn)
{
  return fn.toString();
}

void FieldNameTest::fieldNameTest()
{
  using Sql::FieldName;
  using Sql::Schema::Field;
  using Sql::Schema::AutoIncrementPrimaryKey;

  // Must not compile
//   QCOMPARE(fieldNameTestFunction("Bug"), QString("Bug"));

  // From explicit field name
  QCOMPARE(fieldNameTestFunction(FieldName("A")), QString("A"));
  // From a field
  Field B;
  B.setName("B");
  QCOMPARE(fieldNameTestFunction(B), QString("B"));
  // From a AI PK
  AutoIncrementPrimaryKey Id_PK("Id_PK");
  QCOMPARE(fieldNameTestFunction(Id_PK), QString("Id_PK"));
  // Null flag
  QVERIFY(FieldName("").isNull());
  QVERIFY(!FieldName("C").isNull());
}

void FieldNameTest::fieldNameListTest()
{
  using Sql::FieldNameList;
  using Sql::FieldName;
  using Sql::Schema::AutoIncrementPrimaryKey;
  using Sql::Schema::Field;

  Field field;

  FieldNameList list1;
  QCOMPARE(list1.size(), 0);

  FieldNameList list2( FieldName("A") );
  QCOMPARE(list2.size(), 1);
  QCOMPARE(list2.at(0), QString("A"));

  FieldNameList list3( AutoIncrementPrimaryKey("B") );
  QCOMPARE(list3.size(), 1);
  QCOMPARE(list3.at(0), QString("B"));

  FieldNameList list4( FieldName("C") , FieldName("D") );
  QCOMPARE(list4.size(), 2);
  QCOMPARE(list4.at(0), QString("C"));
  QCOMPARE(list4.at(1), QString("D"));

  field.setName("E");
  FieldNameList list5(field);
  QCOMPARE(list5.size() , 1);
  QCOMPARE(list5.at(0), QString("E"));

  field.setName("F");
  FieldNameList list6(field, FieldName("G"));
  QCOMPARE(list6.size() , 2);
  QCOMPARE(list6.at(0), QString("F"));
  QCOMPARE(list6.at(1), QString("G"));

  field.setName("I");
  FieldNameList list7( AutoIncrementPrimaryKey("H"), field, FieldName("J") );
  QCOMPARE(list7.size() , 3);
  QCOMPARE(list7.at(0), QString("H"));
  QCOMPARE(list7.at(1), QString("I"));
  QCOMPARE(list7.at(2), QString("J"));

  // Must not compile
//   FieldNameList bugList("l");
//   FieldNameList bugList(2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FieldNameTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
