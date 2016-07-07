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
#include "ActionsTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/RowState.h"
#include "Mdt/ItemEditor/RowChangeEventMapper.h"
#include "Mdt/ItemEditor/NavigationActions.h"
#include <QSignalSpy>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QTableView>
#include <QPointer>
#include <QAction>

/// For sandbox
#include <QStyledItemDelegate>
#include <QEvent>
#include <QStyleOptionViewItem>
#include <QModelIndex>

/// For menuSandbox
#include <QMenu>


#include <QDebug>

void ActionsTest::initTestCase()
{
}

void ActionsTest::cleanupTestCase()
{
}

/**
 * For sandbox
 */

class sandboxDelegate : public QStyledItemDelegate
{
 Q_OBJECT

 public:
   
  sandboxDelegate(QObject* parent = nullptr)
   : QStyledItemDelegate(parent)
   {
   }
   
   
  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const override
  {
    qDebug() << "Create editor ...";
    return nullptr;
    return QStyledItemDelegate::createEditor(parent, option, index);
  }

  void setEditorData(QWidget *editor, const QModelIndex & index) const override
  {
    qDebug() << "Set editor data ...";
    QStyledItemDelegate::setEditorData(editor, index);
  }
  
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const override
  {
    qDebug() << "Set model data ...";
    QStyledItemDelegate::setModelData(editor, model, index);
  }

//    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem & option, const QModelIndex & index) override
//    {
//      qDebug() << "editorEvent(): event: " << event->type();
//      return false;
//    }
};

#include "ActionsTest.moc"

/*
 * Tests
 */

void ActionsTest::sandbox()
{
  sandboxDelegate delegate;
  QStringListModel model;
  QTableView tableView;
  QStringList list;
  
  tableView.setModel(&model);
  tableView.setEditTriggers(QTableView::DoubleClicked | QTableView::EditKeyPressed);
  tableView.setItemDelegate(&delegate);
  
  list << "A" << "B" << "C";
  model.setStringList(list);
  
  tableView.show();
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void ActionsTest::menuSandbox()
{
  QMenu *menu;
  QPointer<QAction> actA;
  QPointer<QAction> act1;

  menu = new QMenu;
  menu->setObjectName("menu");
  actA = menu->addAction("Action A");
  actA->setObjectName("actA");
  
  act1 = new QAction("Action 1", menu);
  act1->setObjectName("act1");
  menu->addAction(act1);
  
  menu->dumpObjectInfo();
  menu->dumpObjectTree();
  
  qDebug() << "   actA: " << actA << " , parent: " << actA->parent();
  qDebug() << "   act1: " << act1 << " , parent: " << act1->parent();

  qDebug() << "delete menu...";
  delete menu;
  
  qDebug() << "-> actA: " << actA;
  qDebug() << "-> act1: " << act1;
  
//   QTest::qWait(1000000);
  
}

void ActionsTest::rowStateTest()
{
  using Mdt::ItemEditor::RowState;

  /*
   * Initial state
   */
  RowState rs;
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  QVERIFY(rs.isValid());
  QVERIFY(rs.isNull());
  /*
   * Set
   */
  // N: 1 , Current: -1
  rs.setRowCount(1);
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 1 , Current: 0
  rs.setCurrentRow(0);
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  QVERIFY(rs.isValid());
  QVERIFY(!rs.isNull());
  // N: 1 , Current: 1
  rs.setRowCount(1);
  rs.setCurrentRow(1);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 2 , Current: -1
  rs.setRowCount(2);
  rs.setCurrentRow(-1);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 2 , Current: 0
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  QVERIFY(rs.isValid());
  QVERIFY(!rs.isNull());
  // N: 2 , Current: 1
  rs.setRowCount(2);
  rs.setCurrentRow(1);
  QVERIFY(rs.isValid());
  QVERIFY(!rs.isNull());
  // N: 2 , Current: 2
  rs.setRowCount(2);
  rs.setCurrentRow(2);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  // N: 2 , Current: 3
  rs.setRowCount(2);
  rs.setCurrentRow(3);
  QVERIFY(!rs.isValid());
  QVERIFY(rs.isNull());
  /*
   * Clear
   */
  rs.clear();
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  QVERIFY(rs.isValid());
  QVERIFY(rs.isNull());
}

void ActionsTest::rowChangeEventMapperTest()
{
  using Mdt::ItemEditor::RowChangeEventMapper;

  RowChangeEventMapper mapper;
  QSignalSpy rowCountSpy(&mapper, &RowChangeEventMapper::rowCountChanged);
  QSignalSpy currentRowSpy(&mapper, &RowChangeEventMapper::currentRowChanged);
  QList<QVariant> spyItem;
  QStringListModel model;
  QModelIndex index;

  /*
   * Initial state
   */
  QVERIFY(rowCountSpy.isValid());
  QCOMPARE(rowCountSpy.count(), 0);
  QVERIFY(currentRowSpy.isValid());
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Set model
   */
  mapper.setModel(&model);
  // Check that row count was signaled
  QCOMPARE(rowCountSpy.count(), 1);
  spyItem = rowCountSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 0);
  // Current is not updated here (the selection model will signal it to mapper)
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Add a item to model
   *  - Use setStringList() helper method
   */
  model.setStringList({"A"});
  // Check that row count was signaled
  QCOMPARE(rowCountSpy.count(), 1);
  spyItem = rowCountSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 1);
  // Current is not updated here (the selection model will signal it to mapper)
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Add a item to model
   *  - Use insertRow()
   */
  QVERIFY(model.insertRow(0));
  // Check that row count was signaled
  QCOMPARE(rowCountSpy.count(), 1);
  spyItem = rowCountSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 2);
  // Current is not updated here (the selection model will signal it to mapper)
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Remove a item from model
   */
  QVERIFY(model.removeRow(0));
  // Check that row count was signaled
  QCOMPARE(rowCountSpy.count(), 1);
  spyItem = rowCountSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 1);
  // Current is not updated here (the selection model will signal it to mapper)
  QCOMPARE(currentRowSpy.count(), 0);
  /*
   * Set a selection model
   */
  QItemSelectionModel selectionModel(&model);
  mapper.setSelectionModel(&selectionModel);
  // Selection model's model is the same as before, so, now row count must be signaled
  QCOMPARE(rowCountSpy.count(), 0);
  // Check that current row was signaled
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), -1);
  /*
   * Change current row
   */
  index = model.index(0, 0);
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // No row count has changed
  QCOMPARE(rowCountSpy.count(), 0);
  // Check that current row was signaled
  QCOMPARE(currentRowSpy.count(), 1);
  spyItem = currentRowSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 0);
  /*
   * Add a item to model
   *  - Use insertRow()
   */
  QVERIFY(model.insertRow(0));
  // Check that row count was signaled
  QCOMPARE(rowCountSpy.count(), 1);
  spyItem = rowCountSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 2);
  // Check that current row was signaled
  /**
   * \todo Must define what is good when inserting/removing rows.
   *       F.ex. when inserting before row 0, the new current row becomes 1,
   *       but dit not change. Then, to edit the inserted row we have to go to
   *       row 0, which will also be signaled.
   */
