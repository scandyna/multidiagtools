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
#include "AbstractCachedTableModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/AbstractCachedTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/TestLib/ItemModelInsertRowTest.h"
#include "Mdt/TestLib/ItemModelRemoveRowTest.h"
#include "Mdt/Container/StlContainer.h"
#include "Mdt/Container/TableCache.h"
#include "Mdt/IndexRange/RowRange.h"
#include <QSignalSpy>
#include <QVariant>
#include <QVariantList>
#include <QModelIndex>
#include <QMetaType>
#include <vector>
#include <array>
#include <initializer_list>
#include <algorithm>

// #include <QDebug>

using namespace Mdt::ItemModel;
using namespace Mdt::Container;
using namespace Mdt::TestLib;

/*
 * Test class
 */

class CachedTableModel : public Mdt::ItemModel::AbstractCachedTableModel
{
  using ParentClass = Mdt::ItemModel::AbstractCachedTableModel;

 public:

  using Record = std::array<QVariant, 2>;
  using Storage = std::vector<Record>;

  CachedTableModel(QObject* parent = nullptr)
   : AbstractCachedTableModel(parent)
  {
    setModelEditable(true);
  }

  int storageRowCount() const
  {
    return mStorage.size();
  }

  QVariant storageData(int row, int column) const
  {
    Q_ASSERT(row < storageRowCount());
    Q_ASSERT(column < 2);
    return mStorage[row][column];
  }

  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override
  {
    if(!ParentClass::setData(index, value, role)){
      return false;
    }
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row() < rowCount());

    emitVerticalHeaderDataChanged(index.row());

    return true;
  }

  bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex())
  {
    if(parent.isValid()){
      return false;
    }
    Q_ASSERT(row >= 0);
    Q_ASSERT(row <= rowCount());
    Q_ASSERT(count >= 0);

    Mdt::IndexRange::RowRange rowRange;
    rowRange.setFirstRow(row);
    rowRange.setRowCount(count);

    beginInsertRows(rowRange.firstRow(), rowRange.lastRow());
    mCache.insertRecords(row, count, Record());
    endInsertRows();

    return true;
  }

  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override
  {
    if(parent.isValid()){
      return false;
    }
    Q_ASSERT(row >= 0);
    Q_ASSERT(count >= 1);
    Q_ASSERT( (row+count) <= rowCount() );

    mCache.removeRecords(row, count);

    Mdt::IndexRange::RowRange rowRange;
    rowRange.setFirstRow(row);
    rowRange.setRowCount(count);
    emitVerticalHeaderDataChanged(rowRange);

    return true;
  }

  bool submitChanges()
  {
    removeRowsToDeleteFromCacheOnly();
    if( !insertRowsIntoStorage( mCache.getRowsToInsertIntoStorage() ) ){
      return false;
    }
    if( !removeRowsToDeleteFromStorage() ){
      return false;
    }
    mCache.commitChanges();
    const auto rowRange = mCache.committedRows();
    if(!rowRange.isNull()){
      emitVerticalHeaderDataChanged(rowRange);
    }
    return true;
  }

  void populate(std::initializer_list<Record> initTable)
  {
    beginResetModel();
    mCache.clear();
    mStorage.clear();
    for(const auto & record : initTable){
      mStorage.push_back(record);
      mCache.appendRecordFromStorage(record);
    }
    endResetModel();
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

  QVariant displayRoleData(int row, int column) const
  {
    return mCache.constRecordAt(row)[column];
  }

  bool setEditRoleData(int row, int column, const QVariant& value) override
  {
    mCache.refRecordAtForUpdate(row)[column] = value;
    return true;
  }

//   bool insertRowsToCache(int row, int count) override
//   {
//     mCache.insertRecords(row, count, Record());
//     return true;
//   }

  bool insertRowsIntoStorage(const RowList & rows)
  {
    for(auto row : rows){
      if(!insertRowIntoStorage(row)){
        return false;
      }
    }
    return true;
  }

  bool insertRowIntoStorage(int row)
  {
    mStorage.push_back( mCache.constRecordAt(row) );
    return true;
  }

  void removeRowsToDeleteFromCacheOnly()
  {
    const auto rowList = mCache.getRowsToDeleteInCacheOnly();
    for(const auto row : rowList){
      beginRemoveRows(row, row);
      mCache.removeRecordInCache(row);
      endRemoveRows();
    }
  }

  bool removeRowsToDeleteFromStorage()
  {
    const auto rowList = mCache.getRowsToDeleteInStorage();
    for(const auto row : rowList){
      if(!removeRowFromStorage(row)){
        return false;
      }
      beginRemoveRows(row, row);
      mCache.removeRecordInCache(row);
      endRemoveRows();
    }
    return true;
  }

  bool removeRowFromStorage(int row)
  {
    removeFromContainer(mStorage, row, 1);
    return true;
  }

  Mdt::Container::TableCacheOperation operationAtRow(int row) const override
  {
    return mCache.operationAtRow(row);
  }

  TableCache<Record> mCache;
  Storage mStorage;
};

