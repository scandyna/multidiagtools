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
#include "VariantTableModelTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QVariantList>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QComboBox>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::VariantTableModel;
using ItemModel::VariantTableModelRow;
using ItemModel::VariantTableModelItem;
using ItemModel::VariantTableModelStorageRule;

/*
 * Class used for flags tests
 */
class FlagsTestTableModel : public QAbstractTableModel
{
 public:

  explicit FlagsTestTableModel(QObject* parent = nullptr)
   : QAbstractTableModel(parent) {}

  int rowCount(const QModelIndex &) const override
  {
    return 1;
  }

  int columnCount(const QModelIndex&) const override
  {
    return 1;
  }

  QVariant data(const QModelIndex &, int) const override
  {
    return QVariant();
  }
};

/*
 * Helper functions
 */

Qt::ItemFlags getTableModelStandardFlags()
{
  FlagsTestTableModel model;
  QModelIndex index = model.index(0, 0);
  Q_ASSERT(index.isValid());
  return model.flags(index);
}

/*
 * Init/cleanup
 */

void VariantTableModelTest::initTestCase()
{
}

void VariantTableModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void VariantTableModelTest::commonDataItemTest()
{
  /*
   * Initial state
   */
  VariantTableModelItem data(VariantTableModelStorageRule::GroupDisplayAndEditRoleData);
  QVERIFY(!data.isEditRoleDataSeparate());
  /*
   * Set display role data
   */
  data.setDisplayRoleData("DA");
  QCOMPARE(data.displayRoleData(), QVariant("DA"));
  QCOMPARE(data.editRoleData(), QVariant("DA"));
  /*
   * Set edit role data
   */
  data.setEditRoleData("EA");
  QCOMPARE(data.displayRoleData(), QVariant("EA"));
  QCOMPARE(data.editRoleData(), QVariant("EA"));
}

void VariantTableModelTest::separateEditDataItemTest()
{
  /*
   * Initial state
   */
  VariantTableModelItem data(VariantTableModelStorageRule::SeparateDisplayAndEditRoleData);
  QVERIFY(data.isEditRoleDataSeparate());
  /*
   * Set display role data
   */
  data.setDisplayRoleData("DA");
  QCOMPARE(data.displayRoleData(), QVariant("DA"));
  QCOMPARE(data.data(Qt::DisplayRole), QVariant("DA"));
  QVERIFY(data.editRoleData().isNull());
  QVERIFY(data.data(Qt::EditRole).isNull());
  /*
   * Set edit role data
   */
  data.setEditRoleData("EA");
  QCOMPARE(data.displayRoleData(), QVariant("DA"));
  QCOMPARE(data.data(Qt::DisplayRole), QVariant("DA"));
  QCOMPARE(data.editRoleData(), QVariant("EA"));
  QCOMPARE(data.data(Qt::EditRole), QVariant("EA"));
  /*
   * Check setData()
   */
  data.setData("DB", Qt::DisplayRole);
  data.setData("EB", Qt::EditRole);
  QCOMPARE(data.displayRoleData(), QVariant("DB"));
  QCOMPARE(data.data(Qt::DisplayRole), QVariant("DB"));
  QCOMPARE(data.editRoleData(), QVariant("EB"));
  QCOMPARE(data.data(Qt::EditRole), QVariant("EB"));
}

