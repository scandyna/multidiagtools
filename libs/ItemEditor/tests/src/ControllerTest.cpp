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
#include "ControllerTest.h"
#include "ItemModelControllerTester.h"
#include "PrimaryKeyChangedSignalSpy.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/PrimaryKeyProxyModel.h"
#include "Mdt/ItemModel/ForeignKeyProxyModel.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include <QSignalSpy>
#include <QStringList>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

void ControllerTest::initTestCase()
{
}

void ControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ControllerTest::statePermissionTest()
{
  ItemModelControllerTester controller;
  auto permission = controller.controllerStatePermission();

  // Current row change
  QVERIFY(permission.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!permission.canChangeCurrentRow(ControllerState::Editing));
  QVERIFY(!permission.canChangeCurrentRow(ControllerState::Inserting));
  // Insert
  QVERIFY(permission.canInsert(ControllerState::Visualizing));
  QVERIFY(!permission.canInsert(ControllerState::Editing));
  QVERIFY(!permission.canInsert(ControllerState::Inserting));
  // Submit
  QVERIFY(!permission.canSubmit(ControllerState::Visualizing));
  QVERIFY(permission.canSubmit(ControllerState::Editing));
  QVERIFY(!permission.canSubmit(ControllerState::Inserting));
  // Revert
  QVERIFY(!permission.canRevert(ControllerState::Visualizing));
  QVERIFY(permission.canRevert(ControllerState::Editing));
  QVERIFY(!permission.canRevert(ControllerState::Inserting));
  // Remove
  QVERIFY(permission.canRemove(ControllerState::Visualizing));
  QVERIFY(!permission.canRemove(ControllerState::Editing));
  QVERIFY(permission.canRemove(ControllerState::Inserting));
  // Select
  QVERIFY(permission.canSelect(ControllerState::Visualizing));
  QVERIFY(!permission.canSelect(ControllerState::Editing));
  QVERIFY(!permission.canSelect(ControllerState::Inserting));
}

void ControllerTest::basicStateTest()
{
  /*
   * Initial state
   */
  ItemModelControllerTester controller;
  QVERIFY(controller.controllerState() == ControllerState::Visualizing);
  ///QVERIFY(controller.primaryKey().isNull());
}

void ControllerTest::setModelTest()
{
  /*
   * Initial state
   */
  ItemModelControllerTester controller;
  QVERIFY(controller.model() == nullptr);
  QVERIFY(controller.sourceModel() == nullptr);
  QVERIFY(controller.modelForView() == nullptr);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Set a empty model
   */
  VariantTableModel tableModel;
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  QVERIFY(controller.sourceModel() == &tableModel);
  QVERIFY(controller.modelForView() == &tableModel);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Set a populated model
   */
  QStringListModel listModel;
  listModel.setStringList( QStringList({"A","B","C"}) );
  controller.setModel(&listModel);
  QVERIFY(controller.model() == &listModel);
  QVERIFY(controller.sourceModel() == &listModel);
  QVERIFY(controller.modelForView() == &listModel);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Set a empty model
   */
  QCOMPARE(tableModel.rowCount(), 0);
  controller.setModel(&tableModel);
  QVERIFY(controller.model() == &tableModel);
  QVERIFY(controller.sourceModel() == &tableModel);
  QVERIFY(controller.modelForView() == &tableModel);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
}

