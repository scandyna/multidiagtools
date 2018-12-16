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
#include "CachedTableModelTest.h"
#include "PersonTableModel.h"
#include "Mdt/ItemModel/AbstractCachedTableModel.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QSignalSpy>
#include <QVariantList>

using namespace Mdt::ItemModel;
using namespace Mdt::TestLib;
using Mdt::Container::VariantRecord;

void CachedTableModelTest::initTestCase()
{
  qRegisterMetaType<Qt::Orientation>();
}

/*
 * Helpers
 */

// bool isItemEditable(const QAbstractItemModel & model, int row, int column)
// {
//   return getModelFlags(model, row, column).testFlag(Qt::ItemIsEditable);
// }

bool isItemEnabled(const QAbstractItemModel & model, int row, int column)
{
  return getModelFlags(model, row, column).testFlag(Qt::ItemIsEnabled);
}

bool isItemPending(const QAbstractItemModel & model, int row, int column)
{
  return !isItemEnabled(model, row, column);
}

bool isItemRemoved(const QAbstractItemModel & model, int row, int column)
{
  const auto flags = getModelFlags(model, row, column);

  if(!flags.testFlag(Qt::ItemIsEnabled)){
    return false;
  }
  if(!flags.testFlag(Qt::ItemIsSelectable)){
    return false;
  }
  if(flags.testFlag(Qt::ItemIsEditable)){
    return false;
  }

  return true;
//   return !isItemEnabled(model, row, column);
}

/*
 * Tests
 */

template<typename Model>
void rowCountLimitTest()
{
  Model model;

  QVERIFY(model.cachedRowCountLimit() > 100);
  model.setCachedRowCountLimit(20);
  QCOMPARE(model.cachedRowCountLimit(), 20);

  QFAIL("Not complete");
}

void CachedTableModelTest::readOnlyRowCountLimitTest()
{
  rowCountLimitTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableRowCountLimitTest()
{
  rowCountLimitTest<EditPersonTableModel>();
}

template<typename Model>
void rowCountLimitSignalTest()
{
  QFAIL("Not complete");
}

void CachedTableModelTest::readOnlyRowCountLimitSignalTest()
{
  rowCountLimitSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableRowCountLimitSignalTest()
{
  rowCountLimitSignalTest<EditPersonTableModel>();
}

template<typename Model>
void horizontalHeaderTest()
{
  Model model;
  QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("id"));
  QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("name"));
  QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole), QVariant());
}

void CachedTableModelTest::readOnlyHorizontalHeaderTest()
{
  horizontalHeaderTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableHorizontalHeaderTest()
{
  horizontalHeaderTest<EditPersonTableModel>();
}

void CachedTableModelTest::readOnlyFlagsTest()
{
  const auto defaultFlags = getDefaultQAbstractTableModelFlags();
  ListPersonTableModel model;

  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);
  QCOMPARE(getModelFlags(model, 0, 0), defaultFlags);
  QCOMPARE(getModelFlags(model, 0, 1), defaultFlags);
}

void CachedTableModelTest::editableFlagsTest()
{
  const auto expectedFlags = getDefaultQAbstractTableModelFlags() | Qt::ItemIsEditable;
  EditPersonTableModel model;

  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);
  QCOMPARE(getModelFlags(model, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 0, 1), expectedFlags);
}

template<typename Model>
void dataTest()
{
  Model model;

  model.fromBackendAppendRecord(VariantRecord{1, "A"});
  model.fromBackendAppendRecord(VariantRecord{2, "B"});
  model.fromBackendAppendRecord(VariantRecord{3, "C"});
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(getModelData(model, 0, 1, Qt::EditRole), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1, Qt::EditRole), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1, Qt::EditRole), QVariant("C"));
  QCOMPARE(getModelData(model, 0, 1, Qt::FontRole), QVariant());
  QCOMPARE(getModelData(model, 1, 1, Qt::FontRole), QVariant());
  QCOMPARE(getModelData(model, 2, 1, Qt::FontRole), QVariant());
}

