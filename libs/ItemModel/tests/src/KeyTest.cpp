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
#include "KeyTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/RowList.h"
#include "Mdt/ItemModel/ColumnList.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/ItemModel/ForeignKeyList.h"
#include "Mdt/ItemModel/KeyData.h"
#include "Mdt/ItemModel/KeyRecord.h"
#include "Mdt/ItemModel/PrimaryKeyRecord.h"
#include "Mdt/ItemModel/ForeignKeyRecord.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QModelIndex>
#include <QModelIndexList>
#include <QVector>
#include <QPair>

using namespace Mdt::ItemModel;

using RowColumnPairList = QVector< QPair<int,int> >;

void KeyTest::initTestCase()
{
}

void KeyTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void KeyTest::rowListTest()
{
  /*
   * Construct a empty list
   */
  RowList list1;
  QCOMPARE(list1.size(), 0);
  QVERIFY(list1.isEmpty());
  QVERIFY(list1.begin() == list1.end());
  QVERIFY(!list1.contains(0));
  QCOMPARE(list1.greatestRow(), -1);
  /*
   * Construct list
   */
  RowList list2({0,3,2});
  QCOMPARE(list2.size(), 3);
  QVERIFY(!list2.isEmpty());
  QCOMPARE(list2.at(1), 3);
  QVERIFY(list2.begin() != list2.end());
  QCOMPARE(list2.greatestRow(), 3);
  /*
   * Check contains
   */
  QVERIFY(list2.contains(0));
  QVERIFY(!list2.contains(1));
  QVERIFY(list2.contains(2));
  QVERIFY(list2.contains(3));
  QVERIFY(!list2.contains(4));
  /*
   * Clear
   */
  list2.clear();
  QCOMPARE(list2.size(), 0);
  QVERIFY(list2.isEmpty());
  /*
   * Append
   */
  list2.append(1);
  QCOMPARE(list2.size(), 1);
  QCOMPARE(list2.at(0), 1);
}

void KeyTest::rowListFromModelIndexListTest()
{
  QFETCH(RowColumnPairList , rowColumnPairList);
  QFETCH(RowList , expectedRowList);
  VariantTableModel model;
  /*
   * Build model index list
   */
  QModelIndexList modelIndexList;
  for(const auto pair : rowColumnPairList){
    if( (model.rowCount() <= pair.first) || (model.columnCount() <= pair.second) ){
      model.resize(pair.first+1, pair.second+1);
    }
    const auto index = model.index(pair.first, pair.second);
    QVERIFY(index.isValid());
    modelIndexList.append(index);
  }
  // Add a invalid index
  if(!modelIndexList.isEmpty()){
    modelIndexList.append(QModelIndex());
  }
  /*
   * Check
   */
  const auto rowList = RowList::fromModelIndexList(modelIndexList);
  QCOMPARE(rowList.size(), expectedRowList.size());
  for(int i = 0; i < rowList.size(); ++i){
    QCOMPARE(rowList.at(i), expectedRowList.at(i));
  }
}

void KeyTest::rowListFromModelIndexListTest_data()
{
  QTest::addColumn<RowColumnPairList>("rowColumnPairList");
  QTest::addColumn<RowList>("expectedRowList");

  QTest::newRow("") \
    << RowColumnPairList({}) \
    << RowList{} ;
  QTest::newRow("Simple1") \
    << RowColumnPairList({qMakePair(1,2)}) \
    << RowList{1} ;
  QTest::newRow("SimpleM") \
    << RowColumnPairList({qMakePair(1,2),qMakePair(3,4),qMakePair(5,6)}) \
    << RowList{1,3,5} ;
  QTest::newRow("UniqueTest") \
    << RowColumnPairList({qMakePair(1,2),qMakePair(1,4),qMakePair(5,6)}) \
    << RowList{1,5} ;
}

