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
#include "Mdt/ItemEditor/RowChangeEventMapper.h"
#include <QSignalSpy>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QTableView>

/// For sandbox
#include <QStyledItemDelegate>
#include <QEvent>
#include <QStyleOptionViewItem>
#include <QModelIndex>

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
