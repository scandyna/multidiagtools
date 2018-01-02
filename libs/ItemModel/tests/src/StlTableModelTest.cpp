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
#include "StlTableModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/ReadOnlyStlTableModel.h"
#include "Mdt/ItemModel/EditableStlTableModel.h"
#include "Mdt/ItemModel/RowResizableStlTableModel.h"
#include <QAbstractTableModel>
#include <QVariantList>
#include <QList>
#include <QVector>
#include <QModelIndex>
#include <QSignalSpy>
#include <vector>

using namespace Mdt::ItemModel;

void StlTableModelTest::initTestCase()
{
}

void StlTableModelTest::cleanupTestCase()
{
}

/*
 * Helpers
 */

// Class to get default flags
class FlagsTestTableModel : public QAbstractTableModel
{
 public:

  explicit FlagsTestTableModel(QObject* parent = nullptr)
   : QAbstractTableModel(parent) {}

  int rowCount(const QModelIndex &) const override
  {
    return 1;
  }

  int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
  {
    return 1;
  }

  QVariant data(const QModelIndex &, int) const override
  {
    return QVariant();
  }
};

Qt::ItemFlags getTableModelStandardFlags()
{
  FlagsTestTableModel model;
  QModelIndex index = model.index(0, 0);
  Q_ASSERT(index.isValid());
  return model.flags(index);
}

/*
 * Tests
 */

void StlTableModelTest::readOnlyStdVectorTest()
{
  using Container = std::vector< std::vector<int> >;
  ReadOnlyStlTableModel<Container> model;
  QVERIFY(model.isEmpty());
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.rowCount(QModelIndex()), 0);
  QCOMPARE(model.columnCount(), 0);
  QCOMPARE(model.columnCount(QModelIndex()), 0);

  Container table{{10,11},{20,21}, {30,31}};
  model.setContainer(table);
  QVERIFY(!model.isEmpty());
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.rowCount(QModelIndex()), 3);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(model.columnCount(QModelIndex()), 2);

  QCOMPARE(model.record(0).at(0), 10);
  QCOMPARE(model.record(0).at(1), 11);
  QCOMPARE(model.record(1).at(0), 20);
  QCOMPARE(model.record(1).at(1), 21);
  QCOMPARE(model.record(2).at(0), 30);
  QCOMPARE(model.record(2).at(1), 31);

  QCOMPARE(model.at(0, 0), 10);
  QCOMPARE(model.at(0, 1), 11);
  QCOMPARE(model.at(1, 0), 20);
  QCOMPARE(model.at(1, 1), 21);
  QCOMPARE(model.at(2, 0), 30);
  QCOMPARE(model.at(2, 1), 31);

  // Check get data for default role
  QCOMPARE(getModelData(model, 0, 0), QVariant(10));
  QCOMPARE(getModelData(model, 0, 1), QVariant(11));
  QCOMPARE(getModelData(model, 1, 0), QVariant(20));
  QCOMPARE(getModelData(model, 1, 1), QVariant(21));
  QCOMPARE(getModelData(model, 2, 0), QVariant(30));
  QCOMPARE(getModelData(model, 2, 1), QVariant(31));
  // Check get data for DisplayRole
  QCOMPARE(getModelData(model, 0, 0, Qt::DisplayRole), QVariant(10));
  QCOMPARE(getModelData(model, 0, 1, Qt::DisplayRole), QVariant(11));
  QCOMPARE(getModelData(model, 1, 0, Qt::DisplayRole), QVariant(20));
  QCOMPARE(getModelData(model, 2, 0, Qt::DisplayRole), QVariant(30));
  // Check get data for EditRole
//   QCOMPARE(getModelData(model, 0, 0, Qt::EditRole), QVariant(1));
//   QCOMPARE(getModelData(model, 1, 0, Qt::EditRole), QVariant(2));
//   QCOMPARE(getModelData(model, 2, 0, Qt::EditRole), QVariant(3));
  // Check getting data for ToolTipRole
  QVERIFY(getModelData(model, 0, 0, Qt::ToolTipRole).isNull());
  QVERIFY(getModelData(model, 0, 1, Qt::ToolTipRole).isNull());
  QVERIFY(getModelData(model, 1, 0, Qt::ToolTipRole).isNull());
  QVERIFY(getModelData(model, 2, 0, Qt::ToolTipRole).isNull());
}

