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
#include "StlListModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/ReadOnlyStlListModel.h"
#include "Mdt/ItemModel/EditableStlListModel.h"
#include "Mdt/ItemModel/ResizableStlListModel.h"
#include <QVariantList>
#include <QList>
#include <QVector>
#include <QModelIndex>
#include <QSignalSpy>
#include <vector>

using namespace Mdt::ItemModel;

void StlListTest::initTestCase()
{
}

void StlListTest::cleanupTestCase()
{
}

/*
 * Helpers
 */

// Class to get efault flags
class FlagsTestListModel : public QAbstractListModel
{
 public:

  explicit FlagsTestListModel(QObject* parent = nullptr)
   : QAbstractListModel(parent) {}

  int rowCount(const QModelIndex &) const override
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

Qt::ItemFlags getListModelStandardFlags()
{
  FlagsTestListModel model;
  QModelIndex index = model.index(0, 0);
  Q_ASSERT(index.isValid());
  return model.flags(index);
}


/*
 * Tests
 */

void StlListTest::stdVectorRoTest()
{
  const Qt::ItemFlags expectedFlags = getListModelStandardFlags();
  ReadOnlyStlListModel< std::vector<int> > model;
  QCOMPARE(model.rowCount(), 0);

  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.rowCount(QModelIndex()), 3);
  // Check flags
  QCOMPARE(getModelFlags(model, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 1, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 2, 0), expectedFlags);
  // Check accessing at row
  QCOMPARE(model.at(0), 1);
  QCOMPARE(model.at(1), 2);
  QCOMPARE(model.at(2), 3);
  // Check get data for default role
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  // Check get data for DisplayRole
  QCOMPARE(getModelData(model, 0, 0, Qt::DisplayRole), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0, Qt::DisplayRole), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0, Qt::DisplayRole), QVariant(3));
  // Check get data for EditRole
//   QCOMPARE(getModelData(model, 0, 0, Qt::EditRole), QVariant(1));
//   QCOMPARE(getModelData(model, 1, 0, Qt::EditRole), QVariant(2));
//   QCOMPARE(getModelData(model, 2, 0, Qt::EditRole), QVariant(3));
  // Check getting data for ToolTipRole
  QVERIFY(getModelData(model, 0, 0, Qt::ToolTipRole).isNull());
  QVERIFY(getModelData(model, 1, 0, Qt::ToolTipRole).isNull());
  QVERIFY(getModelData(model, 2, 0, Qt::ToolTipRole).isNull());
}

void StlListTest::variantListRoTest()
{
  ReadOnlyStlListModel<QVariantList> model;
  QVariantList l{1, "A"};
  model.setContainer(l);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant("A"));
}

void StlListTest::roSignalTest()
{
  using model_type = ReadOnlyStlListModel<QVariantList>;
  model_type model;
  QSignalSpy resetSpy(&model, &model_type::modelReset);
  QVERIFY(resetSpy.isValid());
  QCOMPARE(resetSpy.count(), 0);

  QVariantList l{2, "B"};
  model.setContainer(l);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(resetSpy.count(), 1);
}

void StlListTest::roQtModelTest()
{
  ReadOnlyStlListModel< std::vector<int> > model;
  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  QtModelTest mt(&model);
}

void StlListTest::stdVectorRwTest()
{
  const Qt::ItemFlags expectedFlags = (getListModelStandardFlags() | Qt::ItemIsEditable);
  EditableStlListModel< std::vector<int> > model;
  QCOMPARE(model.rowCount(), 0);

  std::vector<int> v{0,0,0};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  // Check flags
  QCOMPARE(getModelFlags(model, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 1, 0), expectedFlags);
  QCOMPARE(getModelFlags(model, 2, 0), expectedFlags);
  // Check setValue and at
  QCOMPARE(model.at(0), 0);
  QCOMPARE(model.at(1), 0);
  QCOMPARE(model.at(2), 0);
  model.setValue(0, 1);
  model.setValue(1, 2);
  model.setValue(2, 3);
  QCOMPARE(model.at(0), 1);
  QCOMPARE(model.at(1), 2);
  QCOMPARE(model.at(2), 3);
  // Check get data for default role
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  // Check setting data
  QVERIFY(setModelData(model, 0, 0, 11));
  QVERIFY(setModelData(model, 1, 0, 12));
  QVERIFY(setModelData(model, 2, 0, 13));
  QCOMPARE(getModelData(model, 0, 0), QVariant(11));
  QCOMPARE(getModelData(model, 1, 0), QVariant(12));
  QCOMPARE(getModelData(model, 2, 0), QVariant(13));
}

