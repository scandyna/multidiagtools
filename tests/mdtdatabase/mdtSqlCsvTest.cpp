/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlCsvTest.h"
#include "mdtSqlCsvData.h"
#include "mdtCsvData.h"
#include "mdtCsvRecordFormat.h"
#include "mdtApplication.h"
#include <QTemporaryFile>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QSqlRecord>
#include <QSqlField>
#include <QtTest>
#include <string>
#include <vector>

#include <QDebug>

void mdtSqlCsvTest::fieldFormatTest()
{
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::UnknownType) == QMetaType::UnknownType);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Boolean) == QMetaType::Bool);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Integer) == QMetaType::Int);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Float) == QMetaType::Float);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Double) == QMetaType::Double);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Varchar) == QMetaType::QString);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Date) == QMetaType::QDate);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Time) == QMetaType::QTime);
  QVERIFY(mdtSqlCsvData::csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::DateTime) == QMetaType::QDateTime);
}

void mdtSqlCsvTest::csvSqlRecordFormatTest()
{
  QSqlRecord sqlRecord;

  // Build SQL record
  sqlRecord.clear();
  sqlRecord.append(QSqlField("Id_PK", QVariant::Int));
  sqlRecord.append(QSqlField("Name", QVariant::String));
  // Get format and check
  mdtCsvRecordFormat fmt = mdtSqlCsvData::csvRecordFormatFromQsqlRecord(sqlRecord);
  QCOMPARE(fmt.fieldCount(), sqlRecord.count());
  QVERIFY(fmt.fieldType(0) == QMetaType::Int);
  QVERIFY(fmt.fieldType(1) == QMetaType::QString);
}

void mdtSqlCsvTest::csvSqlRecordTest()
{
  QSqlRecord sqlRecord;
  mdtCsvRecord csvRecord;

  /*
   * Check QSqlRecord to mdtCsvRecord
   */
  // Build SQL record
  sqlRecord.clear();
  auto Id_PK = QSqlField("Id_PK", QVariant::Int);
  Id_PK.setValue(1);
  sqlRecord.append(Id_PK);
  auto Name = QSqlField("Name", QVariant::String);
  Name.setValue("Some name");
  sqlRecord.append(Name);
  // Get CSV record and check
  csvRecord = mdtSqlCsvData::csvRecordFromQsqlRecord(sqlRecord);
  QCOMPARE(csvRecord.count(), sqlRecord.count());
  QCOMPARE(csvRecord.columnDataList.at(0).type(), QVariant::Int);
  QCOMPARE(csvRecord.columnDataList.at(0), QVariant(1));
  QCOMPARE(csvRecord.columnDataList.at(1).type(), QVariant::String);
  QCOMPARE(csvRecord.columnDataList.at(1), QVariant("Some name"));
  /*
   * Check filling QSqlRecord with CSV data
   */
  // Setup a freshly parsed CSV record (all data are strings)
  csvRecord.clear();
  csvRecord.columnDataList << "25" << "Another name";
  // Fill SQL record and check
  QVERIFY(mdtSqlCsvData::fillQSqlRecord(sqlRecord, csvRecord));
  QVERIFY(sqlRecord.field(0).type() == QVariant::Int);
  QCOMPARE(sqlRecord.value(0), QVariant(25));
  QVERIFY(sqlRecord.field(1).type() == QVariant::String);
  QCOMPARE(sqlRecord.value(1), QVariant("Another name"));
  // Check also with data that are not convertible
  csvRecord.clear();
  csvRecord.columnDataList << "Non integral" << "Another name 2";
  QVERIFY(!mdtSqlCsvData::fillQSqlRecord(sqlRecord, csvRecord));
  // Also check that SQL record was not touched
  QCOMPARE(sqlRecord.value(0), QVariant(25));
  QCOMPARE(sqlRecord.value(1), QVariant("Another name"));

}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSqlCsvTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
