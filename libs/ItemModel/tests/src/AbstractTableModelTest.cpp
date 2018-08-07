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
#include "AbstractTableModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/AbstractTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/TestLib/ItemModelInsertRowTest.h"
#include "Mdt/TestLib/ItemModelRemoveRowTest.h"
#include "Mdt/Container/StlContainer.h"
#include <QSignalSpy>
#include <QVariant>
#include <QVariantList>
#include <QMetaType>
#include <vector>
#include <array>

// #include <QDebug>

// using namespace Mdt::ItemModel;
using namespace Mdt::TestLib;
// using namespace Mdt::Container;

/*
 * A read only table model
 */
class ReadOnlyTableModel : public Mdt::ItemModel::AbstractTableModel
{
 public:

  using Record = std::array<QVariant, 2>;
  using Table = std::vector<Record>;

  using ParentClass = AbstractTableModel;
  using ParentClass::emitDataChanged;
  using ParentClass::emitVerticalHeaderDataChanged;

  ReadOnlyTableModel(QObject *parent = nullptr)
   : AbstractTableModel(parent)
  {
  }

  void setTable(const Table & table)
  {
    mTable = table;
  }

 protected:

  Table & table()
  {
    return mTable;
  }

 private:

  int rowCountImpl() const override
  {
    return mTable.size();
  }

  int columnCountImpl() const override
  {
    return 2;
  }

  QVariant displayRoleData(int row, int column) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < rowCount());
    Q_ASSERT(column >= 0);
    Q_ASSERT(column < columnCount());

    return mTable[row][column];
  }

  bool insertRowsImpl(int row, int count) override
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row <= rowCount());
    Q_ASSERT(count >= 1);

    Mdt::Container::insertToContainer(mTable, row, count, Record());

    return true;
  }

  bool removeRowsImpl(int row, int count) override
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(count >= 1);
    Q_ASSERT( (row + count) <= rowCountImpl());

    Mdt::Container::removeFromContainer(mTable, row, count);

    return true;
  }

  Table mTable;
};

/*
 * A table model that defines no header
 */

class DefaultHeaderTableModel : public Mdt::ItemModel::AbstractTableModel
{
 public:

  int rowCountImpl() const override
  {
    return 0;
  }

  int columnCountImpl() const override
  {
    return 3;
  }

  QVariant displayRoleData(int, int) const override
  {
    return QVariant();
  }
};

/*
 * A table model that custom header
 */

class CustomHeaderTableModel : public Mdt::ItemModel::AbstractTableModel
{
 public:

  int rowCountImpl() const override
  {
    return 0;
  }

  int columnCountImpl() const override
  {
    return 3;
  }

  QVariant displayRoleData(int, int) const override
  {
    return QVariant();
  }

  QVariant horizontalHeaderDisplayRoleData(int column) const override
  {
    switch(column){
      case 0:
        return "A";
      case 1:
        return "B";
      case 2:
        return "C";
    }
    return QVariant();
  }
};

/*
 * A editable table model
 */
class EditableTableModel : public ReadOnlyTableModel
{
 public:

  using ParentClass = ReadOnlyTableModel;
  using ParentClass::emitDataChanged;
  using ParentClass::emitDataAtRowsChanged;

  EditableTableModel(QObject* parent = nullptr)
   : ReadOnlyTableModel(parent)
  {
    setModelEditable(true);
  }

 private:

  bool setEditRoleData(int row, int column, const QVariant& value) override
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < rowCount());
    Q_ASSERT(column >= 0);
    Q_ASSERT(column < columnCount());

    table()[row][column] = value;
    return true;
  }
};

/*
 * Helpers
 */

void populateModel(ReadOnlyTableModel & model, const ReadOnlyTableModel::Table & tableData)
{
  model.setTable(tableData);
}

/*
 * Tests
 */

void AbstractTableModelTest::initTestCase()
{
  // For QSignalSpy
  qRegisterMetaType<Qt::Orientation>();
}

