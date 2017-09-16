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
#include "ItemViewPrivateContainerTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/ItemViewPrivateContainer.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/PrimaryKeyProxyModel.h"
#include "Mdt/ItemEditor/EventCatchItemDelegate.h"
#include "Mdt/ItemEditor/ItemSelectionModel.h"
#include <QTableView>
#include <QPointer>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

void ItemViewPrivateContainerTest::initTestCase()
{
}

void ItemViewPrivateContainerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ItemViewPrivateContainerTest::setModelThenViewTest()
{
  VariantTableModel model;
  QTableView view;
  ItemViewPrivateContainer container;
  auto *delegate = view.itemDelegate();
  /*
   * Initial state
   */
  QVERIFY(container.model() == nullptr);
  QVERIFY(container.view() == nullptr);
  QVERIFY(container.itemDelegate() == nullptr);
  QVERIFY(container.proxyItemDelegate() != nullptr);
  QVERIFY(container.selectionModel() != nullptr);
  QVERIFY(container.selectionModel()->model() == nullptr);
  QVERIFY(view.model() == nullptr);
  /*
   * Set model
   */
  container.setModel(&model);
  QVERIFY(container.model() == &model);
  QVERIFY(container.selectionModel()->model() == &model);
  /*
   * Set view
   */
  container.setView(&view);
  QVERIFY(container.view() == &view);
  // Check model and selection model
  QVERIFY(view.model() == &model);
  QVERIFY(view.selectionModel() == container.selectionModel());
  QVERIFY(view.selectionModel()->model() == &model);
  // Check item delegate
  QVERIFY(view.itemDelegate() == container.proxyItemDelegate());
  QVERIFY(container.itemDelegate() == delegate);
}

void ItemViewPrivateContainerTest::setViewThenModelTest()
{
  VariantTableModel model;
  QTableView view;
  ItemViewPrivateContainer container;
  auto *delegate = view.itemDelegate();
  /*
   * Initial state
   */
  QVERIFY(container.model() == nullptr);
  QVERIFY(container.view() == nullptr);
  QVERIFY(container.itemDelegate() == nullptr);
  QVERIFY(container.proxyItemDelegate() != nullptr);
  QVERIFY(container.selectionModel() != nullptr);
  QVERIFY(container.selectionModel()->model() == nullptr);
  QVERIFY(view.model() == nullptr);
  /*
   * Set view
   */
  container.setView(&view);
  QVERIFY(container.view() == &view);
  /*
   * Set model
   */
  container.setModel(&model);
  // Check model
  QVERIFY(container.model() == &model);
  QVERIFY(view.model() == &model);
  // Check selection model
  QVERIFY(container.selectionModel()->model() == &model);
  QVERIFY(view.selectionModel() == container.selectionModel());
  QVERIFY(view.selectionModel()->model() == &model);
  // Check item delegate
  QVERIFY(view.itemDelegate() == container.proxyItemDelegate());
  QVERIFY(container.itemDelegate() == delegate);
}

void ItemViewPrivateContainerTest::setProxyModelThenModelTest()
{
  VariantTableModel model;
  PrimaryKeyProxyModel proxyModel;
  QTableView view;
  ItemViewPrivateContainer container;
  auto *delegate = view.itemDelegate();

  /*
   * Set proxy model
   */
  container.setModel(&proxyModel);
  QVERIFY(container.model() == &proxyModel);
  QVERIFY(container.selectionModel()->model() == &proxyModel);
  /*
   * Set view
   */
  container.setView(&view);
  QVERIFY(container.view() == &view);
  /*
   * Set source model to proxy model
   * (TableViewController is in charge of setting the model again, here we do it explicitly)
   * This produced the crash detetcetd at 20170311
   */
  proxyModel.setSourceModel(&model);
  container.setModel(&proxyModel);
  QVERIFY(container.model() == &proxyModel);
  QVERIFY(container.selectionModel()->model() == &proxyModel);
}