void VariantTableModelTest::itemFlagsTest()
{
  Qt::ItemFlags flags;
  Qt::ItemFlags expectedFlags;
  /*
   * Initial state
   */
  VariantTableModelItem item(VariantTableModelStorageRule::SeparateDisplayAndEditRoleData);
  QVERIFY(item.isEnabled());
  QVERIFY(item.isEditable());
  // Check with no flags at input
  flags = Qt::NoItemFlags;
  expectedFlags = (Qt::ItemIsEnabled | Qt::ItemIsEditable);
  QCOMPARE(item.flags(flags), expectedFlags);
  // Check with other flags set at input
  flags = Qt::ItemIsSelectable;
  expectedFlags = (Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
  QCOMPARE(item.flags(flags), expectedFlags);
  /*
   * Change enabled flag
   */
  item.setEnabled(false);
  QVERIFY(!item.isEnabled());
  // Check with no flags at input
  flags = Qt::NoItemFlags;
  expectedFlags = (Qt::ItemIsEditable);
  QCOMPARE(item.flags(flags), expectedFlags);
  // Check with other flags set at input
  flags = Qt::ItemIsSelectable;
  expectedFlags = (Qt::ItemIsEditable | Qt::ItemIsSelectable);
  QCOMPARE(item.flags(flags), expectedFlags);
  /*
   * Change editable flag
   */
  item.setEditable(false);
  QVERIFY(!item.isEditable());
  // Check with no flags at input
  flags = Qt::NoItemFlags;
  expectedFlags = (Qt::NoItemFlags);
  QCOMPARE(item.flags(flags), expectedFlags);
  // Check with other flags set at input
  flags = Qt::ItemIsSelectable;
  expectedFlags = (Qt::ItemIsSelectable);
  QCOMPARE(item.flags(flags), expectedFlags);
}

void VariantTableModelTest::dataRowTest()
{
  Qt::ItemFlags flags = Qt::NoItemFlags;
  Qt::ItemFlags expectedFlags = (Qt::ItemIsEnabled | Qt::ItemIsEditable);
  /*
   * Initial state
   */
  VariantTableModelRow row(VariantTableModelStorageRule::SeparateDisplayAndEditRoleData, 3);
  QCOMPARE(row.columnCount(), 3);
  QVERIFY(row.data(0, Qt::DisplayRole).isNull());
  QVERIFY(row.data(0, Qt::EditRole).isNull());
  QVERIFY(row.data(1, Qt::DisplayRole).isNull());
  QVERIFY(row.data(1, Qt::EditRole).isNull());
  QVERIFY(row.data(2, Qt::DisplayRole).isNull());
  QVERIFY(row.data(2, Qt::EditRole).isNull());
  QCOMPARE(row.flags(0, flags), expectedFlags);
  QCOMPARE(row.flags(1, flags), expectedFlags);
  QCOMPARE(row.flags(2, flags), expectedFlags);
  /*
   * Set data
   */
  row.setData(2, "DA2", Qt::DisplayRole);
  row.setData(2, "EA2", Qt::EditRole);
  QCOMPARE(row.columnCount(), 3);
  QVERIFY(row.data(0, Qt::DisplayRole).isNull());
  QVERIFY(row.data(0, Qt::EditRole).isNull());
  QVERIFY(row.data(1, Qt::DisplayRole).isNull());
  QVERIFY(row.data(1, Qt::EditRole).isNull());
  QCOMPARE(row.data(2, Qt::DisplayRole) , QVariant("DA2"));
  QCOMPARE(row.data(2, Qt::EditRole) , QVariant("EA2"));
}

void VariantTableModelTest::dataRowInsertColumnsTest()
{
  VariantTableModelStorageRule storageRule = VariantTableModelStorageRule::GroupDisplayAndEditRoleData;
  VariantTableModelRow row(storageRule, 1);
  QCOMPARE(row.columnCount(), 1);
  /*
   * Initial state
   * |A|
   */
  QCOMPARE(row.columnCount(), 1);
  row.setData(0, "A", Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("A"));
  /*
   * Append 1 column
   * |A|B|
   */
  row.insertColumns(1, 1, storageRule);
  QCOMPARE(row.columnCount(), 2);
  row.setData(1, "B", Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("B"));
  /*
   * Append 2 columns
   * |A|B|C|D|
   */
  row.insertColumns(2, 2, storageRule);
  QCOMPARE(row.columnCount(), 4);
  row.setData(2, "C", Qt::DisplayRole);
  row.setData(3, "D", Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("D"));
  /*
   * Prepend 1 column
   * |9|A|B|C|D|
   */
  row.insertColumns(0, 1, storageRule);
  QCOMPARE(row.columnCount(), 5);
  row.setData(0, 9, Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant(9));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant("D"));
  /*
   * Prepend 2 columns
   * |7|8|9|A|B|C|D|
   */
  row.insertColumns(0, 2, storageRule);
  QCOMPARE(row.columnCount(), 7);
  row.setData(0, 7, Qt::DisplayRole);
  row.setData(1, 8, Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant(7));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant(8));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant(9));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(5, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(6, Qt::DisplayRole), QVariant("D"));
  /*
   * Insert 1 column before column 1
   * Before:
   * |7|8|9|A|B|C|D|
   *   ^
   * After:
   * |7|N|8|9|A|B|C|D|
   */
  row.insertColumns(1, 1, storageRule);
  QCOMPARE(row.columnCount(), 8);
  row.setData(1, "N", Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant(7));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("N"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant(8));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant(9));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(5, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(6, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(7, Qt::DisplayRole), QVariant("D"));
  /*
   * Insert 2 columns before column 1
   * Before:
   * |7|N|8|9|A|B|C|D|
   *   ^
   * After:
   * |7|L|M|N|8|9|A|B|C|D|
   */
  row.insertColumns(1, 2, storageRule);
  QCOMPARE(row.columnCount(), 10);
  row.setData(1, "L", Qt::DisplayRole);
  row.setData(2, "M", Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant(7));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("L"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("M"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("N"));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant(8));
  QCOMPARE(row.data(5, Qt::DisplayRole), QVariant(9));
  QCOMPARE(row.data(6, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(7, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(8, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(9, Qt::DisplayRole), QVariant("D"));
}

void VariantTableModelTest::dataRowRemoveColumnsTest()
{
  VariantTableModelRow row(VariantTableModelStorageRule::GroupDisplayAndEditRoleData, 10);
  QCOMPARE(row.columnCount(), 10);
  /*
   * Initial state
   * |A|B|C|D|E|F|G|H|I|J|
   */
  QCOMPARE(row.columnCount(), 10);
  row.setData(0, "A", Qt::DisplayRole);
  row.setData(1, "B", Qt::DisplayRole);
  row.setData(2, "C", Qt::DisplayRole);
  row.setData(3, "D", Qt::DisplayRole);
  row.setData(4, "E", Qt::DisplayRole);
  row.setData(5, "F", Qt::DisplayRole);
  row.setData(6, "G", Qt::DisplayRole);
  row.setData(7, "H", Qt::DisplayRole);
  row.setData(8, "I", Qt::DisplayRole);
  row.setData(9, "J", Qt::DisplayRole);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("D"));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant("E"));
  QCOMPARE(row.data(5, Qt::DisplayRole), QVariant("F"));
  QCOMPARE(row.data(6, Qt::DisplayRole), QVariant("G"));
  QCOMPARE(row.data(7, Qt::DisplayRole), QVariant("H"));
  QCOMPARE(row.data(8, Qt::DisplayRole), QVariant("I"));
  QCOMPARE(row.data(9, Qt::DisplayRole), QVariant("J"));
  /*
   * Remove 1 column at end
   * |A|B|C|D|E|F|G|H|I|
   */
  row.removeColumns(9, 1);
  QCOMPARE(row.columnCount(), 9);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("D"));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant("E"));
  QCOMPARE(row.data(5, Qt::DisplayRole), QVariant("F"));
  QCOMPARE(row.data(6, Qt::DisplayRole), QVariant("G"));
  QCOMPARE(row.data(7, Qt::DisplayRole), QVariant("H"));
  QCOMPARE(row.data(8, Qt::DisplayRole), QVariant("I"));
  /*
   * Remove 2 columns at end
   * |A|B|C|D|E|F|G|
   */
  row.removeColumns(7, 2);
  QCOMPARE(row.columnCount(), 7);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("D"));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant("E"));
  QCOMPARE(row.data(5, Qt::DisplayRole), QVariant("F"));
  QCOMPARE(row.data(6, Qt::DisplayRole), QVariant("G"));
  /*
   * Remove 1 column at begin
   * |B|C|D|E|F|G|
   */
  row.removeColumns(0, 1);
  QCOMPARE(row.columnCount(), 6);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("D"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("E"));
  QCOMPARE(row.data(4, Qt::DisplayRole), QVariant("F"));
  QCOMPARE(row.data(5, Qt::DisplayRole), QVariant("G"));
  /*
   * Remove 2 columns at begin
   * |D|E|F|G|
   */
  row.removeColumns(0, 2);
  QCOMPARE(row.columnCount(), 4);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("D"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("E"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("F"));
  QCOMPARE(row.data(3, Qt::DisplayRole), QVariant("G"));
  /*
   * Remove E
   * |D|F|G|
   */
  row.removeColumns(1, 1);
  QCOMPARE(row.columnCount(), 3);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("D"));
  QCOMPARE(row.data(1, Qt::DisplayRole), QVariant("F"));
  QCOMPARE(row.data(2, Qt::DisplayRole), QVariant("G"));
  /*
   * Remove F and G
   * |D|
   */
  row.removeColumns(1, 2);
  QCOMPARE(row.columnCount(), 1);
  QCOMPARE(row.data(0, Qt::DisplayRole), QVariant("D"));
}