void KeyTest::rowListFromModelIndexListBenchmark()
{
  QFETCH(int, rowCount);
  QFETCH(int, columnCount);
  VariantTableModel model;
  model.resize(rowCount, columnCount);
  QModelIndexList modelIndexList;

  for(int row = 0; row < rowCount; ++row){
    for(int column = 0; column < columnCount; ++column){
      auto index = model.index(row, column);
      QVERIFY(index.isValid());
      modelIndexList.append(index);
    }
  }
  QBENCHMARK{
    QCOMPARE(RowList::fromModelIndexList(modelIndexList).size(), rowCount);
  }
}

void KeyTest::rowListFromModelIndexListBenchmark_data()
{
  QTest::addColumn<int>("rowCount");
  QTest::addColumn<int>("columnCount");

  QTest::newRow("1x1") << 1 << 1;
  QTest::newRow("100x100") << 100 << 100;
}

void KeyTest::columnListTest()
{
  /*
   * Construct a empty list
   */
  ColumnList list1;
  QCOMPARE(list1.size(), 0);
  QVERIFY(list1.isEmpty());
  QVERIFY(list1.begin() == list1.end());
  QVERIFY(!list1.contains(0));
  QCOMPARE(list1.greatestColumn(), -1);
  /*
   * Construct list
   */
  ColumnList list2({0,3,2});
  QCOMPARE(list2.size(), 3);
  QVERIFY(!list2.isEmpty());
  QCOMPARE(list2.at(1), 3);
  QVERIFY(list2.begin() != list2.end());
  QCOMPARE(list2.greatestColumn(), 3);
  /*
   * Check contains
   */
  QVERIFY(list2.contains(0));
  QVERIFY(!list2.contains(1));
  QVERIFY(list2.contains(2));
  QVERIFY(list2.contains(3));
  QVERIFY(!list2.contains(4));
  /*
   * Clear
   */
  list2.clear();
  QCOMPARE(list2.size(), 0);
  QVERIFY(list2.isEmpty());
  /*
   * Append
   */
  list2.append(1);
  QCOMPARE(list2.size(), 1);
  QCOMPARE(list2.at(0), 1);
}

void KeyTest::columnListQVariantTest()
{
  // Copy
  ColumnList list({1,2});
  QVariant v1;
  v1.setValue(list);
  QCOMPARE(v1.value<ColumnList>().size(), 2);
  QCOMPARE(v1.value<ColumnList>().at(0), 1);
  QCOMPARE(v1.value<ColumnList>().at(1), 2);
  // Move (mybe)
  QVariant v2;
  v2.setValue( ColumnList({3,4}) );
  QCOMPARE(v2.value<ColumnList>().size(), 2);
  QCOMPARE(v2.value<ColumnList>().at(0), 3);
  QCOMPARE(v2.value<ColumnList>().at(1), 4);
  // Copy variant
  QVariant v3 = v2;
  QCOMPARE(v3.value<ColumnList>().size(), 2);
  QCOMPARE(v3.value<ColumnList>().at(0), 3);
  QCOMPARE(v3.value<ColumnList>().at(1), 4);
}

void KeyTest::primaryKeyTest()
{
  /*
   * Construct a null primary key
   */
  PrimaryKey pk1;
  QCOMPARE(pk1.columnCount(), 0);
  QVERIFY(pk1.isNull());
  QCOMPARE(pk1.greatestColumn(), -1);
  QVERIFY(pk1.begin() == pk1.end());
  /*
   * Construct a primary key
   */
  PrimaryKey pk2({0,4,1});
  QCOMPARE(pk2.columnCount(), 3);
  QVERIFY(!pk2.isNull());
  QCOMPARE(pk2.greatestColumn(), 4);
  QVERIFY(pk2.begin() != pk2.end());
  /*
   * Clear
   */
  pk2.clear();
  QVERIFY(pk2.isNull());
  /*
   * Construct from a ColumnList
   */
  ColumnList cl({1,2});
  auto pk3 = PrimaryKey::fromColumnList(cl);
  QCOMPARE(pk3.columnCount(), 2);
}

