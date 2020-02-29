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
#include "RemoveRowsTest.h"
#include "PersonTableModel.h"
#include "Mdt/ItemModel/RemoveRows.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/AbstractCachedTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/Container/TableCache.h"
#include "Mdt/IndexRange/RowRange.h"
#include <QStringList>
#include <QStringListModel>
#include <QVariant>
#include <array>
#include <initializer_list>

using namespace Mdt::ItemModel;
using namespace Mdt::TestLib;
using Mdt::Container::TableCacheOperation;

/*
 * Test class
 */

class CachedTableModel : public Mdt::ItemModel::AbstractCachedTableModel
{
  using ParentClass = Mdt::ItemModel::AbstractCachedTableModel;

 public:

  using Record = std::array<QVariant, 2>;

  CachedTableModel(QObject* parent = nullptr)
   : AbstractCachedTableModel(parent)
  {
    setModelEditable(true);
  }

  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override
  {
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(row >= 0);
    Q_ASSERT(count >= 1);
    Q_ASSERT( (row+count) <= rowCount() );

    mCache.removeRecords(row, count);

    return true;
  }

  void cancelRemoveRows(int row, int count) override
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(count >= 1);
    Q_ASSERT( (row+count) <= rowCount() );

    mCache.cancelRemoveRecords(row, count);
  }

  void populate(std::initializer_list<Record> initTable)
  {
    beginResetModel();
    mCache.clear();
    for(const auto & record : initTable){
      mCache.appendRecordFromStorage(record);
    }
    endResetModel();
  }

  Mdt::Container::TableCacheOperation operationAtRow(int row) const override
  {
    return mCache.operationAtRow(row);
  }

 private:

  int cachedRowCount() const override
  {
    return mCache.rowCount();
  }

  int columnCountImpl() const override
  {
    return 2;
  }

  QVariant displayRoleData(int row, int column) const override
  {
    return mCache.constRecordAt(row)[column];
  }

  Mdt::Container::TableCache<Record> mCache;
};

/*
 * Tests
 */

void RemoveRowsTest::removeRowsRangeTest()
{
  QStringListModel model(QStringList{"A","B","C","D"});
  RowRange r;

//   QVERIFY( removeRows(&model, r) );
//   QCOMPARE(model.stringList(), QStringList({"A","B","C","D"}));

  r.setFirstRow(1);
  r.setLastRow(2);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.stringList(), QStringList({"A","D"}));

  r.setFirstRow(1);
  r.setLastRow(1);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.stringList(), QStringList({"A"}));

  r.setFirstRow(0);
  r.setLastRow(0);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.stringList(), QStringList());
}

void RemoveRowsTest::removeRowsSelectionTest()
{
  QStringListModel model(QStringList{"A","B","C","D","E","F"});
  RowSelection s;
  RowRange r;

  s.clear();
  r.setFirstRow(4);
  r.setLastRow(5);
  s.appendRange(r);
  QVERIFY( removeRows(&model, s) );
  QCOMPARE(model.stringList(), QStringList({"A","B","C","D"}));

  s.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  s.appendRange(r);
  r.setFirstRow(3);
  r.setLastRow(3);
  s.appendRange(r);
  QVERIFY( removeRows(&model, s) );
  QCOMPARE(model.stringList(), QStringList({"C"}));

  s.clear();
  r.setFirstRow(0);
  r.setLastRow(0);
  s.appendRange(r);
  QVERIFY( removeRows(&model, s) );
  QCOMPARE(model.stringList(), QStringList());
}