void AbstractTableModelTest::readOnlyConstructTest()
{
  ReadOnlyTableModel model;
  QCOMPARE(model.columnCount(), 2);

  QObject parent;
  auto *dModel = new ReadOnlyTableModel(&parent);
  QCOMPARE(dModel->columnCount(), 2);
}

void AbstractTableModelTest::editableConstructTest()
{
  EditableTableModel model;
  QCOMPARE(model.columnCount(), 2);

  QObject parent;
  auto *dModel = new EditableTableModel(&parent);
  QCOMPARE(dModel->columnCount(), 2);
}

void AbstractTableModelTest::readOnlyRowCountTest()
{
  ReadOnlyTableModel model;

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.rowCount(QModelIndex()), 0);

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.rowCount(QModelIndex()), 1);
  QCOMPARE(model.rowCount(model.index(0, 0)), 0);
}

void AbstractTableModelTest::editableRowCountTest()
{
  EditableTableModel model;

  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.rowCount(QModelIndex()), 0);

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.rowCount(QModelIndex()), 1);
  QCOMPARE(model.rowCount(model.index(0, 0)), 0);
}

void AbstractTableModelTest::readOnlyColumnCountTest()
{
  ReadOnlyTableModel model;

  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(model.columnCount(QModelIndex()), 2);

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(model.columnCount(QModelIndex()), 2);
  QCOMPARE(model.columnCount(model.index(0, 0)), 0);
}

void AbstractTableModelTest::editableColumnCountTest()
{
  EditableTableModel model;

  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(model.columnCount(QModelIndex()), 2);

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(model.columnCount(QModelIndex()), 2);
  QCOMPARE(model.columnCount(model.index(0, 0)), 0);
}

void AbstractTableModelTest::readOnlyFlagsTest()
{
  const auto defaultFlags = getDefaultQAbstractTableModelFlags();
  ReadOnlyTableModel model;
  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);

  populateModel(model, {{1,"A"}});
  QCOMPARE(getModelFlags(model, 0, 0), defaultFlags);
  QCOMPARE(getModelFlags(model, 0, 1), defaultFlags);
  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);
}

void AbstractTableModelTest::editableFlagsTest()
{
  const auto expectedFlags = getDefaultQAbstractTableModelFlags() | Qt::ItemIsEditable;
  EditableTableModel model;
  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);

  populateModel(model, {{1,"A"}});
  QCOMPARE(getModelFlags(model, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 0, 1), expectedFlags);
  QCOMPARE(model.flags(QModelIndex()), Qt::NoItemFlags);
}

void AbstractTableModelTest::defaultHeaderTest()
{
  DefaultHeaderTableModel model;
  QCOMPARE(model.columnCount(), 3);
  QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Horizontal), QVariant(3));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole), QVariant());
}

void AbstractTableModelTest::customHeaderTest()
{
  CustomHeaderTableModel model;
  QCOMPARE(model.columnCount(), 3);
  QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("A"));
  QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("B"));
  QCOMPARE(model.headerData(2, Qt::Horizontal), QVariant("C"));
  QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
  QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
  QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
  QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole), QVariant());
}

void AbstractTableModelTest::readOnlyDataTest()
{
  ReadOnlyTableModel model;

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 0, 0, Qt::FontRole), QVariant());
  QCOMPARE(getModelData(model, 0, 1, Qt::FontRole), QVariant());
  QVERIFY(model.data(QModelIndex()).isNull());
}

void AbstractTableModelTest::editableDataTest()
{
  EditableTableModel model;

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 0, 0, Qt::EditRole), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1, Qt::EditRole), QVariant("A"));
  QCOMPARE(getModelData(model, 0, 0, Qt::FontRole), QVariant());
  QCOMPARE(getModelData(model, 0, 1, Qt::FontRole), QVariant());
  QVERIFY(model.data(QModelIndex()).isNull());
}

