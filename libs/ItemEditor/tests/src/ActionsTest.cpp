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
#include "TestTableModel.h"
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
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), -1);
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
  using Mdt::ItemEditor::RowState;

  RowChangeEventMapper mapper;
  QSignalSpy rowStateSpy(&mapper, &RowChangeEventMapper::rowStateChanged);
  QList<QVariant> spyItem;
  QStringListModel model;
  QModelIndex index;
  RowState rs;

  /*
   * Initial state
   */
  QVERIFY(rowStateSpy.isValid());
  QCOMPARE(rowStateSpy.count(), 0);
  /*
   * Set model
   */
  mapper.setModel(&model);
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
  QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Populate model (will produce a model reset)
   */
  model.setStringList({"A"});
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Add a item to model
   *  - Use insertRow()
   */
  QVERIFY(model.insertRow(0));
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Remove a item from model
   */
  QVERIFY(model.removeRow(0));
  // Check that row count was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Set a selection model
   */
  QItemSelectionModel selectionModel(&model);
  mapper.setSelectionModel(&selectionModel);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Change current row
   */
  index = model.index(0, 0);
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Add a item to model
   *  - Use insertRow()
   *
   * Current row is 0 ,
   * we insert before row 0 ,
   * current row will become 1.
   */
  QVERIFY(model.insertRow(0));
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 2);
  QCOMPARE(rs.currentRow(), 1);
  /*
   * Remove a item from model
   *  - use removeRow()
   *
   * Current row is 1 ,
   * we remove row 0 ,
   * current row will become 0.
   */
  QVERIFY(model.removeRow(0));
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 1);
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Populate model (will produce a model reset)
   *
   * After a model reset, selection model looses current index
   */
  model.setStringList({"A", "B", "C", "D"});
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Change current to a valid row before setModel() test
   */
  index = model.index(0, 0);
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Change model of selection model
   *  - Set a empty model
   */
  TestTableModel model2;
  selectionModel.setModel(&model2);
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 0);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Change model of selection model
   *  - Set a model with data
   */
  selectionModel.setModel(&model);
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 4);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Change again to model2 for next test
   */
  model2.populate(3, 2);
  selectionModel.setModel(&model2);
  // Check that row count and current row were signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), -1);
  /*
   * Change current row
   */
  index = model2.index(0, 0);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was signaled
  QCOMPARE(rowStateSpy.count(), 1);
  spyItem = rowStateSpy.takeFirst();
  rs = spyItem.at(0).value<RowState>();
//   QVERIFY(rs.isValid());
  QCOMPARE(rs.rowCount(), 3);
  QCOMPARE(rs.currentRow(), 0);
  /*
   * Change current column
   */
  index = model2.index(0, 1);
  QVERIFY(index.isValid());
  selectionModel.setCurrentIndex(index, QItemSelectionModel::Current);
  // Check that current row was not signaled
  QCOMPARE(rowStateSpy.count(), 0);
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
   * Check with some invalid row states
   */
  // N: 1 , current row: -1
  rs.setRowCount(1);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
  // N: 2 , current row: -1
  rs.setRowCount(2);
  rs.setCurrentRow(-1);
  actions->setRowState(rs);
  QVERIFY(toFirst->isEnabled());
  QVERIFY(!toPrevious->isEnabled());
  QVERIFY(toNext->isEnabled());
  QVERIFY(toLast->isEnabled());
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