void StlListTest::variantListRwTest()
{
  EditableStlListModel<QVariantList> model;
  QVariantList l{1,"A"};

  model.setContainer(l);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant("A"));
  QVERIFY(setModelData(model, 0, 0, "B"));
  QVERIFY(setModelData(model, 1, 0, 2));
  QCOMPARE(getModelData(model, 0, 0), QVariant("B"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
}

void StlListTest::setDataSignalTest()
{
  using model_type = EditableStlListModel<QVariantList>;
  model_type model;
  QSignalSpy dataChangedSpy(&model, &model_type::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  QVariantList arguments;
  QModelIndex index;

  QVariantList l{3, "C"};
  model.setContainer(l);
  QCOMPARE(dataChangedSpy.count(), 0);

  model.setValue(0, "A");
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  index = arguments.at(0).toModelIndex();
  QVERIFY(index.isValid());
  QCOMPARE(index.row(), 0);
  QCOMPARE(index.column(), 0);
  QCOMPARE( arguments.at(1).toModelIndex(), index );
  dataChangedSpy.clear();

  QVERIFY(setModelData(model, 1, 0, "B"));
  QCOMPARE(dataChangedSpy.count(), 1);
  arguments = dataChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  index = arguments.at(0).toModelIndex();
  QVERIFY(index.isValid());
  QCOMPARE(index.row(), 1);
  QCOMPARE(index.column(), 0);
  QCOMPARE( arguments.at(1).toModelIndex(), index );
  dataChangedSpy.clear();
}

void StlListTest::rwQtModelTest()
{
  EditableStlListModel< std::vector<int> > model;
  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  QtModelTest mt(&model);
}

void StlListTest::insertRowsQListTest()
{
  ResizableStlListModel<QVariantList> model;
  QVariantList l;

  model.setContainer(l);
  QCOMPARE(model.rowCount(), 0);
  /*
   * Append a row
   * ---
   * |A|
   * ---
   */
  model.appendRow();
  QCOMPARE(model.rowCount(), 1);
  // Check that inserted row can be used
  QVERIFY(setModelData(model, 0, 0, "A"));
  QCOMPARE(getModelData(model, 0, 0), QVariant("A"));
  /*
   * Prepend a row
   * ---
   * |1|
   * ---
   * |A|
   * ---
   */
  model.prependRow();
  QCOMPARE(model.rowCount(), 2);
  // Check that row was inserted at correct place
  QVERIFY(getModelData(model, 0, 0).isNull());
  QCOMPARE(getModelData(model, 1, 0), QVariant("A"));
  // Check that inserted row can be used
  QVERIFY(setModelData(model, 0, 0, 1));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant("A"));
  /*
   * Insert 2 rows before A
   * Before:
   *  ---
   *  |1|
   * >---
   *  |A|
   *  ---
   * After:
   * ---
   * |1|
   * ---
   * |2|
   * ---
   * |3|
   * ---
   * |A|
   * ---
   */
  QVERIFY(model.insertRows(1, 2));
  QCOMPARE(model.rowCount(), 4);
  // Check that rows where inserted at correctplace
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QVERIFY(getModelData(model, 1, 0).isNull());
  QVERIFY(getModelData(model, 2, 0).isNull());
  QCOMPARE(getModelData(model, 3, 0), QVariant("A"));
  // Check that inserted row can be used
  QVERIFY(setModelData(model, 1, 0, 2));
  QVERIFY(setModelData(model, 2, 0, 3));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0), QVariant(3));
  QCOMPARE(getModelData(model, 3, 0), QVariant("A"));
}

void StlListTest::insertRowsStdVectorTest()
{
  ResizableStlListModel< std::vector<int> > model;
  std::vector<int> v;

  model.setContainer(v);
  QCOMPARE(model.rowCount(), 0);
  model.appendRow();
  QCOMPARE(model.rowCount(), 1);
  model.prependRow();
  QCOMPARE(model.rowCount(), 2);
}