void AbstractTableModelTest::emitDataChangedTest()
{
  QVariantList arguments;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  EditableTableModel model;
  QSignalSpy dataChangedSpy(&model, &ReadOnlyTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());

  populateModel(model, {{1,"A"},{2,"B"},{3,"C"}});
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);

  /*
   * emitDataChanged(QModelIndex)
   */

  QCOMPARE(dataChangedSpy.count(), 0);
  model.emitDataChanged(model.index(0, 0));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 0);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 0);
  QCOMPARE(bottomRight.column(), 0);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);

  QCOMPARE(dataChangedSpy.count(), 0);
  model.emitDataChanged(model.index(2, 1));
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
  QCOMPARE(roles.count(), 0);

  /*
   * emitDataAtRowsChanged(first, last)
   */

  QCOMPARE(dataChangedSpy.count(), 0);
  model.emitDataAtRowsChanged(1, 2);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  topLeft = arguments.at(0).toModelIndex();
  QCOMPARE(topLeft.row(), 1);
  QCOMPARE(topLeft.column(), 0);
  bottomRight = arguments.at(1).toModelIndex();
  QCOMPARE(bottomRight.row(), 2);
  QCOMPARE(bottomRight.column(), 1);
  roles = arguments.at(2).value< QVector<int> >();
  QCOMPARE(roles.count(), 0);
}

void AbstractTableModelTest::emitVerticalHeaderDataChangedTest()
{
  Mdt::IndexRange::RowRange rowRange;
  QVariantList arguments;
  ReadOnlyTableModel model;
  QSignalSpy headerDataChangedSpy(&model, &ReadOnlyTableModel::headerDataChanged);
  QVERIFY(headerDataChangedSpy.isValid());

  QCOMPARE(headerDataChangedSpy.count(), 0);
  QVERIFY(appendRowToModel(model));
  QVERIFY(appendRowToModel(model));

  /*
   * RowRange
   */
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  rowRange.setFirstRow(0);
  rowRange.setLastRow(1);
  model.emitVerticalHeaderDataChanged(rowRange);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last

  /*
   * Single row
   */
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  model.emitVerticalHeaderDataChanged(0);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(0));             // last

  /*
   * firstRow, lastRow
   */
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(headerDataChangedSpy.count(), 0);
  model.emitVerticalHeaderDataChanged(0, 1);
  QCOMPARE(headerDataChangedSpy.count(), 1);
  arguments = headerDataChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 3);
  QCOMPARE(arguments.at(0), QVariant(Qt::Vertical));  // orientation
  QCOMPARE(arguments.at(1), QVariant(0));             // first
  QCOMPARE(arguments.at(2), QVariant(1));             // last
}

