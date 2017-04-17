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
#include "TableViewControllerImplementationTest.h"
#include "ItemModelControllerTester.h"
#include "Mdt/ItemEditor/TableViewControllerImplementation.h"
#include "RowStateChangedSpy.h"
// #include "Mdt/ItemEditor/TableViewController.h"
// #include "Mdt/ItemEditor/ControllerStateMachine.h"
// #include "Mdt/ItemEditor/ControllerState.h"
#include "Mdt/ItemEditor/ControllerEvent.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include <QStringList>
// #include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QObject>
#include <QMetaObject>
// #include <QMetaProperty>
// #include <QMetaMethod>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

void TableViewControllerImplementationTest::initTestCase()
{
}

void TableViewControllerImplementationTest::cleanupTestCase()
{
}

/*
 * Tests
 * Complete tests are made in TableViewControllerTest
 */

void TableViewControllerImplementationTest::setModelThenViewTest()
{
  /*
   * We need a controller to check connectToController()
   * Set the model to it, and get its modelForView for our tests
   */
  VariantTableModel tableModel;
  tableModel.resize(3, 2);
  ItemModelControllerTester controller;
  controller.setModel(&tableModel);
  /*
   * Initial state
   */
  TableViewControllerImplementation controllerImpl;
  QVERIFY(controllerImpl.model() == nullptr);
  QVERIFY(controllerImpl.view() == nullptr);
  /*
   * Set model
   */
  controllerImpl.setModel(controller.modelForView());
  QCOMPARE(controllerImpl.model(), controller.modelForView());
  QVERIFY(controllerImpl.view() == nullptr);
  QVERIFY(!controllerImpl.connectToController(&controller));
  /*
   * Affect view to controller
   */
  QTableView tableView;
  controllerImpl.setView(&tableView);
  QCOMPARE(controllerImpl.model(), controller.modelForView());
  QCOMPARE(controllerImpl.view(), &tableView);
  QVERIFY(controllerImpl.connectToController(&controller));
  /*
   * Change model
   */
  QStringListModel listModel(QStringList({"A","B"}));
  controllerImpl.setModel(&listModel);
  QVERIFY(controllerImpl.model() == &listModel);
  QCOMPARE(controllerImpl.view(), &tableView);
}

void TableViewControllerImplementationTest::setViewThenModelTest()
{
  /*
   * We need a controller to check connectToController()
   * Set the model to it, and get its modelForView for our tests
   */
  VariantTableModel tableModel;
  tableModel.resize(3, 2);
  ItemModelControllerTester controller;
  controller.setModel(&tableModel);
  /*
   * Initial state
   */
  TableViewControllerImplementation controllerImpl;
  QVERIFY(controllerImpl.model() == nullptr);
  QVERIFY(controllerImpl.view() == nullptr);
  /*
   * Affect view to controller
   */
  QTableView tableView;
  controllerImpl.setView(&tableView);
  QVERIFY(controllerImpl.model() == nullptr);
  QCOMPARE(controllerImpl.view(), &tableView);
  QVERIFY(!controllerImpl.connectToController(&controller));
  /*
   * Set model
   */
  controllerImpl.setModel(controller.modelForView());
  QCOMPARE(controllerImpl.model(), controller.modelForView());
  QCOMPARE(controllerImpl.view(), &tableView);
  QVERIFY(controllerImpl.connectToController(&controller));
}

void TableViewControllerImplementationTest::primaryKeyVisibilityTest()
{
  /*
   * Setup
   */
  VariantTableModel model;
  model.resize(1, 3);
  QTableView view;
  TableViewControllerImplementation controllerImpl;
  controllerImpl.setModel(&model);
  controllerImpl.setView(&view);
  PrimaryKey pk({0,1});
  /*
   * By default, all columns are visible
   */
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Hide primary key
   */
  controllerImpl.setPrimaryKeyHidden(pk, true);
  QVERIFY( view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Change primary key
   */
  PrimaryKey pk2({1});
  controllerImpl.primaryKeyChangedEvent(pk, pk2);
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Show primary key
   */
  controllerImpl.setPrimaryKeyHidden(pk2, false);
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
}

void TableViewControllerImplementationTest::foreignKeyVisibilityTest()
{
  /*
   * Setup
   */
  VariantTableModel model;
  model.resize(1, 3);
  QTableView view;
  TableViewControllerImplementation controllerImpl;
  controllerImpl.setModel(&model);
  controllerImpl.setView(&view);
  ForeignKey fk({0,1});
  /*
   * By default, all columns are visible
   */
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Hide foreign key
   */
  controllerImpl.setForeignKeyHidden(fk, true);
  QVERIFY( view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Change foreign key
   */
  ForeignKey fk2({1});
  controllerImpl.foreignKeyChangedEvent(fk, fk2);
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY( view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
  /*
   * Show foreign key
   */
  controllerImpl.setForeignKeyHidden(fk2, false);
  QVERIFY(!view.isColumnHidden(0));
  QVERIFY(!view.isColumnHidden(1));
  QVERIFY(!view.isColumnHidden(2));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  TableViewControllerImplementationTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