void ControllerTest::setModelSignalTest()
{
  ItemModelControllerTester controller;
  QVariantList arguments;
  RowState rs;
  QSignalSpy sourceModelChangedSpy(&controller, &ItemModelControllerTester::sourceModelChanged);
  QSignalSpy modelForViewChangedSpy(&controller, &ItemModelControllerTester::modelForViewChanged);
  QSignalSpy rowStateChangedSpy(&controller, &ItemModelControllerTester::rowStateChanged);
  QSignalSpy currentRowChangedSpy(&controller, &ItemModelControllerTester::currentRowChanged);
  QVERIFY(sourceModelChangedSpy.isValid());
  QVERIFY(modelForViewChangedSpy.isValid());
  QVERIFY(rowStateChangedSpy.isValid());
  QVERIFY(currentRowChangedSpy.isValid());
  /*
   * Initial state
   */
  QCOMPARE(sourceModelChangedSpy.count(), 0);
  QCOMPARE(modelForViewChangedSpy.count(), 0);
  QCOMPARE(rowStateChangedSpy.count(), 0);
  QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Set a empty model
   */
  VariantTableModel tableModel;
  controller.setModel(&tableModel);
  // Check sourceModelChanged
  QCOMPARE(sourceModelChangedSpy.count(), 1);
  arguments = sourceModelChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &tableModel);
  // Check modelForViewChanged
  QCOMPARE(modelForViewChangedSpy.count(), 1);
  arguments = modelForViewChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &tableModel);
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 0);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Set a populated model
   */
  QStringListModel listModel;
  listModel.setStringList( QStringList({"A","B","C"}) );
  controller.setModel(&listModel);
  // Check sourceModelChanged
  QCOMPARE(sourceModelChangedSpy.count(), 1);
  arguments = sourceModelChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &listModel);
  // Check modelForViewChanged
  QCOMPARE(modelForViewChangedSpy.count(), 1);
  arguments = modelForViewChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &listModel);
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 1);
  arguments = rowStateChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  rs = arguments.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 1);
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).toInt(), 0);
  /*
   * Set same model again
   */
  controller.setModel(&listModel);
  QCOMPARE(sourceModelChangedSpy.count(), 0);
  QCOMPARE(modelForViewChangedSpy.count(), 0);
  QCOMPARE(rowStateChangedSpy.count(), 0);
  QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Set a empty model
   */
  QCOMPARE(tableModel.rowCount(), 0);
  controller.setModel(&tableModel);
  // Check sourceModelChanged
  QCOMPARE(sourceModelChangedSpy.count(), 1);
  arguments = sourceModelChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &tableModel);
  // Check modelForViewChanged
  QCOMPARE(modelForViewChangedSpy.count(), 1);
  arguments = modelForViewChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &tableModel);
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 1);
  arguments = rowStateChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  rs = arguments.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 1);
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).toInt(), -1);
}

void ControllerTest::modelSizeChangedTest()
{
  ItemModelControllerTester controller;
  /*
   * Set a empty model
   */
  VariantTableModel tableModel;
  controller.setModel(&tableModel);
  QCOMPARE(controller.rowCount(), 0);
  QCOMPARE(controller.currentRow(), -1);
  /*
   * Resize the model
   * (reset signal from model)
   */
  tableModel.resize(3, 2);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  /*
   * Insert rows
   */
  // At beginning
  tableModel.prependRow();
  QCOMPARE(controller.rowCount(), 4);
  QCOMPARE(controller.currentRow(), 1);
  // At end
  tableModel.appendRow();
  QCOMPARE(controller.rowCount(), 5);
  QCOMPARE(controller.currentRow(), 1);
  /*
   * Remove rows
   */
  // At beginning
  tableModel.removeFirstRow();
  QCOMPARE(controller.rowCount(), 4);
  QCOMPARE(controller.currentRow(), 0);
  // At end
  tableModel.removeLastRow();
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
}