void AbstractCachedTableModelTest::initTestCase()
{
  // For QSignalSpy
  qRegisterMetaType<Qt::Orientation>();
}

/*
 * Tests
 */

void AbstractCachedTableModelTest::testClassPopulateTest()
{
  CachedTableModel model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);

  model.populate({{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.storageRowCount(), 1);
  QCOMPARE(model.storageData(0, 0), QVariant(1));
  QCOMPARE(model.storageData(0, 1), QVariant("A"));

  model.populate({{2,"B"},{3,"C"}});
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(3));
  QCOMPARE(getModelData(model, 1, 1), QVariant("C"));
  QCOMPARE(model.storageRowCount(), 2);
  QCOMPARE(model.storageData(0, 0), QVariant(2));
  QCOMPARE(model.storageData(0, 1), QVariant("B"));
  QCOMPARE(model.storageData(1, 0), QVariant(3));
  QCOMPARE(model.storageData(1, 1), QVariant("C"));
}

void AbstractCachedTableModelTest::constructTest()
{
  CachedTableModel model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);

  QObject parent;
  auto *model2 = new CachedTableModel(&parent);
  QCOMPARE(model2->rowCount(), 0);
  QCOMPARE(model2->columnCount(), 2);
}

void AbstractCachedTableModelTest::insertRowsAndSubmitTest()
{
  CachedTableModel model;
  const auto defaultFlags = getDefaultQAbstractTableModelFlags() | Qt::ItemIsEditable;

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(getModelFlags(model, 0, 0), defaultFlags);
  QCOMPARE(getModelFlags(model, 0, 1), defaultFlags);
  QCOMPARE(model.storageRowCount(), 0);

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(getModelFlags(model, 0, 0), defaultFlags);
  QCOMPARE(getModelFlags(model, 0, 1), defaultFlags);
  QCOMPARE(model.storageRowCount(), 1);
}

void AbstractCachedTableModelTest::insertRowsAndSubmitHeaderTest()
{
  QVariantList arguments;
  CachedTableModel model;
  QSignalSpy headerDataChangedSpy(&model, &CachedTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(appendRowToModel(model));
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));
}

void AbstractCachedTableModelTest::itemModelInsertRowTest()
{
  CachedTableModel model;
  ItemModelInsertRowTest test(&model);
}

void AbstractCachedTableModelTest::setDataTest()
{
  CachedTableModel model;

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);
  const auto flags = getModelFlags(model, 0, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));

  QVERIFY(setModelData(model, 0, 0, 10));
  QVERIFY(setModelData(model, 0, 1, "A"));
  QCOMPARE(getModelData(model, 0, 0), QVariant(10));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));

  QVERIFY(!model.setData(QModelIndex(), 15));
  QVERIFY(!setModelData(model, 0, 0, 15, Qt::FontRole));
  QCOMPARE(getModelData(model, 0, 0), QVariant(10));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
}