void VariantTableModelTest::tableModelTest()
{
  QModelIndex index;

  /*
   * Initial state
   */
  VariantTableModel model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  /*
   * Populate
   */
  model.populate(3, 2);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check row 0 - also check data(index, role)
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("0A"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("0A"));
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("0B"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("0B"));
  // Check row 1 - also check data(row, column, role)
  QCOMPARE(model.data(1, 0, Qt::DisplayRole), QVariant("1A"));
  QCOMPARE(model.data(1, 0, Qt::EditRole), QVariant("1A"));
  QCOMPARE(model.data(1, 1, Qt::DisplayRole), QVariant("1B"));
  QCOMPARE(model.data(1, 1, Qt::EditRole), QVariant("1B"));
  // Check row 2
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("2A"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("2A"));
  index = model.index(2, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("2B"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("2B"));
  /*
   * Set data
   */
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  // Set DisplayRole data
  QVERIFY(model.setData(index, "D 1A", Qt::DisplayRole));
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("D 1A"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("D 1A"));
  // Set EditRole data
  QVERIFY(model.setData(index, "E 1A", Qt::EditRole));
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("E 1A"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("E 1A"));
  /*
   * Clear
   */
  model.clear();
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
}

void VariantTableModelTest::tableModelSeparateEditTest()
{
  QModelIndex index;

  /*
   * Initial state
   */
  VariantTableModel model(VariantTableModelStorageRule::SeparateDisplayAndEditRoleData);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  /*
   * Populate
   */
  model.populate(3, 2);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check row 0
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("0A"));
  QVERIFY(model.data(index, Qt::EditRole).isNull());
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("0B"));
  QVERIFY(model.data(index, Qt::EditRole).isNull());
  // Check row 1
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("1A"));
  QVERIFY(model.data(index, Qt::EditRole).isNull());
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("1B"));
  QVERIFY(model.data(index, Qt::EditRole).isNull());
  // Check row 2
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("2A"));
  QVERIFY(model.data(index, Qt::EditRole).isNull());
  index = model.index(2, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("2B"));
  QVERIFY(model.data(index, Qt::EditRole).isNull());
  /*
   * Set data
   */
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  // Set DisplayRole data
  QVERIFY(model.setData(index, "D 1A", Qt::DisplayRole));
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("D 1A"));
  QVERIFY(model.data(index, Qt::EditRole).isNull());
  // Set EditRole data
  QVERIFY(model.setData(index, "E 1A", Qt::EditRole));
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("D 1A"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("E 1A"));
  /*
   * Clear
   */
  model.clear();
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
}