void ControllerTest::modelSizeChangedSignalsTest()
{
  ItemModelControllerTester controller;
  QVariantList arguments;
  RowState rs;
  QSignalSpy rowStateChangedSpy(&controller, &ItemModelControllerTester::rowStateChanged);
  QSignalSpy currentRowChangedSpy(&controller, &ItemModelControllerTester::currentRowChanged);
  QVERIFY(rowStateChangedSpy.isValid());
  QVERIFY(currentRowChangedSpy.isValid());

  /*
   * Set a empty model
   */
  VariantTableModel tableModel;
  controller.setModel(&tableModel);
  QCOMPARE(rowStateChangedSpy.count(), 0);
  QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Resize the model
   * (reset signal from model)
   */
  tableModel.resize(3, 2);
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 1);
  arguments = rowStateChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  rs = arguments.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 1);
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).toInt(), 0);
  /*
   * Insert rows
   */
  // At beginning
  tableModel.prependRow();
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 1);
  arguments = rowStateChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  rs = arguments.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), 1);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 1);
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).toInt(), 1);
  // At end
  tableModel.appendRow();
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 1);
  arguments = rowStateChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  rs = arguments.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 5);
  QCOMPARE(rs.currentRow(), 1);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 0);
  /*
   * Remove rows
   */
  // At beginning
  tableModel.removeFirstRow();
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 1);
  arguments = rowStateChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  rs = arguments.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), 0);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 1);
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).toInt(), 0);
  // At end
  tableModel.removeLastRow();
  // Check rowStateChanged
  QCOMPARE(rowStateChangedSpy.count(), 1);
  arguments = rowStateChangedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  rs = arguments.at(0).value<RowState>();
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 1);
  // Check currentRowChanged
  QCOMPARE(currentRowChangedSpy.count(), 0);
}

void ControllerTest::setModelRowCountTest()
{
  /*
   * Initial state
   */
  ItemModelControllerTester controller;
  QCOMPARE(controller.rowCount(), 0);
  /*
   * Set model
   */
  VariantTableModel model;
  model.resize(2, 1);
  controller.setModel(&model);
  QCOMPARE(controller.rowCount(), 2);

  QFAIL("Not complete");
}

void ControllerTest::setModelRowCountSignalTest()
{
  QFAIL("Not complete");
}

void ControllerTest::addRemoveProxyModelWithoutSourceModelTest()
{
  /*
   * Initial state
   */
  ItemModelControllerTester controller;
  QVERIFY(controller.sourceModel() == nullptr);
  QVERIFY(controller.modelForView() == nullptr);
  /*
   * Prepend a proxy model
   */
  QSortFilterProxyModel proxyModel1;
  controller.prependProxyModel(&proxyModel1);
  QCOMPARE(controller.modelForView(), &proxyModel1);
  /*
   * Append a proxy model
   */
  QSortFilterProxyModel proxyModel2;
  controller.appendProxyModel(&proxyModel2);
  QCOMPARE(controller.modelForView(), &proxyModel2);
  /*
   * Remove proxy models
   */
  controller.removeProxyModel(&proxyModel1);
  QCOMPARE(controller.modelForView(), &proxyModel2);
  controller.removeProxyModel(&proxyModel2);
  QVERIFY(controller.modelForView() == nullptr);
}

void ControllerTest::addRemoveProxyModelTest()
{
  ItemModelControllerTester controller;
  /*
   * Set a model
   */
  QStringListModel model1;
  controller.setModel(&model1);
  QCOMPARE(controller.model(), &model1);
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.modelForView(), &model1);
  /*
   * Prepend a proxy model
   */
  QSortFilterProxyModel proxyModel1;
  controller.prependProxyModel(&proxyModel1);
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.modelForView(), &proxyModel1);
  /*
   * Append a proxy model
   */
  QSortFilterProxyModel proxyModel2;
  controller.appendProxyModel(&proxyModel2);
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.modelForView(), &proxyModel2);
  /*
   * Remove proxy models
   */
  controller.removeProxyModel(&proxyModel1);
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.modelForView(), &proxyModel2);
  controller.removeProxyModel(&proxyModel2);
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.modelForView(), &model1);
}

