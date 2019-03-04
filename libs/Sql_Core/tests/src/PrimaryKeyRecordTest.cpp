/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "PrimaryKeyRecordTest.h"
#include "Mdt/Sql/PrimaryKeyRecord.h"
#include <QLatin1String>

using namespace Mdt::Sql;

/*
 * Tests
 */

void PrimaryKeyRecordTest::constructTest()
{
  PrimaryKeyRecord record0;
  QCOMPARE(record0.fieldCount(), 0);
  QVERIFY(record0.isNull());
}

void PrimaryKeyRecordTest::addValueTest()
{
  PrimaryKeyRecord record;
  QCOMPARE(record.fieldCount(), 0);
  QVERIFY(record.isNull());
  QCOMPARE(record.value("A"), QVariant());

  record.addValue(FieldName("A"), 1);
  QCOMPARE(record.fieldCount(), 1);
  QVERIFY(!record.isNull());
  QCOMPARE(record.value("A"), QVariant(1));
  QCOMPARE(record.fieldNameAt(0), QLatin1String("A"));
  QCOMPARE(record.valueAt(0), QVariant(1));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyRecordTest test;

  return QTest::qExec(&test, argc, argv);
}
