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
#include "RecordUtilsTest.h"
#include "Mdt/Sql/RecordUtils.h"

using namespace Mdt::Sql;
using Mdt::Container::VariantRecord;

/*
 * Tests
 */

void RecordUtilsTest::variantRecordFromSqlRecordTest()
{
  QFETCH(VariantRecord, inRecord);

  const QSqlRecord sqlRecord = sqlRecordFromVariantRecord(inRecord);
  const VariantRecord record = variantRecordFromSqlRecord(sqlRecord);
  QCOMPARE(record.columnCount(), inRecord.columnCount());
  const int n = record.columnCount();
  for(int col = 0; col < n; ++col){
    QCOMPARE(record.value(col), inRecord.value(col));
  }
}

void RecordUtilsTest::variantRecordFromSqlRecordTest_data()
{
  QTest::addColumn<VariantRecord>("inRecord");

  QTest::newRow("") << VariantRecord();
  QTest::newRow("A") << VariantRecord({"A"});
  QTest::newRow("1") << VariantRecord({1});
  QTest::newRow("A,B") << VariantRecord({"A","B"});
  QTest::newRow("A,1") << VariantRecord({"A",1});
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RecordUtilsTest test;

  return QTest::qExec(&test, argc, argv);
}