void CachedTableModelTest::readOnlyDataTest()
{
  dataTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableDataTest()
{
  dataTest<EditPersonTableModel>();
}

template<typename Model>
void fromBackendInsertRecordsTest()
{
  Model model;

  QCOMPARE(model.columnCount(), 2);

  /*
   * Add to the end
   */
  QCOMPARE(model.rowCount(), 0);
  model.fromBackendAppendRecord({1,"A"});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  model.fromBackendAppendRecord({2,"B"});
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  /*
   * Insert at the beginning
   */
  model.fromBackendInsertRecords(0, 2, {0,"X"});
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  /*
   * Insert in the middle
   */
  model.fromBackendInsertRecords(2, 1, {30,"Z"});
  QCOMPARE(model.rowCount(), 5);
  QCOMPARE(getModelData(model, 0, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("Z"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 4, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QCOMPARE(model.headerData(4, Qt::Vertical), QVariant(5));
}

void CachedTableModelTest::readOnlyFromBackendInsertRecordsTest()
{
  fromBackendInsertRecordsTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendInsertRecordsTest()
{
  fromBackendInsertRecordsTest<EditPersonTableModel>();
}

template<typename Model>
void fromBackendInsertRecordsSignalTest()
{
  Model model;
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &Model::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &Model::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  model.fromBackendInsertRecords(0, 2, VariantRecord(2));
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

void CachedTableModelTest::readOnlyFromBackendInsertRecordsSignalTest()
{
  fromBackendInsertRecordsSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendInsertRecordsSignalTest()
{
  fromBackendInsertRecordsSignalTest<EditPersonTableModel>();
}

template<typename Model>
void fetchAllTest()
{
  Model model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);
  /*
   * Storage has no data
   */
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 0);
  /*
   * Storage has less data than limit
   */
  model.setCachedRowCountLimit(500);
  populatePersonStorageByNames(model, {"A","B","C"});
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  /*
   * Storage has more data than limit
   */
  model.setCachedRowCountLimit(2);
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  /*
   * Storage has the same amount of data than limit
   */
  model.setCachedRowCountLimit(2);
  populatePersonStorageByNames(model, {"A","B"});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
}

void CachedTableModelTest::readOnlyFetchAllTest()
{
  fetchAllTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFetchAllTest()
{
  fetchAllTest<EditPersonTableModel>();
}

template<typename Model>
void fetchAllSignalTest()
{
  Model model;
  QSignalSpy modelAboutToBeResetSpy(&model, &Model::modelAboutToBeReset);
  QSignalSpy modelResetSpy(&model, &Model::modelReset);
  QVERIFY(modelAboutToBeResetSpy.isValid());
  QVERIFY(modelResetSpy.isValid());
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &Model::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &Model::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());

  populatePersonStorageByNames(model, {"A","B","C"});
  QCOMPARE(modelAboutToBeResetSpy.count(), 0);
  QCOMPARE(modelResetSpy.count(), 0);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);

  QVERIFY(model.fetchAll());
  QCOMPARE(modelAboutToBeResetSpy.count(), 1);
  QCOMPARE(modelResetSpy.count(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
  QCOMPARE(rowsInsertedSpy.count(), 3);
  QCOMPARE(model.rowCount(), 3);
}

void CachedTableModelTest::readOnlyFetchAllSignalTest()
{
  fetchAllSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFetchAllSignalTest()
{
  fetchAllSignalTest<EditPersonTableModel>();
}

template<typename Model>
void fetchRowTest()
{
  Model model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);

  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));

  model.updateStoragePersonNameAt(1, "uB");
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));

  QVERIFY(model.fetchRow(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));

  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("uB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
}

void CachedTableModelTest::readOnlyFetchRowTest()
{
  fetchRowTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFetchRowTest()
{
  fetchRowTest<EditPersonTableModel>();
}

template<typename Model>
void fetchRowSignalTest()
{
  Model model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &Model::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
//   QSignalSpy headerDataChangedSpy(&model, &Model::headerDataChanged);
//   QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
//   QCOMPARE(headerDataChangedSpy.count(), 0);

  model.updateStoragePersonNameAt(1, "uB");
  QCOMPARE(dataChangedSpy.count(), 0);
//   QCOMPARE(headerDataChangedSpy.count(), 0);

  QVERIFY(model.fetchRow(1));
  QCOMPARE(dataChangedSpy.count(), 0);

  model.fetchRecordFromBackendSucceeded();
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
  QVERIFY(roles.contains(Qt::DisplayRole));
}

void CachedTableModelTest::readOnlyFetchRowSignalTest()
{
  fetchRowSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFetchRowSignalTest()
{
  fetchRowSignalTest<EditPersonTableModel>();
}

template<typename Model>
void fetchRowThenFromBackendInsertRecordTest()
{
  Model model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);

  populatePersonStorageByNames(model, {"A"});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));

  model.updateStoragePersonNameAt(0, "uA");
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QVERIFY(!isItemPending(model, 0, 1));

  QVERIFY(model.fetchRow(0));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QVERIFY(isItemPending(model, 0, 1));

  model.appendPersonByNameToModelAndStorage("B");
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));

  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));

  model.updateStoragePersonNameAt(1, "uB");
  QCOMPARE(getModelData(model, 0, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));

  QVERIFY(model.fetchRow(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));

  model.prependPersonByNameToModelAndStorage("Z");
  QCOMPARE(getModelData(model, 0, 1), QVariant("Z"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(isItemPending(model, 2, 1));

  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("Z"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("uB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
}

void CachedTableModelTest::readOnlyFetchRowThenFromBackendInsertRecordTest()
{
  fetchRowThenFromBackendInsertRecordTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFetchRowThenFromBackendInsertRecordTest()
{
  fetchRowThenFromBackendInsertRecordTest<EditPersonTableModel>();
}

template<typename Model>
void fetchRowThenFromBackendInsertRecordSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| A  |
   * --------------
   */
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);

  /*
   * Fetch record at row 0
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| A  |
   * --------------
   */
  model.updateStoragePersonNameAt(0, "uA");
  QVERIFY(model.fetchRow(0));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);

  /*
   * Append a record
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| A  |
   * --------------
   * | 2 || 2| B  |
   * --------------
   */
  model.appendPersonByNameToModelAndStorage("B");
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(dataChangedSpy.count(), 0);

  /*
   * Fetch record succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| uA |
   * --------------
   * | 2 || 2| B  |
   * --------------
   */
  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 2);
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
  QVERIFY(roles.contains(Qt::DisplayRole));

  /*
   * Fetch record at row 1
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| uA |
   * --------------
   * | 2 || 2| B  |
   * --------------
   */
  model.updateStoragePersonNameAt(1, "uB");
  QVERIFY(model.fetchRow(1));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(dataChangedSpy.count(), 0);

  /*
   * Prepend a record
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 3| Z  |
   * --------------
   * | 2 || 1| uA |
   * --------------
   * | 3 || 2| B  |
   * --------------
   */
  model.prependPersonByNameToModelAndStorage("Z");
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(dataChangedSpy.count(), 0);

  /*
   * Fetch record succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 3| Z  |
   * --------------
   * | 2 || 1| uA |
   * --------------
   * | 3 || 2| uB |
   * --------------
   */
  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 3);
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
  QVERIFY(roles.contains(Qt::DisplayRole));
}

