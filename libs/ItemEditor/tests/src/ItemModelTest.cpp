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
#include "ItemModelTest.h"
#include "Mdt/Application.h"
#include "TestTableModel.h"
#include <QSignalSpy>
// #include <QItemSelectionModel>
// #include <QStringListModel>
#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>

#include <QDebug>

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

void ItemModelTest::initTestCase()
{
}

void ItemModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ItemModelTest::testTableModelCommonItemDataTest()
{
  /*
   * Initial state
   */
  TestTableModelItemData data(TestTableModelRoleStorage::GroupDisplayAndEditRoleData);
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

void ItemModelTest::testTableModelSeparateEditItemDataTest()
{
  /*
   * Initial state
   */
  TestTableModelItemData data(TestTableModelRoleStorage::SeparateDisplayAndEditRoleData);
  QVERIFY(data.isEditRoleDataSeparate());
  /*
   * Set display role data
   */
  data.setDisplayRoleData("DA");
  QCOMPARE(data.displayRoleData(), QVariant("DA"));
  QVERIFY(data.editRoleData().isNull());
  /*
   * Set edit role data
   */
  data.setEditRoleData("EA");
  QCOMPARE(data.displayRoleData(), QVariant("DA"));
  QCOMPARE(data.editRoleData(), QVariant("EA"));
}

void ItemModelTest::testTableModelItemFlagsTest()
{
  /*
   * Initial state
   */
  TestTableModelItemData item(TestTableModelRoleStorage::SeparateDisplayAndEditRoleData);
  QVERIFY(item.isEnabled());
  QVERIFY(item.isEditable());
  /*
   * Change enabled flag
   */
  item.setEnabled(false);
  QVERIFY(!item.isEnabled());
  /*
   * Change editable flag
   */
  item.setEditable(false);
  QVERIFY(!item.isEditable());
}

void ItemModelTest::testTableModelTest()
{
  QModelIndex index;

  /*
   * Initial state
   */
  TestTableModel model;
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
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("0A"));
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("0B"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("0B"));
  // Check row 1
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("1A"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("1A"));
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("1B"));
  QCOMPARE(model.data(index, Qt::EditRole), QVariant("1B"));
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

void ItemModelTest::testTableModelSeparateEditTest()
{
  QModelIndex index;

  /*
   * Initial state
   */
  TestTableModel model(TestTableModelRoleStorage::SeparateDisplayAndEditRoleData);
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

void ItemModelTest::testTableModelFlagTest()
{
  QModelIndex index;
  TestTableModel model;
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

void ItemModelTest::testTableModelViewTest()
{
  TestTableModel model;
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
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ItemModelTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
