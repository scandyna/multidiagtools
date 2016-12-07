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
#include "ItemDelegateTest.h"
#include "ItemViewTestEdit.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/ItemDelegateProxy.h"
#include "Mdt/ItemEditor/EventCatchItemDelegate.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
// #include <QItemSelectionModel>
// #include <QStringListModel>
#include <QTableView>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>

#include <QDebug>

using Mdt::ItemModel::VariantTableModel;

void ItemDelegateTest::initTestCase()
{
}

void ItemDelegateTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ItemDelegateTest::sandbox()
{
  using Mdt::ItemEditor::ItemDelegateProxy;

  ItemDelegateProxy delegateProxy;
//   sandboxDelegate delegate;
  QStringListModel model;
  QTableView tableView;
  QStringList list;
  
  tableView.setModel(&model);
  tableView.setEditTriggers(QTableView::DoubleClicked | QTableView::EditKeyPressed);
  delegateProxy.setItemDelegate(tableView.itemDelegate());
  tableView.setItemDelegate(&delegateProxy);
  
  list << "A" << "B" << "C";
  model.setStringList(list);
  
  tableView.show();
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
}

void ItemDelegateTest::itemDelegateProxyTest()
{
  using Mdt::ItemEditor::ItemDelegateProxy;

  QTableView view;
  QWidget *editor1, *editor2;
  QStringListModel model;
  QStringList list;
  QStyledItemDelegate delegate;
  ItemDelegateProxy *proxy;
  QStyleOptionViewItem option;
  QModelIndex index;
  QEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::NoModifier);
  QPainter painter;
  QLineEdit *lineEdit;

  /// \todo Intercept signals

  /*
   * Setup model and view
   */
  list << "A" << "B" << "C";
  model.setStringList(list);
  view.setModel(&model);
  /*
   * Get a model index that we will use for various checks
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  /*
   * Check when delegate was not allready set
   */
  proxy = new ItemDelegateProxy;
  QVERIFY(proxy->itemDelegate() == nullptr);
  view.setItemDelegate(proxy);
  // Check paint()
  proxy->paint(&painter, option, index);
  // Check sizeHint()
  QVERIFY(!proxy->sizeHint(option, index).isValid());
  // Check createEditor()
  editor1 = proxy->createEditor(&view, option, index);
  QVERIFY(editor1 == nullptr);
  // Check setEditorData()
  lineEdit = new QLineEdit;
  proxy->setEditorData(lineEdit, index);
  QVERIFY(lineEdit->text().isEmpty());
  // Check setModelData()
  lineEdit->setText("Z");
  proxy->setModelData(lineEdit, &model, index);
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("A"));
  delete lineEdit;
  // Check editorEvent()
  QVERIFY(!proxy->editorEvent(event, &model, option, index));
  /// \todo Check destroyEditor()
  /// \todo Check helpEvent()
  /// \todo Check updateEditorGeometry()
  
  // 
  /*
   * Checks with delegate set
   */
  proxy->setItemDelegate(&delegate);
  QVERIFY(proxy->itemDelegate() == &delegate);
  // Check paint()
  proxy->paint(&painter, option, index);
  // Check sizeHint()
  QVERIFY(proxy->sizeHint(option, index).isValid());
  // Check createEditor()
  editor1 = proxy->createEditor(&view, option, index);
  QVERIFY(editor1 != nullptr);
//   editor2 = delegate.createEditor(&view, option, index);
//   QVERIFY(editor1 == editor2);
  // Check setEditorData()
  lineEdit = dynamic_cast<QLineEdit*>(editor1);
  QVERIFY(lineEdit != nullptr);
  proxy->setEditorData(lineEdit, index);
  QCOMPARE(lineEdit->text(), QString("A"));
  // Check setModelData()
  lineEdit->setText("Z");
  proxy->setModelData(lineEdit, &model, index);
  QCOMPARE(model.data(index, Qt::DisplayRole), QVariant("Z"));
  // Check editorEvent()
  QVERIFY(proxy->editorEvent(event, &model, option, index));
  /// \todo Check destroyEditor()
  /// \todo Check helpEvent()
  /// \todo Check updateEditorGeometry()
  
  // 
  /*
   * Delete proxy
   */
  delete proxy;
  /// call some functions on delegate
  
  delete event;
  
  /*! \todo We should create a test model,
   *   that is a table, and that support
   *   Qt::DisplayRole and Qt::EditRole.
   *   Then, do the test with a QTableView
   *   with various types (int, text, ..).
   *   Once done so, we understand more
   *   on delegates, and could rewrite a better test
   *   that acts directly on a delegate later.
   */
}

void ItemDelegateTest::itemDelegateProxyTableViewTest()
{
  using Mdt::ItemEditor::ItemDelegateProxy;

  VariantTableModel model;
  QTableView view;
  ItemDelegateProxy proxy;

  /*
   * Populate model
   */
  model.populate(3, 2);
  /*
   * Setup view
   */
  view.setModel(&model);
  proxy.setItemDelegate(view.itemDelegate());
  view.setItemDelegate(&proxy);
  view.resize(300, 200);
  view.show();

  /*
   * Play
   */
//   while(view.isVisible()){
//     QTest::qWait(500);
//   }
}

void ItemDelegateTest::itemDelegateProxyTableViewEditTest()
{
  using Mdt::ItemEditor::ItemDelegateProxy;

  QTableView view;
  ItemDelegateProxy proxy;
  VariantTableModel model;
  QModelIndex index;
  QFETCH(BeginEditTrigger, beginEditTrigger);
  QFETCH(EndEditTrigger, endEditTrigger);

  /*
   * Setup model and view
   */
  model.populate(2, 1);
  view.setModel(&model);
  proxy.setItemDelegate(view.itemDelegate());
  view.setItemDelegate(&proxy);
  view.show();
  /*
   * Check
   */
  // Check at row 0 and column 0
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index) != QVariant("TEST"));
  edit(view, index, "TEST", beginEditTrigger, endEditTrigger);
  QCOMPARE(model.data(index), QVariant("TEST"));

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
 * Helper functions
 */

void ItemDelegateTest::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void ItemDelegateTest::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex, EndEditTrigger trigger)
{
  ItemViewTestEdit::endEditing(view, editingIndex, trigger);
}

void ItemDelegateTest::edit(QAbstractItemView& view, const QModelIndex& index, const QString& str,
                            BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  ItemViewTestEdit::edit(view, index, str, beginEditTrigger, endEditTrigger);
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
