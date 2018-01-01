/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include <vector>
#include <QVariantList>
#include <QModelIndex>

using namespace Mdt::ItemModel;

void StlListTest::initTestCase()
{
}

void StlListTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void StlListTest::stdVectorRoTest()
{
  ReadOnlyStlListModel< std::vector<int> > model;
  QCOMPARE(model.rowCount(), 0);

  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.rowCount(QModelIndex()), 3);
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
  QCOMPARE(getModelData(model, 0, 0, Qt::EditRole), QVariant(1));
  QCOMPARE(getModelData(model, 1, 0, Qt::EditRole), QVariant(2));
  QCOMPARE(getModelData(model, 2, 0, Qt::EditRole), QVariant(3));
  // Check getting data for ToolTipRole
  QVERIFY(getModelData(model, 0, 0, Qt::ToolTipRole).isNull());
  QVERIFY(getModelData(model, 1, 0, Qt::ToolTipRole).isNull());
  QVERIFY(getModelData(model, 2, 0, Qt::ToolTipRole).isNull());
}

void StlListTest::variantListRoTest()
{
  QFAIL("Not complete");
}

void StlListTest::roSignalTest()
{
  QFAIL("Not complete");
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
  EditableStlListModel< std::vector<int> > model;
  QCOMPARE(model.rowCount(), 0);

  std::vector<int> v{1,2,3};
  model.setContainer(v);
  QCOMPARE(model.rowCount(), 3);
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
  // Setting data on invalid index should fail
  QVERIFY(!setModelData(model, 0, 1, 44));
  QCOMPARE(getModelData(model, 0, 0), QVariant(11));
  QCOMPARE(getModelData(model, 1, 0), QVariant(12));
  QCOMPARE(getModelData(model, 2, 0), QVariant(13));

  QFAIL("Not complete");
}

void StlListTest::variantListRwTest()
{
  QFAIL("Not complete");
}

void StlListTest::setDataSignalTest()
{
  QFAIL("Not complete");
}

void StlListTest::rwQtModelTest()
{
  EditableStlListModel< std::vector<int> > model;
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