void ControllerTest::addRemoveProxyModelSignalTest()
{
  ItemModelControllerTester controller;
  QVariantList arguments;
  QSignalSpy sourceModelSpy(&controller, &ItemModelControllerTester::sourceModelChanged);
  QSignalSpy ModelForViewSpy(&controller, &ItemModelControllerTester::modelForViewChanged);
  QVERIFY(sourceModelSpy.isValid());
  QVERIFY(ModelForViewSpy.isValid());
  /*
   * Set a model
   */
  QStringListModel model1;
  controller.setModel(&model1);
  sourceModelSpy.clear();
  ModelForViewSpy.clear();
  /*
   * Append a proxy model
   */
  QSortFilterProxyModel proxyModel1;
  controller.appendProxyModel(&proxyModel1);
  // Check source model signal
  QCOMPARE(sourceModelSpy.count(), 0);
  // Check model for view signal
  QCOMPARE(ModelForViewSpy.count(), 1);
  arguments = ModelForViewSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &proxyModel1);
  /*
   * Prepend a proxy model
   */
  QSortFilterProxyModel proxyModel2;
  controller.prependProxyModel(&proxyModel2);
  // Check source model signal
  QCOMPARE(sourceModelSpy.count(), 0);
  // Check model for view signal
  QCOMPARE(ModelForViewSpy.count(), 0);
  /*
   * Remove first proxy model
   */
  controller.removeProxyModel(&proxyModel2);
  // Check source model signal
  QCOMPARE(sourceModelSpy.count(), 0);
  // Check model for view signal
  QCOMPARE(ModelForViewSpy.count(), 0);
  /*
   * Remove last proxy model
   */
  controller.removeProxyModel(&proxyModel1);
  // Check source model signal
  QCOMPARE(sourceModelSpy.count(), 0);
  // Check model for view signal
  QCOMPARE(ModelForViewSpy.count(), 1);
  arguments = ModelForViewSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &model1);
}

void ControllerTest::filterCheckModelTest()
{
  ItemModelControllerTester controller;
  /*
   * Set a model
   */
  QStringListModel model1;
  controller.setModel(&model1);
  QCOMPARE(controller.model(), &model1);
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.modelForView(), &model1);
  /*
   * Initial state
   */
  QVERIFY(!controller.isFilterEnabled());
  /*
   * Enable filter
   */
  controller.setFilterEnabled(true);
  QVERIFY(controller.isFilterEnabled());
  QCOMPARE(controller.sourceModel(), &model1);
  QVERIFY(controller.modelForView() != nullptr);
  QVERIFY(controller.modelForView() != &model1);
  /*
   * Disable filter
   */
  controller.setFilterEnabled(false);
  QVERIFY(!controller.isFilterEnabled());
  QCOMPARE(controller.sourceModel(), &model1);
  QCOMPARE(controller.modelForView(), &model1);
  /*
   * Check that setting a filter also enables filter
   */
  controller.setFilter(FilterColumn(0) == 2);
  QVERIFY(controller.isFilterEnabled());
}

void ControllerTest::filterCheckModelSignalTest()
{
  ItemModelControllerTester controller;
  QVariantList arguments;
  QSignalSpy sourceModelSpy(&controller, &ItemModelControllerTester::sourceModelChanged);
  QSignalSpy ModelForViewSpy(&controller, &ItemModelControllerTester::modelForViewChanged);
  QVERIFY(sourceModelSpy.isValid());
  QVERIFY(ModelForViewSpy.isValid());
  /*
   * Set a model
   */
  QStringListModel model1;
  controller.setModel(&model1);
  sourceModelSpy.clear();
  ModelForViewSpy.clear();
  /*
   * Enable filter
   */
  controller.setFilterEnabled(true);
  // Check source model signal
  QCOMPARE(sourceModelSpy.count(), 0);
  // Check model for view signal
  QCOMPARE(ModelForViewSpy.count(), 1);
  arguments = ModelForViewSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QVERIFY(arguments.at(0).value<QAbstractItemModel*>() != nullptr);
  QVERIFY(arguments.at(0).value<QAbstractItemModel*>() != &model1);
  /*
   * Disable filter
   */
  controller.setFilterEnabled(false);
  // Check source model signal
  QCOMPARE(sourceModelSpy.count(), 0);
  // Check model for view signal
  QCOMPARE(ModelForViewSpy.count(), 1);
  arguments = ModelForViewSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0).value<QAbstractItemModel*>(), &model1);

  QFAIL("Not complete");
}