void VariantTableModelTest::tableModelFlagTest()
{
  QModelIndex index;
  VariantTableModel model;
  const Qt::ItemFlags standardFlags = Qt::ItemFlags(getTableModelStandardFlags() | Qt::ItemIsEditable);
  Qt::ItemFlags flags;

  model.populate(2, 2);
  /*
   * Check without having specified any flag
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  /*
   * Set a item disabled
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, false);
  flags = standardFlags & Qt::ItemFlags(~Qt::ItemIsEnabled);
  QCOMPARE(model.flags(index), flags);
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  /*
   * Re-enable item
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, true);
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  /*
   * Set a item not editable
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  flags = Qt::ItemFlags(getTableModelStandardFlags());
  QCOMPARE(model.flags(index), flags);
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  /*
   * Set item editable again
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, true);
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.flags(index), standardFlags);
}

void VariantTableModelTest::tableModelSignalTest()
{
  QModelIndex index;
  QVariantList arguments;
  /*
   * Setup model and signal spies
   */
  VariantTableModel model;
  QSignalSpy resetSpy(&model, &VariantTableModel::modelReset);
  QSignalSpy dataChangedSpy(&model, &VariantTableModel::dataChanged);
  QVERIFY(resetSpy.isValid());
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Resize
   */
  QCOMPARE(resetSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 0);
  model.resize(2, 1);
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);
  /*
   * Set data
   */
  resetSpy.clear();
  dataChangedSpy.clear();
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, "A");
  QCOMPARE(resetSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  index = arguments.at(0).toModelIndex();
  QVERIFY(index.isValid());
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 0);
  QCOMPARE( arguments.at(1).toModelIndex(), index );
  /*
   * populateColumnWithInt
   */
  resetSpy.clear();
  dataChangedSpy.clear();
  model.populateColumnWithInt(0, 1);
  QCOMPARE(resetSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 2);
  /*
   * populateColumnWithAscii
   */
  resetSpy.clear();
  dataChangedSpy.clear();
  model.populateColumnWithAscii(0, 'A');
  QCOMPARE(resetSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 2);
  /*
   * populateColumn
   */
  resetSpy.clear();
  dataChangedSpy.clear();
  model.populateColumn(0, {10,55});
  QCOMPARE(resetSpy.count(), 0);
  QCOMPARE(dataChangedSpy.count(), 2);
  /*
   * populate
   */
  resetSpy.clear();
  dataChangedSpy.clear();
  model.populate(3, 1);
  QCOMPARE(resetSpy.count(), 1);
  QCOMPARE(dataChangedSpy.count(), 0);
}