void AbstractCachedTableModelTest::setDataHeaderTest()
{
  QVariantList arguments;
  CachedTableModel model;
  QSignalSpy headerDataChangedSpy(&model, &CachedTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QVERIFY(model.submitChanges());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  headerDataChangedSpy.clear();

  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(setModelData(model, 0, 0, 10));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("e"));

  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(model.submitChanges());
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
}

void AbstractCachedTableModelTest::removeRowsAndSubmitTest()
{
  CachedTableModel model;

  model.populate({{1,"A"},{2,"B"}});
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  auto flags = getModelFlags(model, 0, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 0, 1);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 1);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));

  QVERIFY(removeFirstRowFromModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  flags = getModelFlags(model, 0, 0);
  QVERIFY(!flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 0, 1);
  QVERIFY(!flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 1);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  QCOMPARE(model.storageRowCount(), 2);
  QCOMPARE(model.storageData(0, 0), QVariant(1));
  QCOMPARE(model.storageData(0, 1), QVariant("A"));
  QCOMPARE(model.storageData(1, 0), QVariant(2));
  QCOMPARE(model.storageData(1, 1), QVariant("B"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B"));
  flags = getModelFlags(model, 0, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 0, 1);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  QCOMPARE(model.storageRowCount(), 1);
  QCOMPARE(model.storageData(0, 0), QVariant(2));
  QCOMPARE(model.storageData(0, 1), QVariant("B"));
}

void AbstractCachedTableModelTest::removeRowsAndSubmitResizeSignalsTest()
{
  QVariantList arguments;
  CachedTableModel model;
  QSignalSpy rowsAboutToBeRemovedSpy(&model, &CachedTableModel::rowsAboutToBeRemoved);
  QSignalSpy rowsRemovedSpy(&model, &CachedTableModel::rowsRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QVERIFY(rowsRemovedSpy.isValid());

  model.populate({{1,"A"},{2,"B"}});
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
}

void AbstractCachedTableModelTest::removeRowsAndSubmitHeaderTest()
{
  QVariantList arguments;
  CachedTableModel model;
  QSignalSpy headerDataChangedSpy(&model, &CachedTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  model.populate({{1,"A"},{2,"B"}});
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));

  QVERIFY(removeFirstRowFromModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
}

void AbstractCachedTableModelTest::insertRemoveRowsAndSubmitTest()
{
  CachedTableModel model;

  model.populate({{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  auto flags = getModelFlags(model, 0, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 0, 1);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant());
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  flags = getModelFlags(model, 0, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 0, 1);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 0);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 1);
  QVERIFY(flags.testFlag(Qt::ItemIsEditable));
  QCOMPARE(model.storageRowCount(), 1);
  QCOMPARE(model.storageData(0, 0), QVariant(1));
  QCOMPARE(model.storageData(0, 1), QVariant("A"));

  QVERIFY(model.removeRows(0, 2));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant());
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  flags = getModelFlags(model, 0, 0);
  QVERIFY(!flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 0, 1);
  QVERIFY(!flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 0);
  QVERIFY(!flags.testFlag(Qt::ItemIsEditable));
  flags = getModelFlags(model, 1, 1);
  QVERIFY(!flags.testFlag(Qt::ItemIsEditable));
  QCOMPARE(model.storageRowCount(), 1);
  QCOMPARE(model.storageData(0, 0), QVariant(1));
  QCOMPARE(model.storageData(0, 1), QVariant("A"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.storageRowCount(), 0);
}

void AbstractCachedTableModelTest::insertRemoveRowsAndSubmitHeaderTest()
{
  QVariantList arguments;
  CachedTableModel model;
  QSignalSpy headerDataChangedSpy(&model, &CachedTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  model.populate({{1,"A"},{2,"B"}});
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("2"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("*"));

  QVERIFY(model.removeRows(1, 2));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("1"));
}

void AbstractCachedTableModelTest::qtModelTest()
{
  CachedTableModel model;

  appendRowToModel(model);
  appendRowToModel(model);
  appendRowToModel(model);

  QCOMPARE(model.rowCount(), 3);
  QtModelTest mt(&model);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AbstractCachedTableModelTest test;

  return QTest::qExec(&test, argc, argv);
}