void StlTableModelTest::readOnlySignalTest()
{
  using Table = std::vector< QVariantList >;
  ReadOnlyStlTableModel<Table> model;
  QCOMPARE(model.rowCount(), 0);
  QSignalSpy resetSpy(&model, &ReadOnlyStlTableModel<Table>::modelReset);
  QVERIFY(resetSpy.isValid());
  QCOMPARE(resetSpy.count(), 0);

  Table t{{1,"A"},{2, "B"},{3,"C"}};
  model.setContainer(t);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(resetSpy.count(), 1);
}

void StlTableModelTest::readOnlyQtModelTest()
{
  using Table = std::vector< QVariantList >;
  ReadOnlyStlTableModel<Table> model;
  Table t{{1,"A"},{2, "B"},{3,"C"}};
  model.setContainer(t);
  QCOMPARE(model.rowCount(), 3);
  QtModelTest mt(&model);
}

void StlTableModelTest::editableStdVectorTest()
{
  const Qt::ItemFlags expectedFlags = (getTableModelStandardFlags() | Qt::ItemIsEditable);
  using Table = std::vector< QVariantList >;
  EditableStlTableModel<Table> model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);

  Table t{{0,0},{0, 0},{0,0}};
  model.setContainer(t);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  // Check flags
  QCOMPARE(getModelFlags(model, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 0, 1), expectedFlags);
  QCOMPARE(getModelFlags(model, 1, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 1, 1), expectedFlags);
  QCOMPARE(getModelFlags(model, 2, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 2, 1), expectedFlags);
  // Check get data for default role
  QCOMPARE(getModelData(model, 0, 0), QVariant(0));
  QCOMPARE(getModelData(model, 0, 1), QVariant(0));
  QCOMPARE(getModelData(model, 1, 0), QVariant(0));
  QCOMPARE(getModelData(model, 1, 1), QVariant(0));
  QCOMPARE(getModelData(model, 2, 0), QVariant(0));
  QCOMPARE(getModelData(model, 2, 1), QVariant(0));
  // Check setting data
  QVERIFY(setModelData(model, 0, 0, 1));
  QVERIFY(setModelData(model, 0, 1, "A"));
  QVERIFY(setModelData(model, 1, 0, 2));
  QVERIFY(setModelData(model, 1, 1, "B"));
  QVERIFY(setModelData(model, 2, 0, 3));
  QVERIFY(setModelData(model, 2, 1, "C"));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
}

void StlTableModelTest::setDataSignalTest()
{
  using Table = std::vector< QVariantList >;
  EditableStlTableModel<Table> model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
  QSignalSpy dataChangedSpy(&model, &EditableStlTableModel<Table>::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QCOMPARE(dataChangedSpy.count(), 0);
  QVariantList arguments;
  QModelIndex index;

  Table t{{0,0},{0, 0},{0,0}};
  model.setContainer(t);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(dataChangedSpy.count(), 0);

  model.setValue(0, 0, 1);
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QVERIFY(index.isValid());
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 0);
  QCOMPARE( arguments.at(1).toModelIndex(), index );  // bottomRight
  dataChangedSpy.clear();

  QVERIFY(setModelData(model, 0, 1, "A"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  index = arguments.at(0).toModelIndex(); // topLeft
  QVERIFY(index.isValid());
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 1);
  QCOMPARE( arguments.at(1).toModelIndex(), index );  // bottomRight
  dataChangedSpy.clear();
}

void StlTableModelTest::editableQtModelTest()
{
  using Table = std::vector< QVariantList >;
  EditableStlTableModel<Table> model;
  Table t{{1,"A"},{2, "B"},{3,"C"}};
  model.setContainer(t);
  QtModelTest mt(&model);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
    StlTableModelTest test;

  return QTest::qExec(&test, argc, argv);
}