void VariantTableModelTest::tableModelViewTest()
{
  VariantTableModel model;
  QModelIndex index;
  QTableView tableView;
  QTreeView treeView;
  QListView listView;
  QComboBox comboBox;

  /*
   * Setup views
   */
  tableView.setModel(&model);
  tableView.resize(300, 200);
  tableView.show();
  treeView.setModel(&model);
  treeView.resize(300, 200);
  treeView.show();
  listView.setModel(&model);
  listView.resize(150, 200);
  listView.show();
  comboBox.setModel(&model);
  comboBox.show();
  /*
   * Populate model
   */
  model.populate(3, 2);
  /*
   * Set some flags
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEnabled(index, false);
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);

  /*
   * Play
   */
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
}

void VariantTableModelTest::tableModelResizeTest()
{
  VariantTableModel model;
  QModelIndex index;

  /*
   * Resize with more
   */
  // Initial state
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  // Resize and check that we have null variants
  model.resize(1, 2);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  // Set some data
  index = model.index(0, 0);
  model.setData(index, 1);
  index = model.index(0, 1);
  model.setData(index, "A");
  // Add a row
  model.resize(2, 2);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.columnCount(), 2);
  // Check that existing data are still here
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant(1));
  index = model.index(0, 1);
  QCOMPARE(model.data(index), QVariant("A"));
  // Check that new rows has null variants
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  /*
   * Resize with less
   */
  model.resize(1, 1);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 1);
  // Check that existing data are still here
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant(1));
}

void VariantTableModelTest::tableModelPopulateColumnTest()
{
  VariantTableModel model(VariantTableModelStorageRule::SeparateDisplayAndEditRoleData);
  QModelIndex index;

  model.resize(3, 2);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  /*
   * Populate column 0 with ints
   */
  // Default increment (1)
  model.populateColumnWithInt(0, 1);
  // Check that model still the same dimentions
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check data
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant(1));
  index = model.index(1, 0);
  QCOMPARE(model.data(index), QVariant(2));
  index = model.index(2, 0);
  QCOMPARE(model.data(index), QVariant(3));
  // Other increment
  model.populateColumnWithInt(0, 2, 4);
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant(2));
  index = model.index(1, 0);
  QCOMPARE(model.data(index), QVariant(6));
  index = model.index(2, 0);
  QCOMPARE(model.data(index), QVariant(10));
  /*
   * Populate column 1 with ASCII
   */
  // Default increment (1)
  model.populateColumnWithAscii(1, 'A');
  // Check that model still the same dimentions
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check data
  index = model.index(0, 1);
  QCOMPARE(model.data(index), QVariant("A"));
  index = model.index(1, 1);
  QCOMPARE(model.data(index), QVariant("B"));
  index = model.index(2, 1);
  QCOMPARE(model.data(index), QVariant("C"));
  // Other increment
  model.populateColumnWithAscii(1, 'b', 3);
  index = model.index(0, 1);
  QCOMPARE(model.data(index), QVariant("b"));
  index = model.index(1, 1);
  QCOMPARE(model.data(index), QVariant("e"));
  index = model.index(2, 1);
  QCOMPARE(model.data(index), QVariant("h"));
  /*
   * Populate column 0 with data
   */
  model.populateColumn(0, {53,"BEF", "P"});
  // Check that model still the same dimentions
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check data
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant(53));
  index = model.index(1, 0);
  QCOMPARE(model.data(index), QVariant("BEF"));
  index = model.index(2, 0);
  QCOMPARE(model.data(index), QVariant("P"));
  // Check partial population
  model.populateColumn(0, {"A",7});
  // Check that model still the same dimentions
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check data
  index = model.index(0, 0);
  QCOMPARE(model.data(index), QVariant("A"));
  index = model.index(1, 0);
  QCOMPARE(model.data(index), QVariant(7));
  index = model.index(2, 0);
  QCOMPARE(model.data(index), QVariant("P"));
  /*
   * Populate column 0 with data
   *  -> EditRole
   */
  model.populateColumn(0, {"A","B","C"}, Qt::DisplayRole);
  model.populateColumn(0, {1,2,3}, Qt::EditRole);
  // Check that model still the same dimentions
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check data
  index = model.index(0, 0);
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant(1));
  index = model.index(1, 0);
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant(2));
  index = model.index(2, 0);
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("C"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant(3));
}