void StlListTest::insertRowsQVectorTest()
{
  ResizableStlListModel< QVector<int> > model;
  QVector<int> v;

  model.setContainer(v);
  QCOMPARE(model.rowCount(), 0);
  model.appendRow();
  QCOMPARE(model.rowCount(), 1);
  model.prependRow();
  QCOMPARE(model.rowCount(), 2);
}

void StlListTest::insertRowsSignalTest()
{
  using model_type = ResizableStlListModel< std::vector<int> >;
  model_type model;
  QSignalSpy insertRowsSpy(&model, &model_type::rowsInserted);
  QVERIFY(insertRowsSpy.isValid());
  QCOMPARE(insertRowsSpy.count(), 0);
  QVariantList arguments;

  std::vector<int> v;
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(insertRowsSpy.count(), 0); // Set container causes a model reset

  model.appendRow();
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(insertRowsSpy.count(), 1);
  arguments = insertRowsSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
}

void StlListTest::removeRowsQListTest()
{
  ResizableStlListModel<QVariantList> model;
  QCOMPARE(model.rowCount(), 0);

  /*
   * Setup model
   * ---
   * |A|
   * ---
   * |B|
   * ---
   * |C|
   * ---
   * |D|
   * ---
   * |E|
   * ---
   */
  QVariantList l{"A","B","C","D","E"};
  model.setContainer(l);
  QCOMPARE(model.rowCount(), 5);
  /*
   * Remove last row
   * ---
   * |A|
   * ---
   * |B|
   * ---
   * |C|
   * ---
   * |D|
   * ---
   */
  model.removeLastRow();
  QCOMPARE(model.rowCount(), 4);
  // Check that correct row was removed
  QCOMPARE(getModelData(model, 0, 0), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 0), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 0), QVariant("C"));
  QCOMPARE(getModelData(model, 3, 0), QVariant("D"));
  /*
   * Remove first row
   * ---
   * |B|
   * ---
   * |C|
   * ---
   * |D|
   * ---
   */
  model.removeFirstRow();
  QCOMPARE(model.rowCount(), 3);
  // Check that correct row was removed
  QCOMPARE(getModelData(model, 0, 0), QVariant("B"));
  QCOMPARE(getModelData(model, 1, 0), QVariant("C"));
  QCOMPARE(getModelData(model, 2, 0), QVariant("D"));
  /*
   * Remove C and D
   * ---
   * |B|
   * ---
   */
  QVERIFY(model.removeRows(1, 2));
  QCOMPARE(model.rowCount(), 1);
  // Check that correct rows were removed
  QCOMPARE(getModelData(model, 0, 0), QVariant("B"));
  /*
   * Remove last row
   */
  model.removeFirstRow();
  QCOMPARE(model.rowCount(), 0);
  /*
   * Try to call remove first/last on empty model
   */
  model.removeFirstRow();
  QCOMPARE(model.rowCount(), 0);
  model.removeLastRow();
  QCOMPARE(model.rowCount(), 0);
}

void StlListTest::removeRowsStdVectorTest()
{
  ResizableStlListModel< std::vector<int> > model;
  std::vector<int> v{1,2};

  model.setContainer(v);
  QCOMPARE(model.rowCount(), 2);
  model.removeFirstRow();
  QCOMPARE(model.rowCount(), 1);
  model.removeLastRow();
  QCOMPARE(model.rowCount(), 0);
}

void StlListTest::removeRowsQVectorTest()
{
  ResizableStlListModel< QVector<int> > model;
  QVector<int> v{1,2};

  model.setContainer(v);
  QCOMPARE(model.rowCount(), 2);
  model.removeFirstRow();
  QCOMPARE(model.rowCount(), 1);
  model.removeLastRow();
  QCOMPARE(model.rowCount(), 0);
}

void StlListTest::removeRowsSignalTest()
{
  using model_type = ResizableStlListModel< std::vector<int> >;
  model_type model;
  QSignalSpy rowsRemovedSpy(&model, &model_type::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QVariantList arguments;

  std::vector<int> v{1, 2};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowsRemovedSpy.count(), 0); // Set container causes a model reset

  model.removeLastRow();
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last
}

void StlListTest::resizableQtModelTest()
{
  ResizableStlListModel< std::vector<int> > model;
  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  QtModelTest mt(&model);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  StlListTest test;

  return QTest::qExec(&test, argc, argv);
}
