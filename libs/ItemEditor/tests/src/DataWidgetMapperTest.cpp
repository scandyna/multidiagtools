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
#include "DataWidgetMapperTest.h"
#include "TestTableModel.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/MappedWidgetList.h"
#include "Mdt/ItemEditor/DataWidgetMapper.h"
#include <QSignalSpy>
#include <QLineEdit>
#include <QModelIndex>

void DataWidgetMapperTest::initTestCase()
{
}

void DataWidgetMapperTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void DataWidgetMapperTest::mappedWidgetTest()
{
  using Mdt::ItemEditor::MappedWidget;

  auto *w = new QWidget;

  w->setObjectName("TestWidget");

  auto *mw = new MappedWidget(w, 1);
  QVERIFY(mw->widget() == w);
  QCOMPARE(mw->column(), 1);

  // MappedWidget must not own the widget
  delete mw;
  QCOMPARE(w->objectName(), QString("TestWidget"));
}

void DataWidgetMapperTest::mappedWidgetListTest()
{
  using Mdt::ItemEditor::MappedWidget;
  using Mdt::ItemEditor::MappedWidgetList;

  auto *widget1 = new QWidget;

  /*
   * Initial state
   */
  MappedWidgetList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  list.addWidget(widget1, 1);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  for(const auto & mw : list){
    QVERIFY(mw.widget() == widget1);
  }
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Free
   */
  delete widget1;
}

void DataWidgetMapperTest::setModelTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  TestTableModel model;
  QModelIndex index;
  QLineEdit edit0, edit1;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Initial state
   */
  QVERIFY(mapper.model() == nullptr);
  QVERIFY(mapper.itemDelegate() != nullptr);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Set a empty model
   */
  QCOMPARE(model.rowCount(), 0);
  mapper.setModel(&model);
  QVERIFY(mapper.model() == &model);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Map widgets
   */
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Populate model with 1 column
   */
  model.populate(3, 1);
  QCOMPARE(mapper.currentRow(), 0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(edit0.text(), model.data(index).toString());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Populate model with 2 columns
   */
  model.populate(3, 2);
  QCOMPARE(mapper.currentRow(), 0);
  // Check that current row changed was not signaled
  QCOMPARE(rowChangedSpy.count(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(edit0.text(), model.data(index).toString());
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(edit1.text(), model.data(index).toString());
  /*
   * Clear mapping
   */
  mapper.clearMapping();
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Set allready populated model
   */
  TestTableModel model2;
  model2.populate(4, 2);
  mapper.setModel(&model2);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  QCOMPARE(mapper.currentRow(), 0);
  // Check that current row changed was not signaled
  QCOMPARE(rowChangedSpy.count(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  index = model2.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(edit0.text(), model2.data(index).toString());
  index = model2.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(edit1.text(), model2.data(index).toString());
}

void DataWidgetMapperTest::setCurrentRowTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  TestTableModel model;
  QModelIndex index;
  QLineEdit edit0, edit1;

  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  rowChangedSpy.clear();
  editStartedSpy.clear();
  /*
   * Change current row
   */
  mapper.setCurrentRow(1);
  QCOMPARE(mapper.currentRow(), 1);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(edit0.text(), model.data(index).toString());
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(edit1.text(), model.data(index).toString());
}

void DataWidgetMapperTest::editStartDoneSignalTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  TestTableModel model;
  QModelIndex index;
  QFETCH(QWidget*, editor0);
  QFETCH(QWidget*, editor1);

  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  QSignalSpy editDoneSpy(&mapper, &DataWidgetMapper::dataEditionDone);
  QVERIFY(editDoneSpy.isValid());

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(editor0, 0);
  mapper.addMapping(editor1, 1);
  editStartedSpy.clear();
  editDoneSpy.clear();
  /*
   * Start edit with key on editor 0
   */
  QTest::keyClick(editor0, Qt::Key_A);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  // Continue typing must not emit signal again
  QTest::keyClick(editor0, Qt::Key_B);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  // Editing on other widget must also not signal anymore
  QTest::keyClick(editor1, Qt::Key_A);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  /*
   * Submit
   */
  QVERIFY(mapper.submit());
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 1);
  editDoneSpy.clear();
  // After a call of submit, start edit must be signaled again
  QTest::keyClick(editor0, Qt::Key_A);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  QTest::keyClick(editor0, Qt::Key_B);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  /*
   * Revert
   */
  mapper.revert();
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 1);
  editDoneSpy.clear();
  // After a call of revert, start edit must be signaled again
  QTest::keyClick(editor0, Qt::Key_A);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  QTest::keyClick(editor0, Qt::Key_B);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  /*
   * Submit (for nexte test)
   */
  QVERIFY(mapper.submit());
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 1);
  editDoneSpy.clear();
  /*
   * Start edit with key on editor 1
   */
  QTest::keyClick(editor1, Qt::Key_A);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  // Continue typing must not emit signal again
  QTest::keyClick(editor1, Qt::Key_B);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  // Editing on other widget must also not signal anymore
  QTest::keyClick(editor0, Qt::Key_A);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);

  delete editor0;
  delete editor1;
}

void DataWidgetMapperTest::editStartDoneSignalTest_data()
{
  QTest::addColumn<QWidget*>("editor0");
  QTest::addColumn<QWidget*>("editor1");

  QWidget *widget0, *widget1;

  widget0 = new QLineEdit;
  widget1 = new QLineEdit;
  QTest::newRow("QLineEdit") << widget0 << widget1;

}

void DataWidgetMapperTest::submitDataTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  TestTableModel model;
  QModelIndex index;
  QFETCH(QWidget*, editor0);
  QFETCH(QWidget*, editor1);

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(editor0, 0);
  mapper.addMapping(editor1, 1);

  QFAIL("Not implemented");
}

void DataWidgetMapperTest::submitDataTest_data()
{
  QTest::addColumn<QWidget*>("editor0");
  QTest::addColumn<QWidget*>("editor1");

  QWidget *widget0, *widget1;

  widget0 = new QLineEdit;
  widget1 = new QLineEdit;
  QTest::newRow("QLineEdit") << widget0 << widget1;

}

void DataWidgetMapperTest::revertDataTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  TestTableModel model;
  QModelIndex index;
  QFETCH(QWidget*, editor0);
  QFETCH(QWidget*, editor1);

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(editor0, 0);
  mapper.addMapping(editor1, 1);

  QFAIL("Not implemented");
}

void DataWidgetMapperTest::revertDataTest_data()
{
  QTest::addColumn<QWidget*>("editor0");
  QTest::addColumn<QWidget*>("editor1");

  QWidget *widget0, *widget1;

  widget0 = new QLineEdit;
  widget1 = new QLineEdit;
  QTest::newRow("QLineEdit") << widget0 << widget1;

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  DataWidgetMapperTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