void VariantTableModelTest::tableModelInsertColumnsTest()
{
  VariantTableModel model;
  QModelIndex index;

  /*
   * Populate model
   */
  model.resize(3, 1);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 1);
  model.populateColumn(0, {1,2,3});
  QCOMPARE(model.data(0, 0), QVariant(1));
  QCOMPARE(model.data(1, 0), QVariant(2));
  QCOMPARE(model.data(2, 0), QVariant(3));
  /*
   * Append a column
   */
  model.appendColumn();
  QCOMPARE(model.columnCount(), 2);
  // Check that initial data are keeped untouched
  QCOMPARE(model.data(0, 0), QVariant(1));
  QCOMPARE(model.data(1, 0), QVariant(2));
  QCOMPARE(model.data(2, 0), QVariant(3));
  // Check that new column can be used
  model.populateColumn(1, {"A","B","C"});
  QCOMPARE(model.data(0, 1), QVariant("A"));
  QCOMPARE(model.data(1, 1), QVariant("B"));
  QCOMPARE(model.data(2, 1), QVariant("C"));
  /*
   * Prepend a column
   */
  model.prependColumn();
  QCOMPARE(model.columnCount(), 3);
  // New column is before, so our data are now shifted
  QVERIFY(model.data(0, 0).isNull());
  QCOMPARE(model.data(0, 1), QVariant(1));
  QCOMPARE(model.data(0, 2), QVariant("A"));
  // Check that new column can be used
  model.populateColumn(0, {"E","F","G"});
  QCOMPARE(model.data(0, 0), QVariant("E"));
  QCOMPARE(model.data(1, 0), QVariant("F"));
  QCOMPARE(model.data(2, 0), QVariant("G"));
  /*
   * Insert a column in the middle
   * Initial (data at row 0):
   * -------------
   * | E | 1 | A |
   * -------------
   *      ^
   * New:
   * -------------------
   * | E | New | 1 | A |
   * -------------------
   */
  model.insertColumn(1);
  QCOMPARE(model.columnCount(), 4);
  // Check that column was inserted at correct place
  QCOMPARE(model.data(0, 0), QVariant("E"));
  QVERIFY(model.data(0, 1).isNull());
  QCOMPARE(model.data(0, 2), QVariant(1));
  QCOMPARE(model.data(0, 3), QVariant("A"));
  // Check that new column can be used
  model.populateColumn(1, {"I","J","K"});
  QCOMPARE(model.data(0, 1), QVariant("I"));
  QCOMPARE(model.data(1, 1), QVariant("J"));
  QCOMPARE(model.data(2, 1), QVariant("K"));
  /*
   * Insert 2 columns in the middle
   * Initial (data at row 0):
   * ---------
   * |E|I|1|A|
   * ---------
   *       ^
   * New:
   * -------------
   * |E|I|1|2|3|A|
   * -------------
   */
  model.insertColumns(3, 2);
  QCOMPARE(model.columnCount(), 6);
  // Check that column was inserted at correct place
  QCOMPARE(model.data(0, 0), QVariant("E"));
  QCOMPARE(model.data(0, 1), QVariant("I"));
  QCOMPARE(model.data(0, 2), QVariant(1));
  QVERIFY(model.data(0, 3).isNull());
  QVERIFY(model.data(0, 4).isNull());
  QCOMPARE(model.data(0, 5), QVariant("A"));
  // Check that new columns can be used
  model.setData(0, 3, 2);
  model.setData(0, 4, 3);
  QCOMPARE(model.data(0, 0), QVariant("E"));
  QCOMPARE(model.data(0, 1), QVariant("I"));
  QCOMPARE(model.data(0, 2), QVariant(1));
  QCOMPARE(model.data(0, 3), QVariant(2));
  QCOMPARE(model.data(0, 4), QVariant(3));
  QCOMPARE(model.data(0, 5), QVariant("A"));
}