void RemoveRowsTest::removeRowsSelectionModelTest()
{
  QStringListModel model(QStringList{"A","B","C","D","E"});
  QItemSelectionModel selectionModel(&model);

  /*
   * A empty selection must be accepted,
   * do nothing and return true.
   */
  selectionModel.clear();
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList({"A","B","C","D","E"}));

  /*
   * ---
   * |S|
   * ---
   * | |
   * ---
   * | |
   * ---
   * | |
   * ---
   * | |
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList({"B","C","D","E"}));
  /*
   * ---
   * | |
   * ---
   * |S|
   * ---
   * | |
   * ---
   * |S|
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(1, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(3, 0), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList({"B","D"}));
  /*
   * ---
   * |S|
   * ---
   * |S|
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(1, 0), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.stringList(), QStringList());
}

void RemoveRowsTest::removeRowsSelectionModelTableTest()
{
  VariantTableModel model;
  model.resize(3, 2);
  QItemSelectionModel selectionModel(&model);

  setModelData(model, 0, 0, "A0");
  setModelData(model, 0, 1, "B0");
  setModelData(model, 1, 0, "A1");
  setModelData(model, 1, 1, "B1");
  setModelData(model, 2, 0, "A2");
  setModelData(model, 2, 1, "B2");

  /*
   * -----
   * |S| |
   * -----
   * | | |
   * -----
   * |S|S|
   * -----
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(2, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(2, 1), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant("A1"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B1"));
  /*
   * -----
   * | |S|
   * -----
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 1), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.rowCount(), 0);
}

// void RemoveRowsTest::cancelRemoveRowsRangeTest()
// {
//   CachedTableModel model;
//   QItemSelectionModel selectionModel(&model);
//   RowRange r;
// 
//   model.populate({{1,"A"},{2,"B"},{3,"C"},{4,"D"},{5,"E"}});
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::None);
// 
//   r.setFirstRow(0);
//   r.setLastRow(4);
//   QVERIFY( removeRows(&model, r) );
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::Delete);
// 
//   r.setFirstRow(0);
//   r.setLastRow(0);
//   cancelRemoveRows(&model, r);
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::Delete);
// 
//   r.setFirstRow(2);
//   r.setLastRow(3);
//   cancelRemoveRows(&model, r);
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::Delete);
// 
//   r.setFirstRow(4);
//   r.setLastRow(4);
//   cancelRemoveRows(&model, r);
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::None);
// }

void RemoveRowsTest::cancelRemoveRowsRangeTest()
{
  EditPersonTableModel model;
  QItemSelectionModel selectionModel(&model);
  RowRange r;

  populatePersonStorage(model, {{1,"A"},{2,"B"},{3,"C"},{4,"D"},{5,"E"}});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant(5));

  r.setFirstRow(0);
  r.setLastRow(4);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant("x"));

  r.setFirstRow(0);
  r.setLastRow(0);
  cancelRemoveRows(&model, r);
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant("x"));

  r.setFirstRow(2);
  r.setLastRow(3);
  cancelRemoveRows(&model, r);
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant("x"));

  r.setFirstRow(4);
  r.setLastRow(4);
  cancelRemoveRows(&model, r);
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant(5));
}

// void RemoveRowsTest::cancelRemoveRowsSelectionTest()
// {
//   CachedTableModel model;
//   QItemSelectionModel selectionModel(&model);
//   RowSelection s;
//   RowRange r;
// 
//   model.populate({{1,"A"},{2,"B"},{3,"C"},{4,"D"},{5,"E"}});
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::None);
// 
//   r.setFirstRow(0);
//   r.setLastRow(4);
//   QVERIFY( removeRows(&model, r) );
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::Delete);
// 
//   s.clear();
//   r.setFirstRow(0);
//   r.setLastRow(1);
//   s.appendRange(r);
//   r.setFirstRow(3);
//   r.setLastRow(4);
//   s.appendRange(r);
//   cancelRemoveRows(&model, s);
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::None);
// }

void RemoveRowsTest::cancelRemoveRowsSelectionTest()
{
  EditPersonTableModel model;
  QItemSelectionModel selectionModel(&model);
  RowSelection s;
  RowRange r;

  populatePersonStorage(model, {{1,"A"},{2,"B"},{3,"C"},{4,"D"},{5,"E"}});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant(5));

  r.setFirstRow(0);
  r.setLastRow(4);
  QVERIFY( removeRows(&model, r) );
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant("x"));

  s.clear();
  r.setFirstRow(0);
  r.setLastRow(1);
  s.appendRange(r);
  r.setFirstRow(3);
  r.setLastRow(4);
  s.appendRange(r);
  cancelRemoveRows(&model, s);
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant(5));
}

// void RemoveRowsTest::cancelRemoveRowsSelectionModelTest()
// {
//   CachedTableModel model;
//   QItemSelectionModel selectionModel(&model);
// 
//   model.populate({{1,"A"},{2,"B"},{3,"C"},{4,"D"},{5,"E"}});
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::None);
// 
//   selectionModel.clear();
//   selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
//   selectionModel.select( model.index(1, 0), QItemSelectionModel::Select );
//   selectionModel.select( model.index(2, 0), QItemSelectionModel::Select );
//   selectionModel.select( model.index(3, 0), QItemSelectionModel::Select );
//   selectionModel.select( model.index(4, 0), QItemSelectionModel::Select );
//   QVERIFY( removeRows(&model, &selectionModel) );
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::Delete);
// 
//   /*
//    * A empty selection must be accepted and do nothing.
//    */
//   selectionModel.clear();
//   cancelRemoveRows(&model, &selectionModel);
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::Delete);
// 
//   /*
//    * ---
//    * |S|
//    * ---
//    * | |
//    * ---
//    * | |
//    * ---
//    * | |
//    * ---
//    * | |
//    * ---
//    */
//   selectionModel.clear();
//   selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
//   cancelRemoveRows(&model, &selectionModel);
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::Delete);
// 
//   /*
//    * ---
//    * |S|
//    * ---
//    * | |
//    * ---
//    * |S|
//    * ---
//    * | |
//    * ---
//    * |S|
//    * ---
//    */
//   selectionModel.clear();
//   selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
//   selectionModel.select( model.index(2, 0), QItemSelectionModel::Select );
//   selectionModel.select( model.index(4, 0), QItemSelectionModel::Select );
//   cancelRemoveRows(&model, &selectionModel);
//   QCOMPARE(model.rowCount(), 5);
//   QCOMPARE(model.operationAtRow(0), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(1), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(2), TableCacheOperation::None);
//   QCOMPARE(model.operationAtRow(3), TableCacheOperation::Delete);
//   QCOMPARE(model.operationAtRow(4), TableCacheOperation::None);
// }

