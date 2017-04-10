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
#include "ItemDelegateTest.h"
#include "Mdt/ItemEditor/ItemDelegateProxy.h"
#include "Mdt/ItemEditor/EventCatchItemDelegate.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QTableView>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

void ItemDelegateTest::initTestCase()
{
}

void ItemDelegateTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ItemDelegateTest::itemDelegateProxyTableViewTest()
{
  QTableView view;
  /*
   * Setup model and view
   */
  VariantTableModel model;
  model.resize(3, 2);
  view.setModel(&model);
  view.show();
  /*
   * Check without a item delegate set to proxy
   */
  ItemDelegateProxy proxy;
  auto *originalDelegate = view.itemDelegate();
  view.setItemDelegate(&proxy);
  QVERIFY(getModelData(model, 0, 0).isNull());
  edit(view, 0, 0, "Test 1", BeginEditTrigger::DoubleClick, EndEditTrigger::EnterKeyClick);
  QCOMPARE(getModelData(model, 0, 0), QVariant("Test 1"));
  /*
   * Set delegate to proxy
   */
  proxy.setItemDelegate(originalDelegate);
  QVERIFY(getModelData(model, 0, 1).isNull());
  edit(view, 0, 1, "Test 2", BeginEditTrigger::DoubleClick, EndEditTrigger::EnterKeyClick);
  QCOMPARE(getModelData(model, 0, 1), QVariant("Test 2"));
  /*
   * Play
   */
//   displayWidget(view);
}

void ItemDelegateTest::itemDelegateProxyTableViewEditTest()
{
  using Mdt::ItemEditor::ItemDelegateProxy;

  QTableView view;
  ItemDelegateProxy proxy;
  VariantTableModel model;
  QFETCH(BeginEditTrigger, beginEditTrigger);
  QFETCH(EndEditTrigger, endEditTrigger);

  /*
   * Setup model and view
   */
  model.resize(2, 1);
  view.setModel(&model);
  proxy.setItemDelegate(view.itemDelegate());
  view.setItemDelegate(&proxy);
  view.show();
  /*
   * Check
   */
  // Check at row 0 and column 0
  QVERIFY(getModelData(model, 0, 0) != QVariant("TEST"));
  edit(view, 0, 0, "TEST", beginEditTrigger, endEditTrigger);
  QCOMPARE(getModelData(model, 0, 0), QVariant("TEST"));
  // Check at row 1 and column 0
  QVERIFY(getModelData(model, 1, 0) != QVariant("TEST 1,0"));
  edit(view, 1, 0, "TEST 1,0", beginEditTrigger, endEditTrigger);
  QCOMPARE(getModelData(model, 1, 0), QVariant("TEST 1,0"));

  /*
   * Play
   */
//   view.resize(300, 200);
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void ItemDelegateTest::itemDelegateProxyTableViewEditTest_data()
{
  QTest::addColumn<BeginEditTrigger>("beginEditTrigger");
  QTest::addColumn<EndEditTrigger>("endEditTrigger");

  QTest::newRow("S:DClick/E:Click") << BeginEditTrigger::DoubleClick << EndEditTrigger::IndexChange;
  QTest::newRow("S:DClick/E:<Enter>") << BeginEditTrigger::DoubleClick << EndEditTrigger::EnterKeyClick;
  QTest::newRow("S:F2/E:Click") << BeginEditTrigger::F2KeyClick << EndEditTrigger::IndexChange;
  QTest::newRow("S:F2/E:<Enter>") << BeginEditTrigger::F2KeyClick << EndEditTrigger::EnterKeyClick;
}

void ItemDelegateTest::eventCatchItemDelegateTableViewEditTest()
{
  using Mdt::ItemEditor::EventCatchItemDelegate;

  QTableView view;
  EventCatchItemDelegate delegate;
  VariantTableModel model;
  QModelIndex index;
  QSignalSpy editionStartedSpy(&delegate, &EventCatchItemDelegate::dataEditionStarted);
  QSignalSpy editionDoneSpy(&delegate, &EventCatchItemDelegate::dataEditionDone);

  /*
   * Setup model and view
   */
  model.populate(2, 1);
  view.setModel(&model);
  delegate.setItemDelegate(view.itemDelegate());
  view.setItemDelegate(&delegate);
  view.show();
  /*
   * Check
   */
  QCOMPARE(editionStartedSpy.count(), 0);
  QCOMPARE(editionDoneSpy.count(), 0);
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  // Start editing
  beginEditing(view, index, BeginEditTrigger::DoubleClick);
  QCOMPARE(editionStartedSpy.count(), 1);
  QCOMPARE(editionDoneSpy.count(), 0);
  // End editing
  endEditing(view, index, EndEditTrigger::IndexChange);
  QCOMPARE(editionStartedSpy.count(), 1);
  QCOMPARE(editionDoneSpy.count(), 1);
  // Reset spys
  editionStartedSpy.clear();
  editionDoneSpy.clear();
  // Edit
  edit(view, index, "TEST", BeginEditTrigger::DoubleClick, EndEditTrigger::IndexChange);
  QCOMPARE(editionStartedSpy.count(), 1);
  QCOMPARE(editionDoneSpy.count(), 1);

  /*
   * Play
   */
//   view.resize(300, 200);
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ItemDelegateTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