void VariantTableModelTest::tableModelRemoveColumnsTest()
{
  VariantTableModel model;
  QModelIndex index;

  /*
   * Populate model
   * -------------
   * |A|B|C|D|E|F|
   * -------------
   */
  model.resize(1, 6);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 6);
  model.setData(0, 0, "A");
  model.setData(0, 1, "B");
  model.setData(0, 2, "C");
  model.setData(0, 3, "D");
  model.setData(0, 4, "E");
  model.setData(0, 5, "F");
  QCOMPARE(model.data(0, 0), QVariant("A"));
  QCOMPARE(model.data(0, 1), QVariant("B"));
  QCOMPARE(model.data(0, 2), QVariant("C"));
  QCOMPARE(model.data(0, 3), QVariant("D"));
  QCOMPARE(model.data(0, 4), QVariant("E"));
  QCOMPARE(model.data(0, 5), QVariant("F"));
  /*
   * Remove last column
   * -----------
   * |A|B|C|D|E|
   * -----------
   */
  model.removeLastColumn();
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 5);
  QCOMPARE(model.data(0, 0), QVariant("A"));
  QCOMPARE(model.data(0, 1), QVariant("B"));
  QCOMPARE(model.data(0, 2), QVariant("C"));
  QCOMPARE(model.data(0, 3), QVariant("D"));
  QCOMPARE(model.data(0, 4), QVariant("E"));
  /*
   * Remove first column
   * ---------
   * |B|C|D|E|
   * ---------
   */
  model.removeFirstColumn();
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 4);
  QCOMPARE(model.data(0, 0), QVariant("B"));
  QCOMPARE(model.data(0, 1), QVariant("C"));
  QCOMPARE(model.data(0, 2), QVariant("D"));
  QCOMPARE(model.data(0, 3), QVariant("E"));
  /*
   * Remove C
   * ---------
   * |B|D|E|
   * ---------
   */
  model.removeColumn(1);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 3);
  QCOMPARE(model.data(0, 0), QVariant("B"));
  QCOMPARE(model.data(0, 1), QVariant("D"));
  QCOMPARE(model.data(0, 2), QVariant("E"));
  /*
   * Remove D and E
   * ---
   * |B|
   * ---
   */
  model.removeColumns(1, 2);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 1);
  QCOMPARE(model.data(0, 0), QVariant("B"));
}

void VariantTableModelTest::tableModelChangeColumnsCountSignalTest()
{
  QModelIndex index;
  QVariantList arguments;
  /*
   * Setup model and signal spies
   */
  VariantTableModel model;
  QSignalSpy insertSpy(&model, &VariantTableModel::columnsInserted);
  QSignalSpy removeSpy(&model, &VariantTableModel::columnsRemoved);
  QVERIFY(insertSpy.isValid());
  QVERIFY(removeSpy.isValid());

  model.resize(1, 1);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 1);
  insertSpy.clear();
  removeSpy.clear();
  /*
   * Append a column
   */
  model.appendColumn();
  QCOMPARE(insertSpy.count(), 1);
  QCOMPARE(removeSpy.count(), 0);
  arguments = insertSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid());
  QCOMPARE(arguments.at(1), QVariant(1));
  QCOMPARE(arguments.at(2), QVariant(1));
  /*
   * Remove a column
   */
  model.removeLastColumn();
  QCOMPARE(insertSpy.count(), 0);
  QCOMPARE(removeSpy.count(), 1);
  arguments = removeSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid());
  QCOMPARE(arguments.at(1), QVariant(1));
  QCOMPARE(arguments.at(2), QVariant(1));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  VariantTableModelTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