void RemoveRowsTest::cancelRemoveRowsSelectionModelTest()
{
  EditPersonTableModel model;
  QItemSelectionModel selectionModel(&model);

  populatePersonStorage(model, {{1,"A"},{2,"B"},{3,"C"},{4,"D"},{5,"E"}});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant(5));

  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(1, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(2, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(3, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(4, 0), QItemSelectionModel::Select );
  QVERIFY( removeRows(&model, &selectionModel) );
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant("x"));

  /*
   * A empty selection must be accepted and do nothing.
   */
  selectionModel.clear();
  cancelRemoveRows(&model, &selectionModel);
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant("x"));

  /*
   * ---
   * |S|
   * ---
   * | |
   * ---
   * | |
   * ---
   * | |
   * ---
   * | |
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  cancelRemoveRows(&model, &selectionModel);
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant("x"));

  /*
   * ---
   * |S|
   * ---
   * | |
   * ---
   * |S|
   * ---
   * | |
   * ---
   * |S|
   * ---
   */
  selectionModel.clear();
  selectionModel.select( model.index(0, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(2, 0), QItemSelectionModel::Select );
  selectionModel.select( model.index(4, 0), QItemSelectionModel::Select );
  cancelRemoveRows(&model, &selectionModel);
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant(5));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RemoveRowsTest test;

  return QTest::qExec(&test, argc, argv);
}
