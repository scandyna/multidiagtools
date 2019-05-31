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
#include "VariantRecordTest.h"
#include "Mdt/Container/VariantRecord.h"

using Mdt::Container::VariantRecord;
/*
 * Tests
 */

void VariantRecordTest::constructTest()
{
  VariantRecord record0;
  QCOMPARE(record0.columnCount(), 0);

  VariantRecord record1(1);
  QCOMPARE(record1.columnCount(), 1);
  QVERIFY(record1.value(0).isNull());

  VariantRecord record2{1, "A"};
  QCOMPARE(record2.columnCount(), 2);
  QCOMPARE(record2.value(0), QVariant(1));
  QCOMPARE(record2.value(1), QVariant("A"));
}

void VariantRecordTest::clearTest()
{
  VariantRecord record{1, "A"};

  record.clear();
  QCOMPARE(record.columnCount(), 0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    VariantRecordTest test;

  return QTest::qExec(&test, argc, argv);
}