//   QCOMPARE(currentRowSpy.count(), 1);
//   spyItem = currentRowSpy.takeFirst();
//   QCOMPARE(spyItem.at(0).toInt(), 1);
  /*
   * Remove a item from model
   */
  QVERIFY(model.removeRow(0));
  // Check that row count was signaled
  QCOMPARE(rowCountSpy.count(), 1);
  spyItem = rowCountSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 1);
  // Check that current row was signaled
  /// \todo define what to do here
//   QCOMPARE(currentRowSpy.count(), 1);
//   spyItem = currentRowSpy.takeFirst();
//   QCOMPARE(spyItem.at(0).toInt(), 0);
  /*
   * Set items to model
   */
  model.setStringList({"A", "B", "C", "D"});
  // Check that row count was signaled
  QCOMPARE(rowCountSpy.count(), 1);
  spyItem = rowCountSpy.takeFirst();
  QCOMPARE(spyItem.at(0).toInt(), 4);
  // Check that current row was signaled
  /// \todo define what to do here
//   QCOMPARE(currentRowSpy.count(), 1);
//   spyItem = currentRowSpy.takeFirst();
//   QCOMPARE(spyItem.at(0).toInt(), 0);

  
  /*
   * Change model of selection model
   */
  /// \todo implement

  /*
   * Play
   */
  QTableView tv;
  tv.setModel(&model);
  mapper.setSelectionModel(tv.selectionModel());
  model.setStringList({"A", "B", "C"});
  tv.show();
  while(tv.isVisible()){
    QTest::qWait(500);
  }
  
  QFAIL("Test not completely implemented yet");
}

void ActionsTest::navigationActionsTest()
{
  using Mdt::ItemEditor::NavigationActions;
  using Mdt::ItemEditor::RowState;

  NavigationActions *actions = new NavigationActions(nullptr);
  QPointer<QAction> toFirst = actions->toFirst();
  QPointer<QAction> toPrevious = actions->toPrevious();
  QPointer<QAction> toNext = actions->toNext();
  QPointer<QAction> toLast = actions->toLast();
  RowState rs;
  /*
   * Initial state
   */
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  /*
   * Check row changes
   */
  // N: 0 , current row: -1
  rs.setRowCount(0);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  // N: 1 , current row: 0
  rs.setRowCount(1);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  // N: 2 , current row: 0
  rs.setRowCount(2);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 2 , current row: 1
  rs.setRowCount(2);
  rs.setCurrentRow(1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  // N: 3 , current row: 0
  rs.setRowCount(3);
  rs.setCurrentRow(0);
  actions->setRowState(rs);
  QVERIFY(!toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 3 , current row: 1
  rs.setRowCount(3);
  rs.setCurrentRow(1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 3 , current row: 2
  rs.setRowCount(3);
  rs.setCurrentRow(2);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(toPrevious->isEnabled());
  QVERIFY(!toNext->isEnabled());
  QVERIFY(!toLast->isEnabled());
  /*
   * Clear
   */
  delete actions;
  QVERIFY(toFirst.isNull());
  QVERIFY(toPrevious.isNull());
  QVERIFY(toNext.isNull());
  QVERIFY(toLast.isNull());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ActionsTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
