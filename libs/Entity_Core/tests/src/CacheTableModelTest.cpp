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
#include "CacheTableModelTest.h"
#include "PersonCache.h"
#include "Mdt/Entity/AbstractReadOnlyCache.h"
#include "Mdt/Entity/AbstractReadOnlyCacheTableModel.h"
#include "Mdt/Entity/AbstractEditableCache.h"
#include "Mdt/Entity/AbstractEditableCacheTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QSignalSpy>
#include <QVariantList>

/*
 * Test class
 */

class ReadOnlyCacheTableModel : public Mdt::Entity::AbstractReadOnlyCacheTableModel
{
};

class EditableCacheTableModel : public Mdt::Entity::AbstractEditableCacheTableModel
{
};

using namespace Mdt::Entity;
using namespace Mdt::TestLib;
using Mdt::Container::VariantRecord;

void CacheTableModelTest::initTestCase()
{
  qRegisterMetaType<Qt::Orientation>();
}

/*
 * Tests
 */

template<typename Cache, typename Model>
void horizontalHeaderTest()
{
  Cache cache;
  Model model;
  model.setCache(&cache);
  QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("id"));
  QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("name"));
  QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole), QVariant());
}

void CacheTableModelTest::readOnlyHorizontalHeaderTest()
{
  horizontalHeaderTest<PersonCache, ReadOnlyCacheTableModel>();
}

void CacheTableModelTest::editableHorizontalHeaderTest()
{
  horizontalHeaderTest<EditPersonCache, EditableCacheTableModel>();
}

template<typename Model>
void noCacheGetTest()
{
  Model model;
  QVERIFY(model.cache() == nullptr);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
}

void CacheTableModelTest::readOnlyNoCacheGetTest()
{
  noCacheGetTest<ReadOnlyCacheTableModel>();
}

void CacheTableModelTest::editableNoCacheGetTest()
{
  noCacheGetTest<EditableCacheTableModel>();
}

void CacheTableModelTest::readOnlyFlagsTest()
{
  const auto defaultFlags = getDefaultQAbstractTableModelFlags();
  PersonCache cache;
  populatePersonStorage(cache, {"A"});
  QVERIFY(cache.fetchAll());
  ReadOnlyCacheTableModel model;
  model.setCache(&cache);

  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);
  QCOMPARE(getModelFlags(model, 0, 0), defaultFlags);
  QCOMPARE(getModelFlags(model, 0, 1), defaultFlags);
}

void CacheTableModelTest::editableFlagsTest()
{
  const auto expectedFlags = getDefaultQAbstractTableModelFlags() | Qt::ItemIsEditable;
  EditPersonCache cache;
  populatePersonStorage(cache, {"A"});
  QVERIFY(cache.fetchAll());
  EditableCacheTableModel model;
  model.setCache(&cache);

  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);
  QCOMPARE(getModelFlags(model, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 0, 1), expectedFlags);
}

template<typename Cache, typename Model>
void setCacheTest()
{
  Model model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);

  Cache emptyCache;
  model.setCache(&emptyCache);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);

  Cache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  QCOMPARE(cache.rowCount(), 3);
  model.setCache(&cache);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
}

void CacheTableModelTest::readOnlySetCacheTest()
{
  setCacheTest<PersonCache, ReadOnlyCacheTableModel>();
}

void CacheTableModelTest::editableSetCacheTest()
{
  setCacheTest<EditPersonCache, EditableCacheTableModel>();
}

template<typename Cache, typename Model>
void setCacheSignalTest()
{
  Model model;
  QSignalSpy resetSpy(&model, &Model::modelReset);
  QVERIFY(resetSpy.isValid());
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &Model::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &Model::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());

  QCOMPARE(resetSpy.count(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  Cache cache;
  model.setCache(&cache);
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);

  resetSpy.clear();
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
  QCOMPARE(rowsInsertedSpy.count(), 3);
}

void CacheTableModelTest::readOnlySetCacheSignalTest()
{
  setCacheSignalTest<PersonCache, ReadOnlyCacheTableModel>();
}

void CacheTableModelTest::editableSetCacheSignalTest()
{
  setCacheSignalTest<EditPersonCache, EditableCacheTableModel>();
}