void ControllerTest::primaryKeyTest()
{
  PrimaryKey pk;
  ItemModelControllerTester controller;
  /*
   * Initial state
   */
  QVERIFY(!controller.isPrimaryKeyEnabled());
  QVERIFY(controller.getPrimaryKey().isNull());
  QCOMPARE(controller.primaryKeyChangedEventCount(), 0);
  /*
   * Set primary key
   * (Must also enable primary key support)
   */
  controller.setPrimaryKey({1});
  QVERIFY(controller.isPrimaryKeyEnabled());
  QCOMPARE(controller.getPrimaryKey().columnCount(), 1);
  QCOMPARE(controller.getPrimaryKey().greatestColumn(), 1);
  QCOMPARE(controller.primaryKeyChangedEventCount(), 1);
  controller.clearPrimaryKeyChangedEventCount();
  /*
   * Check some flags
   */
  controller.setPrimaryKeyEditable(false);
  QVERIFY(!controller.getPrimaryKeyProxyModel()->isPrimaryKeyEditable());
  controller.setPrimaryKeyItemsEnabled(false);
  QVERIFY(!controller.getPrimaryKeyProxyModel()->isPrimaryKeyItemsEnabled());
  /*
   * Set model
   */
  VariantTableModel model;
  model.resize(1, 3);
  controller.setModel(&model);
  QCOMPARE(controller.primaryKeyChangedEventCount(), 1);
  controller.clearPrimaryKeyChangedEventCount();
  /*
   * Disable primary key support
   */
  QVERIFY(!controller.getPrimaryKey().isNull());
  controller.setPrimaryKeyEnabled(false);
  QVERIFY(controller.getPrimaryKey().isNull());
  QCOMPARE(controller.primaryKeyChangedEventCount(), 1);
  controller.clearPrimaryKeyChangedEventCount();
}

void ControllerTest::foreignKeyTest()
{
  ForeignKey fk;
  ItemModelControllerTester controller;
  /*
   * Initial state
   */
  QVERIFY(!controller.isForeignKeyEnabled());
  QVERIFY(controller.getForeignKey().isNull());
  /*
   * Set foreign key
   * (Must also enable foreign key support)
   */
  controller.setForeignKey({2});
  QVERIFY(controller.isForeignKeyEnabled());
  QCOMPARE(controller.getForeignKey().columnCount(), 1);
  QCOMPARE(controller.getForeignKey().greatestColumn(), 2);
  /*
   * Check some flags
   */
  controller.setForeignKeyEditable(false);
  QVERIFY(!controller.getForeignKeyProxyModel()->isForeignKeyEditable());
  controller.setForeignKeyItemsEnabled(false);
  QVERIFY(!controller.getForeignKeyProxyModel()->isForeignKeyItemsEnabled());
  /*
   * Disable foreign key support
   */
  QVERIFY(!controller.getForeignKey().isNull());
  controller.setForeignKeyEnabled(false);
  QVERIFY(controller.getForeignKey().isNull());

  QFAIL("Not complete");
}

void ControllerTest::currentRowTest()
{
  QFAIL("Not complete");
}

void ControllerTest::currentRowSignalsTest()
{
  QFAIL("Not complete");
}


// void ControllerTest::controllerListTest()
// {
//   ControllerList list;
//   TableViewController tableController;
// 
//   /*
//    * Initial state
//    */
//   QCOMPARE(list.size(), 0);
//   QVERIFY(list.isEmpty());
//   QVERIFY(list.cbegin() == list.cend());
//   /*
//    * Add 1 element
//    */
//   list.addController(&tableController);
//   QCOMPARE(list.size(), 1);
//   QVERIFY(!list.isEmpty());
//   QVERIFY(list.cbegin() != list.cend());
//   /*
//    * Clear
//    */
//   list.clear();
//   QVERIFY(list.isEmpty());
// }

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