void CachedTableModelTest::readOnlyFetchRowThenFromBackendInsertRecordSignalTest()
{
  fetchRowThenFromBackendInsertRecordSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFetchRowThenFromBackendInsertRecordSignalTest()
{
  fetchRowThenFromBackendInsertRecordSignalTest<EditPersonTableModel>();
}

void CachedTableModelTest::fetchRowThenInsertRowTest()
{
  EditPersonTableModel model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);

  populatePersonStorageByNames(model, {"A"});
  QVERIFY(model.fetchAll());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));

  model.updateStoragePersonNameAt(0, "uA");
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QVERIFY(!isItemPending(model, 0, 1));

  QVERIFY(model.fetchRow(0));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QVERIFY(isItemPending(model, 0, 1));

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QVERIFY(isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));

  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));

  QVERIFY(setModelData(model, 1, 1, "B"));
  QVERIFY(model.submitChanges());
  model.addRecordToBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));

  model.updateStoragePersonNameAt(1, "uB");
  QCOMPARE(getModelData(model, 0, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));

  QVERIFY(model.fetchRow(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));

  QVERIFY(prependRowToModel(model));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant());
  QCOMPARE(getModelData(model, 1, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(isItemPending(model, 2, 1));

  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant());
  QCOMPARE(getModelData(model, 1, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("uB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));

  QVERIFY(setModelData(model, 0, 1, "Z"));
  QVERIFY(model.submitChanges());
  model.addRecordToBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("Z"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("uA"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("uB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
}

void CachedTableModelTest::fetchRowThenInsertRowSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| A  |
   * --------------
   */
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Fetch record at row 0
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| A  |
   * --------------
   */
  model.updateStoragePersonNameAt(0, "uA");
  QVERIFY(model.fetchRow(0));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Add to the end
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| A  |
   * --------------
   * | * ||  |    |
   * --------------
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Fetch record at row 0 succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| uA |
   * --------------
   * | * ||  |    |
   * --------------
   */
  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 2);
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
  QVERIFY(roles.contains(Qt::DisplayRole));

  /*
   * Set and store new record
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| uA |
   * --------------
   * | 2 || 2| B  |
   * --------------
   */
  QVERIFY(setModelData(model, 1, 1, "B"));
  dataChangedSpy.clear();
  headerDataChangedSpy.clear();
  QVERIFY(model.submitChanges());
  model.addRecordToBackendSucceeded();
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
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last

  /*
   * Fetch record at row 1
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1| uA |
   * --------------
   * | 2 || 2| B  |
   * --------------
   */
  model.updateStoragePersonNameAt(1, "uB");
  QVERIFY(model.fetchRow(1));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Prepend a new row
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |    |
   * --------------
   * | 2 || 1| uA |
   * --------------
   * | 3 || 2| B  |
   * --------------
   */
  QVERIFY(prependRowToModel(model));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Fetch record succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |    |
   * --------------
   * | 2 || 1| uA |
   * --------------
   * | 3 || 2| uB |
   * --------------
   */
  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 3);
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
  QVERIFY(roles.contains(Qt::DisplayRole));
}

template<typename Model>
void fromBackendSetDataTest()
{
  Model model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));

  model.fromBackendSetData(0, 1, "eA");
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("eA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
}

void CachedTableModelTest::readOnlyFromBackendSetDataTest()
{
  fromBackendSetDataTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendSetDataTest()
{
  fromBackendSetDataTest<EditPersonTableModel>();
}

template<typename Model>
void fromBackendSetDataSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  model.fromBackendSetData(2, 1, "EC");
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 2);
  QCOMPARE(topLeft.column(), 1);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedTableModelTest::readOnlyFromBackendSetDataSignalTest()
{
  fromBackendSetDataSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendSetDataSignalTest()
{
  fromBackendSetDataSignalTest<EditPersonTableModel>();
}

template<typename Model>
void fromBackendSetRecordTest()
{
  Model model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));

  model.fromBackendSetRecord(0, VariantRecord{11,"eA"});
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 0), QVariant(11));
  QCOMPARE(getModelData(model, 0, 1), QVariant("eA"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
}

void CachedTableModelTest::readOnlyFromBackendSetRecordTest()
{
  fromBackendSetRecordTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendSetRecordTest()
{
  fromBackendSetRecordTest<EditPersonTableModel>();
}

template<typename Model>
void fromBackendSetRecordSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  model.fromBackendSetRecord(2, VariantRecord{22,"eC"});
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
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedTableModelTest::readOnlyFromBackendSetRecordSignalTest()
{
  fromBackendSetRecordSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendSetRecordSignalTest()
{
  fromBackendSetRecordSignalTest<EditPersonTableModel>();
}

void CachedTableModelTest::setDataTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());

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

void CachedTableModelTest::setDataSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(setModelData(model, 2, 1, "EC"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 2);
  QCOMPARE(topLeft.column(), 1);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
}

// void CachedTableModelTest::setDataThenFetchRowTest()
// {
//   EditPersonTableModel model;
//   populatePersonStorageByNames(model, {"A","B","C"});
//   QVERIFY(model.fetchAll());
// 
//   QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
//   QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
//   QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
// 
//   QVERIFY(setModelData(model, 2, 1, "eC"));
//   QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
//   QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
//   QCOMPARE(getModelData(model, 2, 1), QVariant("eC"));
//   QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
//   QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
//   QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("e"));
// 
//   model.updateStoragePersonNameAt(2, "uC");
//   
//   QFAIL("Not complete");
// }
// 
// void CachedTableModelTest::setDataThenFetchRowSignalTest()
// {
//   QFAIL("Not complete");
// }

void CachedTableModelTest::setDataThenFromBackendInsertRecordsTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B"});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  /*
   * Edit
   */
  QVERIFY(setModelData(model, 1, 1, "eB"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  /*
   * Add to the end
   */
  model.fromBackendAppendRecord({3,"C"});
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  /*
   * Insert to the beginning
   */
  model.fromBackendInsertRecords(0, 1, {30,"Z"});
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("Z"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("eB"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("e"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
}

void CachedTableModelTest::setDataThenFromBackendInsertRecordsSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditPersonTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditPersonTableModel::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.rowCount(), 2);
  /*
   * Edit
   *
   * ------
   * | | A|
   * ------
   * |e|eB|
   * ------
   */
  QVERIFY(setModelData(model, 1, 1, "eB"));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last
  /*
   * Add to the end
   *
   * ------
   * | | A|
   * ------
   * |e|eB|
   * ------
   * | | C|
   * ------
   */
  model.fromBackendAppendRecord({3,"C"});
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Insert to the beginning
   *
   * ------
   * | | Z|
   * ------
   * | | A|
   * ------
   * |e|eB|
   * ------
   * | | C|
   * ------
   */
  model.fromBackendInsertRecords(0, 1, {30,"Z"});
  QCOMPARE(model.rowCount(), 4);
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
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedTableModelTest::setDataThenSubmitTest()
{
  EditPersonTableModel model;
  populatePersonStorage(model, {{1,"A"},{2,"B"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Edit
   */
  QVERIFY(setModelData(model, 1, 1, "eB"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Submit
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Done
   */
  model.updateRecordInBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("eB"));
}

void CachedTableModelTest::setDataThenSubmitSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.rowCount(), 3);
  /*
   * Edit
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * | e || 3| eC |
   * --------------
   */
  QVERIFY(setModelData(model, 2, 1, "eC"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 2);
  QCOMPARE(topLeft.column(), 1);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
  /*
   * Submit
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * | e || 3| eC |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Done
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * |   || 3| eC |
   * --------------
   */
  model.updateRecordInBackendSucceeded();
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
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
}

void CachedTableModelTest::setDataThenSubmitFailTest()
{
  EditPersonTableModel model;
  populatePersonStorage(model, {{1,"A"},{2,"B"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Edit
   */
  QVERIFY(setModelData(model, 1, 1, "eB"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Submit
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Failed
   */
  model.updateRecordInBackendFailed();
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("!"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  /*
   * Submit again
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("!"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  /*
   * Done
   */
  model.updateRecordInBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("eB"));
}

void CachedTableModelTest::setDataThenSubmitFailSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QCOMPARE(model.rowCount(), 3);
  /*
   * Edit
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * | e || 3| eC |
   * --------------
   */
  QVERIFY(setModelData(model, 2, 1, "eC"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 2);
  QCOMPARE(topLeft.column(), 1);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
  /*
   * Submit
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * | e || 3| eC |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Failed
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * | ! || 3| eC |
   * --------------
   */
  model.updateRecordInBackendFailed();
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
  /*
   * Submit again
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * | ! || 3| eC |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Done
   *
   * |Hdr||Id|Name|
   * --------------
   * |   || 1|  A |
   * --------------
   * |   || 2|  B |
   * --------------
   * |   || 3| eC |
   * --------------
   */
  model.updateRecordInBackendSucceeded();
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
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(2)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last
}

void CachedTableModelTest::setDataThenSubmitAndFromBackendInsertRecordsTest()
{
  EditPersonTableModel model;
  populatePersonStorage(model, {{1,"A"},{2,"B"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Edit
   */
  QVERIFY(setModelData(model, 1, 1, "eB"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Submit
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("e"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Insert from backend
   */
  model.fromBackendInsertRecords(0, 1, VariantRecord{9,"X"});
  QCOMPARE(getModelData(model, 0, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("e"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(isItemPending(model, 2, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Done
   */
  model.updateRecordInBackendSucceeded();
  QCOMPARE(getModelData(model, 0, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("eB"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("eB"));
}

void CachedTableModelTest::setDataThenSubmitAndFromBackendInsertRecordsSignalTest()
{
  QFAIL("Not complete");
}

void CachedTableModelTest::insertRowsTest()
{
  EditPersonTableModel model;
  QCOMPARE(model.columnCount(), 2);

  /*
   * Add to the end
   */
  QCOMPARE(model.rowCount(), 0);
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QVERIFY(setModelData(model, 0, 1, "A"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  /*
   * Insert at the beginning
   */
  QVERIFY(prependRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(setModelData(model, 0, 1, "X"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  /*
   * Insert in the middle
   */
  QVERIFY(model.insertRows(1, 1));
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(setModelData(model, 1, 1, "Z"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("X"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("Z"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("*"));
}

void CachedTableModelTest::insertRowsSignalTest()
{
  EditPersonTableModel model;
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditPersonTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditPersonTableModel::rowsInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Add to the end
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |    | < new
   * --------------
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
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
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Insert at the beginning
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |    | < new
   * --------------
   * | * ||  |    |
   * --------------
   */
  QVERIFY(prependRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
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
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Insert in the middle
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |    |
   * --------------
   * | * ||  |    | < new
   * --------------
   * | * ||  |    | < new
   * --------------
   * | * ||  |    |
   * --------------
   */
  QVERIFY(model.insertRows(1, 2));
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedTableModelTest::setDataThenInsertRowsTest()
{
  QFAIL("Not complete");
}

void CachedTableModelTest::setDataThenInsertRowsSignalTest()
{
  QFAIL("Not complete");
}

void CachedTableModelTest::insertRowsThenSetDataThenSubmitTest()
{
  EditPersonTableModel model;
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(model.rowCount(), 0);

  /*
   * Add to the end
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(getModelData(model, 0, 0).isNull());
  QVERIFY(setModelData(model, 0, 1, "A"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.storageCount(), 0);
  /*
   * Submit
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QVERIFY(isItemPending(model, 0, 1));
  QVERIFY(getModelData(model, 0, 0).isNull());
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.storageCount(), 0);
  /*
   * Submit done
   */
  model.addRecordToBackendSucceeded();
  QCOMPARE(model.rowCount(), 1);
  QVERIFY(!isItemPending(model, 0, 1));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  /*
   * Add to the end
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(setModelData(model, 1, 1, "B"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QVERIFY(getModelData(model, 1, 0).isNull());
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.storageCount(), 1);
  /*
   * Submit
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QVERIFY(getModelData(model, 1, 0).isNull());
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.storageCount(), 1);
  /*
   * Submit failed
   */
  model.addRecordToBackendFailed();
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QVERIFY(getModelData(model, 1, 0).isNull());
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("!"));
  QCOMPARE(model.storageCount(), 1);
  /*
   * Submit again
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QVERIFY(getModelData(model, 1, 0).isNull());
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("!"));
  QCOMPARE(model.storageCount(), 1);
  /*
   * Submit done
   */
  model.addRecordToBackendSucceeded();
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  /*
   * Add to the end
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
  QVERIFY(setModelData(model, 2, 1, "C"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QVERIFY(getModelData(model, 2, 0).isNull());
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.storageCount(), 2);
  /*
   * Submit
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(isItemPending(model, 2, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QVERIFY(getModelData(model, 2, 0).isNull());
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.storageCount(), 2);
  /*
   * Submit failed
   */
  model.addRecordToBackendFailed();
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QVERIFY(getModelData(model, 2, 0).isNull());
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("!"));
  QCOMPARE(model.storageCount(), 2);
  /*
   * Set data (could be a correction)
   */
  QVERIFY(setModelData(model, 2, 1, "C3"));
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QVERIFY(getModelData(model, 2, 0).isNull());
  QCOMPARE(getModelData(model, 2, 1), QVariant("C3"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("!"));
  QCOMPARE(model.storageCount(), 2);
  /*
   * Submit again
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(isItemPending(model, 2, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QVERIFY(getModelData(model, 2, 0).isNull());
  QCOMPARE(getModelData(model, 2, 1), QVariant("C3"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("!"));
  QCOMPARE(model.storageCount(), 2);
  /*
   * Submit done
   */
  model.addRecordToBackendSucceeded();
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C3"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C3"));
}

void CachedTableModelTest::insertRowsThenSetDataThenSubmitSignalTest()
{
  EditPersonTableModel model;
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditPersonTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditPersonTableModel::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Add to the end
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |  A |
   * --------------
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 1);
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
  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(setModelData(model, 0, 1, "A"));
  dataChangedSpy.clear();
  headerDataChangedSpy.clear();
  /*
   * Submit
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |  A |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  /*
   * Submit done
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 || 1|  A |
   * --------------
   */
  model.addRecordToBackendSucceeded();
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
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
  QVERIFY(roles.contains(Qt::DisplayRole));
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
}

template<typename Model>
void fromBackendRemoveRowsTest()
{
  Model model;
  populatePersonStorageByNames(model, {"A","B","C","D"});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("D"));

  model.fromBackendRemoveRows(1, 2);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("D"));

  model.fromBackendRemoveRows(1, 1);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));

  model.fromBackendRemoveRows(0, 1);
  QCOMPARE(model.rowCount(), 0);
}

void CachedTableModelTest::readOnlyFromBackendRemoveRowsTest()
{
  fromBackendRemoveRowsTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendRemoveRowsTest()
{
  fromBackendRemoveRowsTest<EditPersonTableModel>();
}

template<typename Model>
void fromBackendRemoveRowsSignalTest()
{
  Model model;
  populatePersonStorageByNames(model, {"A","B","C","D"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QSignalSpy rowsAboutToBeRemovedSpy(&model, &Model::rowsAboutToBeRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QSignalSpy rowsRemovedSpy(&model, &Model::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * |   ||  |  A |
   * --------------
   * |   ||  |  B |
   * --------------
   * |   ||  |  C |
   * --------------
   * |   ||  |  D |
   * --------------
   */
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  /*
   * Remove in the middle
   *
   * |Hdr||Id|Name|
   * --------------
   * |   ||  |  A |
   * --------------
   * |   ||  |  D |
   * --------------
   */
  model.fromBackendRemoveRows(1, 2);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last

  /*
   * Remove last
   *
   * |Hdr||Id|Name|
   * --------------
   * |   ||  |  A |
   * --------------
   */
  model.fromBackendRemoveRows(1, 1);
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

  /*
   * Remove first
   *
   * |Hdr||Id|Name|
   * --------------
   */
  model.fromBackendRemoveRows(0, 1);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
}

void CachedTableModelTest::readOnlyFromBackendRemoveRowsSignalTest()
{
  fromBackendRemoveRowsSignalTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFromBackendRemoveRowsSignalTest()
{
  fromBackendRemoveRowsSignalTest<EditPersonTableModel>();
}

template<typename Model>
void fetchRowThenFromBackendRemoveRowsTest()
{
  Model model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(!isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));

  model.updateStoragePersonNameAt(1, "uB");
  QVERIFY(model.fetchRow(1));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));
  QVERIFY(!isItemPending(model, 2, 1));

  model.fromBackendRemoveRows(2, 1);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QVERIFY(!isItemPending(model, 0, 1));
  QVERIFY(isItemPending(model, 1, 1));

  model.fromBackendRemoveRows(0, 1);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("B"));
  QVERIFY(isItemPending(model, 0, 1));

  model.fetchRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("uB"));
  QVERIFY(!isItemPending(model, 0, 1));
}

void CachedTableModelTest::readOnlyFetchRowThenFromBackendRemoveRowsTest()
{
  fetchRowThenFromBackendRemoveRowsTest<ListPersonTableModel>();
}

void CachedTableModelTest::editableFetchRowThenFromBackendRemoveRowsTest()
{
  fetchRowThenFromBackendRemoveRowsTest<EditPersonTableModel>();
}

void CachedTableModelTest::removeRowsTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C","D"});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("D"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QVERIFY(!isItemRemoved(model, 2, 1));
  QVERIFY(!isItemRemoved(model, 3, 1));

  QVERIFY(model.removeRows(1, 2));
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("D"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant(4));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QVERIFY(isItemRemoved(model, 2, 1));
  QVERIFY(!isItemRemoved(model, 3, 1));

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("D"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QVERIFY(isItemRemoved(model, 2, 1));
  QVERIFY(isItemRemoved(model, 3, 1));

  QVERIFY(removeFirstRowFromModel(model));
  QCOMPARE(model.rowCount(), 4);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(getModelData(model, 3, 1), QVariant("D"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(3, Qt::Vertical), QVariant("x"));
  QVERIFY(isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QVERIFY(isItemRemoved(model, 2, 1));
  QVERIFY(isItemRemoved(model, 3, 1));
}

void CachedTableModelTest::removeRowsThenSubmitTest()
{
  EditPersonTableModel model;
  populatePersonStorage(model, {{1,"A"},{2,"B"},{3,"C"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QVERIFY(!isItemRemoved(model, 2, 1));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C"));

  QVERIFY(model.removeRow(1));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QVERIFY(!isItemRemoved(model, 2, 1));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C"));

  model.removeRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(3), QString("C"));
}

void CachedTableModelTest::removeRowsThenSubmitSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QSignalSpy rowsAboutToBeRemovedSpy(&model, &EditPersonTableModel::rowsAboutToBeRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QSignalSpy rowsRemovedSpy(&model, &EditPersonTableModel::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | 2 ||  |  B |
   * --------------
   * | 3 ||  |  C |
   * --------------
   */
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Remove rows
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | x ||  |  B |
   * --------------
   * | 3 ||  |  C |
   * --------------
   */
  QVERIFY(model.removeRow(1));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last

  /*
   * Submit changes
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | x ||  |  B |
   * --------------
   * | 3 ||  |  C |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | 3 ||  |  C |
   * --------------
   */
  model.removeRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 2);
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
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedTableModelTest::cancelRemoveRowsThenSubmitTest()
{
  EditPersonTableModel model;
  populatePersonStorage(model, {{1,"A"},{2,"B"},{3,"C"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QVERIFY(!isItemRemoved(model, 2, 1));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C"));

  QVERIFY(model.removeRows(0, 3));
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QVERIFY(isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QVERIFY(isItemRemoved(model, 2, 1));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C"));

  model.cancelRemoveRows(0, 2);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QVERIFY(isItemRemoved(model, 2, 1));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.storageCount(), 3);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
  QCOMPARE(model.storageNameForId(3), QString("C"));

  model.removeRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
}

void CachedTableModelTest::cancelRemoveRowsThenSubmitSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A","B","C"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QSignalSpy rowsAboutToBeRemovedSpy(&model, &EditPersonTableModel::rowsAboutToBeRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QSignalSpy rowsRemovedSpy(&model, &EditPersonTableModel::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | 2 ||  |  B |
   * --------------
   * | 3 ||  |  C |
   * --------------
   */
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Remove rows
   *
   * |Hdr||Id|Name|
   * --------------
   * | x ||  |  A |
   * --------------
   * | x ||  |  B |
   * --------------
   * | x ||  |  C |
   * --------------
   */
  QVERIFY(model.removeRows(0, 3));
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(2)); // last

  /*
   * Cancel remove rows
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | 2 ||  |  B |
   * --------------
   * | x ||  |  C |
   * --------------
   */
  model.cancelRemoveRows(0, 2);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last

  /*
   * Submit changes
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | 2 ||  |  B |
   * --------------
   * | x ||  |  C |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | 2 ||  |  B |
   * --------------
   */
  model.removeRecordFromBackendSucceeded();
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(2));             // first
  QCOMPARE(arguments.at(2), QVariant(2));             // last
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedTableModelTest::insertRowsRemoveRowsThenSubmitTest()
{
  EditPersonTableModel model;
  populatePersonStorage(model, {{1,"A"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(model.removeRows(0, 2));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QVERIFY(isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  model.removeRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.storageCount(), 0);

  populatePersonStorage(model, {{1,"A"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(prependRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant());
  QCOMPARE(getModelData(model, 1, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(model.removeRows(0, 2));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant());
  QCOMPARE(getModelData(model, 1, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QVERIFY(isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  model.removeRecordFromBackendSucceeded();
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.storageCount(), 0);
}

void CachedTableModelTest::insertRowsRemoveRowsThenSubmitSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditPersonTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditPersonTableModel::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());
  QSignalSpy rowsAboutToBeRemovedSpy(&model, &EditPersonTableModel::rowsAboutToBeRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QSignalSpy rowsRemovedSpy(&model, &EditPersonTableModel::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   */
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Insert row
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | * ||  |    |
   * --------------
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Remove rows
   *
   * |Hdr||Id|Name|
   * --------------
   * | x ||  |  A |
   * --------------
   * | x ||  |    |
   * --------------
   */
  QVERIFY(model.removeRows(0, 2));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last

  /*
   * Submit changes
   *
   * |Hdr||Id|Name|
   * --------------
   * | x ||  |  A |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
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
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   */
  model.removeRecordFromBackendSucceeded();
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(headerDataChangedSpy.count(), 0);

  populatePersonStorageByNames(model, {"A"});
  QVERIFY(model.fetchAll());
  rowsAboutToBeInsertedSpy.clear();
  rowsInsertedSpy.clear();

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   */
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Insert row
   *
   * |Hdr||Id|Name|
   * --------------
   * | * ||  |    |
   * --------------
   * | 2 ||  |  A |
   * --------------
   */
  QVERIFY(prependRowToModel(model));
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
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Remove rows
   *
   * |Hdr||Id|Name|
   * --------------
   * | x ||  |    |
   * --------------
   * | x ||  |  A |
   * --------------
   */
  QVERIFY(model.removeRows(0, 2));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last

  /*
   * Submit changes
   *
   * |Hdr||Id|Name|
   * --------------
   * | x ||  |  A |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   */
  model.removeRecordFromBackendSucceeded();
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last
  QCOMPARE(headerDataChangedSpy.count(), 0);
}

void CachedTableModelTest::insertRowsCancelRemoveRowsThenSetDataThenSubmitTest()
{
  EditPersonTableModel model;
  populatePersonStorage(model, {{1,"A"}});
  QVERIFY(model.fetchAll());

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(appendRowToModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(model.removeRows(0, 2));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant("x"));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("x"));
  QVERIFY(isItemRemoved(model, 0, 1));
  QVERIFY(isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  model.cancelRemoveRows(0, 2);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant());
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(setModelData(model, 1, 0, 2));
  QVERIFY(setModelData(model, 1, 1, "B"));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  QVERIFY(model.submitChanges());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant("*"));
  QCOMPARE(model.storageCount(), 1);
  QCOMPARE(model.storageNameForId(1), QString("A"));

  model.addRecordToBackendSucceeded();
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QVERIFY(!isItemRemoved(model, 0, 1));
  QVERIFY(!isItemRemoved(model, 1, 1));
  QCOMPARE(model.storageCount(), 2);
  QCOMPARE(model.storageNameForId(1), QString("A"));
  QCOMPARE(model.storageNameForId(2), QString("B"));
}

void CachedTableModelTest::insertRowsCancelRemoveRowsThenSetDataThenSubmitSignalTest()
{
  EditPersonTableModel model;
  populatePersonStorageByNames(model, {"A"});
  QVERIFY(model.fetchAll());
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QSignalSpy rowsAboutToBeInsertedSpy(&model, &EditPersonTableModel::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&model, &EditPersonTableModel::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());
  QSignalSpy rowsAboutToBeRemovedSpy(&model, &EditPersonTableModel::rowsAboutToBeRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QSignalSpy rowsRemovedSpy(&model, &EditPersonTableModel::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());
  QSignalSpy dataChangedSpy(&model, &EditPersonTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QSignalSpy headerDataChangedSpy(&model, &EditPersonTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  /*
   * Initial state
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   */
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Insert row
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | * ||  |    |
   * --------------
   */
  QVERIFY(appendRowToModel(model));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Remove rows
   *
   * |Hdr||Id|Name|
   * --------------
   * | x ||  |  A |
   * --------------
   * | x ||  |    |
   * --------------
   */
  QVERIFY(model.removeRows(0, 2));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last

  /*
   * Cancel remove rows
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | * ||  |    |
   * --------------
   */
  model.cancelRemoveRows(0, 2);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last

  /*
   * Set data
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | * ||  |  B |
   * --------------
   */
  QVERIFY(setModelData(model, 1, 1, "B"));
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 1);
  QCOMPARE(topLeft.column(), 1);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 1);
  QCOMPARE(bottomRight.column(), 1);
  headerDataChangedSpy.clear();

  /*
   * Submit changes
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | * ||  |  B |
   * --------------
   */
  QVERIFY(model.submitChanges());
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  QCOMPARE(headerDataChangedSpy.count(), 0);

  /*
   * Succeeded
   *
   * |Hdr||Id|Name|
   * --------------
   * | 1 ||  |  A |
   * --------------
   * | 2 || 2|  B |
   * --------------
   */
  model.addRecordToBackendSucceeded();
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 1);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 1);
  QCOMPARE(bottomRight.column(), 1);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CachedTableModelTest test;

  return QTest::qExec(&test, argc, argv);
}
