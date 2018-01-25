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
#include "Mdt/TestLib/ItemModelInsertRowTest.h"
#include "Mdt/TestLib/ItemModelRemoveRowTest.h"
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
using Mdt::TestLib::ItemModelInsertRowTest;
using Mdt::TestLib::ItemModelRemoveRowTest;

void StlListModelTest::initTestCase()
{
}

void StlListModelTest::cleanupTestCase()
{
}

/*
 * Helpers
 */

// Class to get default flags
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

void StlListModelTest::stdVectorRoTest()
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

void StlListModelTest::variantListRoTest()
{
  ReadOnlyStlListModel<QVariantList> model;
  QVariantList l{1, "A"};
  model.setContainer(l);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant("A"));
}

void StlListModelTest::roSignalTest()
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

void StlListModelTest::roQtModelTest()
{
  ReadOnlyStlListModel< std::vector<int> > model;
  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  QtModelTest mt(&model);
}

void StlListModelTest::stdVectorRwTest()
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

void StlListModelTest::variantListRwTest()
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

void StlListModelTest::setDataSignalTest()
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

void StlListModelTest::rwQtModelTest()
{
  EditableStlListModel< std::vector<int> > model;
  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  QtModelTest mt(&model);
}

void StlListModelTest::insertRowsTest()
{
  ResizableStlListModel<QVariantList> model;
  QVariantList l;
  model.setContainer(l);
  ItemModelInsertRowTest mt(&model);
}

void StlListModelTest::prependAppendRowTest()
{
  using model_type = ResizableStlListModel< std::vector<int> >;
  model_type model;
  QSignalSpy insertRowsSpy(&model, &model_type::rowsInserted);
  QVERIFY(insertRowsSpy.isValid());
  QCOMPARE(insertRowsSpy.count(), 0);
  QVariantList arguments;

  /*
   * Setup model
   * ---
   * |1|
   * ---
   */
  std::vector<int> v{1};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(insertRowsSpy.count(), 0); // Set container causes a model reset
  /*
   * Append a row
   * ---
   * |1|
   * ---
   * |2|
   * ---
   */
  model.appendRow();
  // Check data
  QCOMPARE(model.rowCount(), 2);
  QVERIFY(setModelData(model, 1, 0, 2));
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  // Check signal
  QCOMPARE(insertRowsSpy.count(), 1);
  arguments = insertRowsSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last
  /*
   * Prepend a row
   * ----
   * |-1|
   * ----
   * | 1|
   * ----
   * | 2|
   * ----
   */
  model.prependRow();
  // Check data
  QCOMPARE(model.rowCount(), 3);
  QVERIFY(setModelData(model, 0, 0, -1));
  QCOMPARE(getModelData(model, 0, 0), QVariant(-1));
  QCOMPARE(getModelData(model, 1, 0), QVariant(1));
  QCOMPARE(getModelData(model, 2, 0), QVariant(2));
  // Check signal
  QCOMPARE(insertRowsSpy.count(), 1);
  arguments = insertRowsSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
}

void StlListModelTest::removeRowsTest()
{
  ResizableStlListModel<QVariantList> model;
  QCOMPARE(model.rowCount(), 0);
  ItemModelRemoveRowTest mt(&model);
}

void StlListModelTest::removeFirstLastRowTest()
{
  using model_type = ResizableStlListModel< std::vector<int> >;
  model_type model;
  QSignalSpy rowsRemovedSpy(&model, &model_type::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QVariantList arguments;

  /*
   * Setup model
   * ---
   * |1|
   * ---
   * |2|
   * ---
   */
  std::vector<int> v{1,2};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(rowsRemovedSpy.count(), 0); // Set container causes a model reset
  /*
   * Remove last row
   * ---
   * |1|
   * ---
   */
  model.removeLastRow();
  // Check data
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  // Check signal
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(1)); // first
  QCOMPARE(arguments.at(2), QVariant(1)); // last
  /*
   * Remove first row
   */
  model.removeFirstRow();
  // Check data
  QCOMPARE(model.rowCount(), 0);
  // Check signal
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.size(), 3);
  QVERIFY(!arguments.at(0).toModelIndex().isValid()); // parent
  QCOMPARE(arguments.at(1), QVariant(0)); // first
  QCOMPARE(arguments.at(2), QVariant(0)); // last
  /*
   * Try to call remove first/last on empty model
   */
  QCOMPARE(model.rowCount(), 0);
  model.removeFirstRow();
  QCOMPARE(model.rowCount(), 0);
  model.removeLastRow();
  QCOMPARE(model.rowCount(), 0);
}

void StlListModelTest::resizableQtModelTest()
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
    StlListModelTest test;

  return QTest::qExec(&test, argc, argv);
}