void AbstractTableModelTest::readOnlySetDataTest()
{
  ReadOnlyTableModel model;
  QSignalSpy dataChangedSpy(&model, &ReadOnlyTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(dataChangedSpy.count(), 0);
  QVERIFY(!setModelData(model, 0, 0, 15));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(dataChangedSpy.count(), 0);
}

void AbstractTableModelTest::editableSetDataTest()
{
  EditableTableModel model;
  QModelIndex topLeft, bottomRight;
  QVector<int> roles;
  QSignalSpy dataChangedSpy(&model, &ReadOnlyTableModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QVariantList arguments;

  populateModel(model, {{1,"A"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);

  QCOMPARE(dataChangedSpy.count(), 0);
  QVERIFY(setModelData(model, 0, 0, 15));
  QCOMPARE(getModelData(model, 0, 0), QVariant(15));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(dataChangedSpy.count(), 0);
//   arguments = dataChangedSpy.takeFirst();
//   QCOMPARE(arguments.count(), 3);
//   topLeft = arguments.at(0).toModelIndex();
//   QCOMPARE(topLeft.row(), 0);
//   QCOMPARE(topLeft.column(), 0);
//   bottomRight = arguments.at(1).toModelIndex();
//   QCOMPARE(bottomRight.row(), 0);
//   QCOMPARE(bottomRight.column(), 0);
//   roles = arguments.at(2).value< QVector<int> >();
//   QCOMPARE(roles.count(), 0);

  QCOMPARE(dataChangedSpy.count(), 0);
  QVERIFY(setModelData(model, 0, 1, "A15"));
  QCOMPARE(getModelData(model, 0, 0), QVariant(15));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A15"));
  QCOMPARE(dataChangedSpy.count(), 0);
//   arguments = dataChangedSpy.takeFirst();
//   QCOMPARE(arguments.count(), 3);
//   topLeft = arguments.at(0).toModelIndex();
//   QCOMPARE(topLeft.row(), 0);
//   QCOMPARE(topLeft.column(), 1);
//   bottomRight = arguments.at(1).toModelIndex();
//   QCOMPARE(bottomRight.row(), 0);
//   QCOMPARE(bottomRight.column(), 1);
//   roles = arguments.at(2).value< QVector<int> >();
//   QCOMPARE(roles.count(), 0);

  QCOMPARE(dataChangedSpy.count(), 0);
  QVERIFY(!setModelData(model, 0, 0, 0, Qt::FontRole));
  QCOMPARE(getModelData(model, 0, 0), QVariant(15));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A15"));
  QCOMPARE(dataChangedSpy.count(), 0);
}

void AbstractTableModelTest::readOnlyPrependAppendRowsTest()
{
  ReadOnlyTableModel model;

  populateModel(model, {{2,"B"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B"));

  QVERIFY(model.prependRow());
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(getModelData(model, 0, 0).isNull());
  QVERIFY(getModelData(model, 0, 1).isNull());
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));

  QVERIFY(model.appendRow());
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(getModelData(model, 0, 0).isNull());
  QVERIFY(getModelData(model, 0, 1).isNull());
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QVERIFY(getModelData(model, 2, 0).isNull());
  QVERIFY(getModelData(model, 2, 1).isNull());
}

void AbstractTableModelTest::editablePrependAppendRowsTest()
{
  EditableTableModel model;

  populateModel(model, {{2,"B"}});
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B"));

  QVERIFY(model.prependRow());
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(setModelData(model, 0, 0, 1));
  QVERIFY(setModelData(model, 0, 1, "A"));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));

  QVERIFY(model.appendRow());
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(setModelData(model, 2, 0, 3));
  QVERIFY(setModelData(model, 2, 1, "C"));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
}

void AbstractTableModelTest::itemModelInsertRowTest()
{
  ReadOnlyTableModel model1;
  ItemModelInsertRowTest test1(&model1);

  EditableTableModel model2;
  ItemModelInsertRowTest test2(&model2);
}

template<typename Model>
void removeFirstLastRowTestImpl()
{
  Model model;

  populateModel(model, {{1,"A"},{2,"B"},{3,"C"}});
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));

  QVERIFY(removeFirstRowFromModel(model));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(3));
  QCOMPARE(getModelData(model, 1, 1), QVariant("C"));

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(2));
  QCOMPARE(getModelData(model, 0, 1), QVariant("B"));

  QVERIFY(removeLastRowFromModel(model));
  QCOMPARE(model.rowCount(), 0);
}

void AbstractTableModelTest::readOnlyRemoveFirstLastRowTest()
{
  removeFirstLastRowTestImpl<ReadOnlyTableModel>();
}

void AbstractTableModelTest::editableRemoveFirstLastRowTest()
{
  removeFirstLastRowTestImpl<EditableTableModel>();
}

void AbstractTableModelTest::itemModelRemoveRowTest()
{
  /*
   * This test requires that setData() is implemented
   */
  EditableTableModel model;
  ItemModelRemoveRowTest test(&model);
}

void AbstractTableModelTest::readOnlyQtModelTest()
{
  ReadOnlyTableModel model;

  QtModelTest mt0(&model);

  populateModel(model, {{1,"A"},{2,"B"},{3,"C"}});
  QtModelTest mt3(&model);
}

void AbstractTableModelTest::editableQtModelTest()
{
  EditableTableModel model;

  QtModelTest mt0(&model);

  populateModel(model, {{1,"A"},{2,"B"},{3,"C"}});
  QtModelTest mt3(&model);
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AbstractTableModelTest test;

  return QTest::qExec(&test, argc, argv);
}