void CacheTableModelTest::setDataTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  EditableCacheTableModel model;
  model.setCache(&cache);

  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));

  QVERIFY(!setModelData(model, 2, 1, "ToolTip", Qt::ToolTipRole));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));

  QVERIFY(setModelData(model, 2, 1, "EC"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("EC"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("e"));
}

void CacheTableModelTest::setDataSignalTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  EditableCacheTableModel model;
  model.setCache(&cache);
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditableCacheTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditableCacheTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(setModelData(model, 2, 1, "EC"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 2);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
}

void CacheTableModelTest::setDataFromCacheTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  EditableCacheTableModel model;
  model.setCache(&cache);

  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));

  cache.setData(0, 1, "EA");
  cache.fromBackendSetData(1, 1, "EB");
  QCOMPARE(getModelData(model, 0, 1), QVariant("EA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("EB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("e"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
}

void CacheTableModelTest::setDataFromCacheSignalTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  EditableCacheTableModel model;
  model.setCache(&cache);
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditableCacheTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditableCacheTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  cache.setData(0, 1, "EA");
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 0);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 0);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  cache.fromBackendSetData(1, 1, "EB");
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 1);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 1);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CacheTableModelTest::setDataAndSubmitTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  EditableCacheTableModel model;
  model.setCache(&cache);

  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));

  QVERIFY(setModelData(model, 2, 1, "EC"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("EC"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("e"));

  QVERIFY(cache.submitChanges());
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("EC"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
}

void CacheTableModelTest::setDataAndSubmitSignalTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  EditableCacheTableModel model;
  model.setCache(&cache);
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditableCacheTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditableCacheTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(setModelData(model, 2, 1, "EC"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 2);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last

  QVERIFY(cache.submitChanges());
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 2);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
}

template<typename Cache, typename Model>
void insertRowsFromCacheTest()
{
  Cache cache;
  Model model;
  model.setCache(&cache);

  QCOMPARE(model.rowCount(), 0);
  cache.fromBackendInsertRecords(0, 2, VariantRecord(2));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
}

void CacheTableModelTest::readOnlyInsertRowsFromCacheTest()
{
  insertRowsFromCacheTest<PersonCache, ReadOnlyCacheTableModel>();
}

void CacheTableModelTest::editableInsertRowsFromCacheTest()
{
  insertRowsFromCacheTest<EditPersonCache, EditableCacheTableModel>();
}

template<typename Cache, typename Model>
void insertRowsFromCacheSignalTest()
{
  Model model;
  Cache cache;
  model.setCache(&cache);
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditableCacheTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditableCacheTableModel::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  cache.fromBackendInsertRecords(0, 2, VariantRecord(2));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
}

void CacheTableModelTest::readOnlyInsertRowsFromCacheSignalTest()
{
  insertRowsFromCacheSignalTest<PersonCache, ReadOnlyCacheTableModel>();
}

void CacheTableModelTest::editableInsertRowsFromCacheSignalTest()
{
  insertRowsFromCacheSignalTest<EditPersonCache, EditableCacheTableModel>();
}

void CacheTableModelTest::insertRowsTest()
{
  EditableCacheTableModel model;

  QCOMPARE(model.rowCount(), 0);
  QVERIFY(!model.insertRows(0, 2));
  QCOMPARE(model.rowCount(), 0);

  EditPersonCache cache;
  model.setCache(&cache);
  QVERIFY(model.insertRows(0, 2));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
}

void CacheTableModelTest::insertRowsSignalTest()
{
  EditableCacheTableModel model;
  EditPersonCache cache;
  model.setCache(&cache);
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditableCacheTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditableCacheTableModel::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditableCacheTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(model.insertRows(0, 2));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last
}

void CacheTableModelTest::insertRowsAndSubmitTest()
{
  EditableCacheTableModel model;
  EditPersonCache cache;
  model.setCache(&cache);

  QVERIFY(model.insertRows(0, 2));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));

  QVERIFY(cache.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
}

void CacheTableModelTest::insertRowsAndSubmitSignalTest()
{
  EditableCacheTableModel model;
  EditPersonCache cache;
  model.setCache(&cache);
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditableCacheTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditableCacheTableModel::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditableCacheTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(model.insertRows(0, 1));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(cache.submitChanges());
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
}

void CacheTableModelTest::readOnlyQtModelTest()
{

  QFAIL("Not complete");
}

void CacheTableModelTest::editableQtModelTest()
{

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CacheTableModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