void ItemViewPrivateContainerTest::delegateLifeTimeTest()
{
  QPointer<QAbstractItemDelegate> delegate;
  QTableView view;
  /*
   * Initial state
   */
  auto *container = new ItemViewPrivateContainer;
  QVERIFY(container->proxyItemDelegate() != nullptr);
  QVERIFY(container->itemDelegate() == nullptr);
  /*
   * Set view
   */
  delegate = view.itemDelegate();
  QVERIFY(!delegate.isNull());
  container->setView(&view);
  // Check that proxy delegate was set to the view
  QVERIFY(view.itemDelegate() == container->proxyItemDelegate());
  // Check also that delegate was set to proxy delegate
  QVERIFY(container->itemDelegate() == delegate);
  QVERIFY(container->proxyItemDelegate()->itemDelegate() == delegate);
  /*
   * Delete container
   */
  delete container;
  QVERIFY(!delegate.isNull());
  QVERIFY(view.itemDelegate() == delegate);
}

void ItemViewPrivateContainerTest::selectionModelLifeTimeTest()
{
  VariantTableModel model;
  QTableView view;
  /*
   * Initial state
   */
  auto *container = new ItemViewPrivateContainer;
  QVERIFY(container->selectionModel() != nullptr);
  /*
   * Set model and view
   */
  container->setModel(&model);
  container->setView(&view);
  QVERIFY(view.selectionModel() == container->selectionModel());
  QVERIFY(view.selectionModel()->model() == &model);
  /*
   * Delete container
   */
  delete container;
  QVERIFY(view.selectionModel() == nullptr);
}

void ItemViewPrivateContainerTest::changeModelTest()
{
  QTableView view;
  ItemViewPrivateContainer container;
  auto *delegate = view.itemDelegate();
  /*
   * Set model and view
   */
  VariantTableModel model1;
  container.setModel(&model1);
  container.setView(&view);
  // Check model
  QVERIFY(container.model() == &model1);
  QVERIFY(view.model() == &model1);
  // Check selection model
  QVERIFY(container.selectionModel()->model() == &model1);
  QVERIFY(view.selectionModel() == container.selectionModel());
  QVERIFY(view.selectionModel()->model() == &model1);
  // Check item delegate
  QVERIFY(view.itemDelegate() == container.proxyItemDelegate());
  QVERIFY(container.itemDelegate() == delegate);
  /*
   * Change model
   */
  VariantTableModel model2;
  container.setModel(&model2);
  // Check model
  QVERIFY(container.model() == &model2);
  QVERIFY(view.model() == &model2);
  // Check selection model
  QVERIFY(container.selectionModel()->model() == &model2);
  QVERIFY(view.selectionModel() == container.selectionModel());
  QVERIFY(view.selectionModel()->model() == &model2);
}

void ItemViewPrivateContainerTest::changeViewTest()
{
  VariantTableModel model;
  ItemViewPrivateContainer container;
  /*
   * Set model and view
   */
  QTableView view1;
  auto *delegate1 = view1.itemDelegate();
  container.setModel(&model);
  container.setView(&view1);
  // Check model
  QVERIFY(container.model() == &model);
  QVERIFY(view1.model() == &model);
  // Check selection model
  QVERIFY(container.selectionModel()->model() == &model);
  QVERIFY(view1.selectionModel() == container.selectionModel());
  QVERIFY(view1.selectionModel()->model() == &model);
  // Check item delegate
  QVERIFY(view1.itemDelegate() == container.proxyItemDelegate());
  QVERIFY(container.itemDelegate() == delegate1);
  /*
   * Change view
   */
  QTableView view2;
  auto *delegate2 = view2.itemDelegate();
  container.setView(&view2);
  // Check model
  QVERIFY(container.model() == &model);
  QVERIFY(view2.model() == &model);
  // Check selection model
  QVERIFY(container.selectionModel()->model() == &model);
  QVERIFY(view2.selectionModel() == container.selectionModel());
  QVERIFY(view2.selectionModel()->model() == &model);
  // Check item delegate
  QVERIFY(view2.itemDelegate() == container.proxyItemDelegate());
  QVERIFY(container.itemDelegate() == delegate2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ItemViewPrivateContainerTest test;

  return QTest::qExec(&test, argc, argv);
}