void KeyTest::primaryKeyQVariantTest()
{
  // Copy
  PrimaryKey pk({1,2});
  QVariant v1;
  v1.setValue(pk);
  QCOMPARE(v1.value<PrimaryKey>().greatestColumn(), 2);
  // Move (mybe)
  QVariant v2;
  v2.setValue( PrimaryKey({3,4}) );
  QCOMPARE(v2.value<PrimaryKey>().greatestColumn(), 4);
  // Copy variant
  QVariant v3 = v2;
  QCOMPARE(v3.value<PrimaryKey>().greatestColumn(), 4);
}

void KeyTest::foreignKeyTest()
{
  /*
   * Contruct a null foreign key
   */
  ForeignKey fk1;
  QCOMPARE(fk1.columnCount(), 0);
  QVERIFY(fk1.isNull());
  QCOMPARE(fk1.greatestColumn(), -1);
  QVERIFY(fk1.begin() == fk1.end());
  /*
   * Contruct a foreign key
   */
  ForeignKey fk2({1,3});
  QCOMPARE(fk2.columnCount(), 2);
  QVERIFY(!fk2.isNull());
  QCOMPARE(fk2.greatestColumn(), 3);
  QVERIFY(fk2.begin() != fk2.end());
  /*
   * Clear
   */
  fk2.clear();
  QVERIFY(fk2.isNull());
  /*
   * Construct from a ColumnList
   */
  ColumnList cl({1,2});
  auto fk3 = ForeignKey::fromColumnList(cl);
  QCOMPARE(fk3.columnCount(), 2);
}

void KeyTest::foreignKeyListTest()
{
  ForeignKey fk;
  /*
   * Initial state
   */
  ForeignKeyList list;
  QVERIFY(list.getForeignKeyReferencing("A").isNull());
  /*
   * Add a foreign key
   */
  list.addForeignKey("A", ForeignKey({1,2}) );
  fk = list.getForeignKeyReferencing("A");
  QCOMPARE(fk.columnCount(), 2);
}

void KeyTest::keyDataTest()
{
  KeyData data(1, "A");
  QCOMPARE(data.column(), 1);
  QCOMPARE(data.data(), QVariant("A"));
}

void KeyTest::keyRecordTest()
{
  /*
   * Initial state
   */
  KeyRecord record;
  QCOMPARE(record.columnCount(), 0);
  QVERIFY(record.isEmpty());
  QVERIFY(record.dataForColumn(0).isNull());
  /*
   * Append
   */
  record.append(1, "A");
  QCOMPARE(record.columnCount(), 1);
  QVERIFY(!record.isEmpty());
  QCOMPARE(record.columnAt(0), 1);
  QCOMPARE(record.dataAt(0), QVariant("A"));
  QVERIFY(record.dataForColumn(0).isNull());
  QCOMPARE(record.dataForColumn(1), QVariant("A"));
  /*
   * Clear
   */
  record.clear();
  QCOMPARE(record.columnCount(), 0);
  QVERIFY(record.isEmpty());
}

void KeyTest::primaryKeyRecordTest()
{
  PrimaryKeyRecord record;
  QVERIFY(record.isEmpty());
  record.append(2, "B");
  QVERIFY(!record.isEmpty());
  /*
   * Check casting from KeyRecord
   */
  KeyRecord kr;
  kr.append(3, "C");
  record = PrimaryKeyRecord::fromKeyRecord(kr);
  QCOMPARE(record.columnCount(), 1);
  QCOMPARE(record.columnAt(0), 3);
}

void KeyTest::foreignKeyRecordTest()
{
  ForeignKeyRecord record;
  QVERIFY(record.isEmpty());
  record.append(2, "B");
  QVERIFY(!record.isEmpty());
  /*
   * Check casting from KeyRecord
   */
  KeyRecord kr;
  kr.append(3, "C");
  record = ForeignKeyRecord::fromKeyRecord(kr);
  QCOMPARE(record.columnCount(), 1);
  QCOMPARE(record.columnAt(0), 3);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  KeyTest test;

  return QTest::qExec(&test, argc, argv);
}
