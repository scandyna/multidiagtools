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
#include "DataWidgetMapperTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemEditor/MappedWidgetList.h"
#include "Mdt/ItemEditor/DataWidgetMapper.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QModelIndex>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QPlainTextEdit>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

// namespace ItemEditor = Mdt::ItemEditor;
// using ItemEditor::DataWidgetMapper;
// using Mdt::ItemModel::VariantTableModel;

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
  auto *w = new QWidget;

  w->setObjectName("TestWidget");

  auto *mw = new MappedWidget(w, 1, true);
  QVERIFY(mw->widget() == w);
  QCOMPARE(mw->column(), 1);
  QVERIFY(mw->hasReadOnlyProperty());

  // MappedWidget must not own the widget
  delete mw;
  QCOMPARE(w->objectName(), QString("TestWidget"));

  delete w;
}

void DataWidgetMapperTest::mappedWidgetListTest()
{
  using Mdt::ItemEditor::MappedWidget;
  using Mdt::ItemEditor::MappedWidgetList;

  auto *widget1 = new QWidget;
  auto *widget2 = new QWidget;

  /*
   * Initial state
   */
  MappedWidgetList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  /*
   * Add 1 element
   */
  list.addWidget(widget1, 1, true);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  for(const auto & mw : list){
    QVERIFY(mw.widget() == widget1);
  }
  /*
   * Add second widget
   */
  list.addWidget(widget2, 2, false);
  QCOMPARE(list.size(), 2);
  /*
   * Set enable state
   */
//   QVERIFY(widget1->isEnabled());
//   QVERIFY(widget2->isEnabled());
//   // Disable widgets
//   list.setAllWidgetsEnabled(false);
//   QVERIFY(!widget1->isEnabled());
//   QVERIFY(!widget2->isEnabled());
//   // Enable widgets
//   list.setAllWidgetsEnabled(true);
//   QVERIFY(widget1->isEnabled());
//   QVERIFY(widget2->isEnabled());
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
  delete widget2;
}

void DataWidgetMapperTest::setModelThenMapWidgetsTest()
{
  QFAIL("Not complete");
}

void DataWidgetMapperTest::mapWidgetsThenSetModelTest()
{
  QFAIL("Not complete");
}

void DataWidgetMapperTest::enableEditableFlagsTest()
{
  QFAIL("Not complete");
}

void DataWidgetMapperTest::formatRoleTest()
{
  QFAIL("Not complete");
}

void DataWidgetMapperTest::setModelTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
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
   *
   * Note: widget mapper allways goes to row -1 after a model reset.
   *       The controller then set a appropriate current row.
   */
  model.populate(3, 1);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 0);
  QCOMPARE(editStartedSpy.count(), 0);
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  // Now, change to first row
  mapper.setCurrentRow(0);
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
   *
   * Note: widget mapper allways goes to row -1 after a model reset.
   *       The controller then set a appropriate current row.
   */
  model.populate(3, 2);
  QCOMPARE(mapper.currentRow(), -1);
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
  QCOMPARE(editStartedSpy.count(), 0);
  // Go to first row
  mapper.setCurrentRow(0);
  // Check that current row changed was not signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Check that editors state was updated
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
   * Set allready populated model and map widgets once we are at first row
   *
   * Widget mapper must go to row -1 after model was set.
   * The controller will tell it which row to go later.
   */
  VariantTableModel model2;
  model2.populate(4, 2);
  mapper.setModel(&model2);
  QCOMPARE(mapper.currentRow(), -1);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Go to first row
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  // Map widgets
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
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

void DataWidgetMapperTest::setCurrentRowWidgetTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  /*
   * Setup
   */
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  /*
   * Change current row
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  /*
   * Change current row to a invalid one
   */
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  // Check editors state
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Change current row
   */
  mapper.setCurrentRow(1);
  QCOMPARE(mapper.currentRow(), 1);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("2"));
  QCOMPARE(edit1.text(), QString("B"));
}

void DataWidgetMapperTest::setCurrentRowSignalTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
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
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 1);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Change to a invalid row
   */
  mapper.setCurrentRow(-1);
  QCOMPARE(mapper.currentRow(), -1);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), -1);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Change current row
   */
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
}

void DataWidgetMapperTest::insertWidgetTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  /*
   * Setup
   */
  model.resize(0, 2);
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  // Check editors state
  QVERIFY(!edit0.isEnabled());
  QVERIFY(!edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  /*
   * Insert at end
   */
  model.appendRow();
  mapper.setCurrentRow(0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  edit0.setText("2");
  edit1.setText("B");
  QVERIFY(mapper.submit());
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.data(0, 0), QVariant("2"));
  QCOMPARE(model.data(0, 1), QVariant("B"));
  /*
   * Insert at beginning
   * Note:
   *  when a controller inserts a row, it will allways call setCurrentRow().
   *  Widget mapper must also repopulate its widgets from model,
   *  eben if current row did not change.
   */
  model.prependRow();
  mapper.setCurrentRow(0);
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QVERIFY(edit0.text().isEmpty());
  QVERIFY(edit1.text().isEmpty());
  edit0.setText("1");
  edit1.setText("A");
  QVERIFY(mapper.submit());
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.data(0, 0), QVariant("1"));
  QCOMPARE(model.data(0, 1), QVariant("A"));
  QCOMPARE(model.data(1, 0), QVariant("2"));
  QCOMPARE(model.data(1, 1), QVariant("B"));
}

void DataWidgetMapperTest::insertSignalTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  QSignalSpy rowChangedSpy(&mapper, &DataWidgetMapper::currentRowChanged);
  QVERIFY(rowChangedSpy.isValid());
  QSignalSpy editStartedSpy(&mapper, &DataWidgetMapper::dataEditionStarted);
  QVERIFY(editStartedSpy.isValid());
  /*
   * Setup
   */
  model.resize(0, 2);
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  rowChangedSpy.clear();
  editStartedSpy.clear();
  QCOMPARE(mapper.currentRow(), -1);
  /*
   * Insert at end
   */
  model.appendRow();
  mapper.setCurrentRow(0);
  // Check that current row changed was signaled
  QCOMPARE(rowChangedSpy.count(), 1);
  QCOMPARE(rowChangedSpy.takeFirst().at(0).toInt(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
  /*
   * Insert at beginning
   */
  model.prependRow();
  mapper.setCurrentRow(0);
  // Check that current row changed was not signaled
  QCOMPARE(rowChangedSpy.count(), 0);
  // Check that no edition started was signaled
  QCOMPARE(editStartedSpy.count(), 0);
}

void DataWidgetMapperTest::insertFromModelWidgetTest()
{
  DataWidgetMapper mapper;
  VariantTableModel model;
  QLineEdit edit0, edit1;
  /*
   * Setup
   */
  model.resize(1, 2);
  model.populateColumn(0, {0});
  model.populateColumn(1, {"_"});
  mapper.setModel(&model);
  mapper.addMapping(&edit0, 0);
  mapper.addMapping(&edit1, 1);
  mapper.setCurrentRow(0);
  QCOMPARE(mapper.currentRow(), 0);
  // Check editors state
  QVERIFY(edit0.isEnabled());
  QVERIFY(edit1.isEnabled());
  QCOMPARE(edit0.text(), QString("0"));
  QCOMPARE(edit1.text(), QString("_"));
  /*
   * Begin editing
   */
  edit0.setText("1");
  edit1.setText("A");
  /*
   * Insert a row before current row
   * Then, go to next row, which contains row that we are editing.
   * (Keeping current row coherent during insertion is done by the controller)
   */
  model.prependRow();
  mapper.setCurrentRow(1);
  QCOMPARE(mapper.currentRow(), 1);
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  /*
   * Submit
   */
  QVERIFY(mapper.submit());
  QCOMPARE(mapper.currentRow(), 1);
  QCOMPARE(edit0.text(), QString("1"));
  QCOMPARE(edit1.text(), QString("A"));
  QCOMPARE(model.data(1, 0), QVariant(1));
  QCOMPARE(model.data(1, 1), QVariant("A"));
}

void DataWidgetMapperTest::editStartDoneSignalTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
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
  mapper.setCurrentRow(0);
  editStartedSpy.clear();
  editDoneSpy.clear();

  /*
   * Start edit with key on editor 0
   */
  QTest::keyClick(editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  // Continue typing must not emit signal again
  QTest::keyClick(editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  // Editing on other widget must also not signal anymore
  QTest::keyClick(editor1, Qt::Key_1);
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
  QTest::keyClick(editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  QTest::keyClick(editor0, Qt::Key_1);
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
  QTest::keyClick(editor0, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  QTest::keyClick(editor0, Qt::Key_1);
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
  QTest::keyClick(editor1, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 1);
  editStartedSpy.clear();
  QCOMPARE(editDoneSpy.count(), 0);
  // Continue typing must not emit signal again
  QTest::keyClick(editor1, Qt::Key_1);
  QCOMPARE(editStartedSpy.count(), 0);
  QCOMPARE(editDoneSpy.count(), 0);
  // Editing on other widget must also not signal anymore
  QTest::keyClick(editor0, Qt::Key_1);
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

  QSpinBox *sb;
  sb = new QSpinBox;
  sb->setMaximum(10000);
  widget0 = sb;
  sb = new QSpinBox;
  sb->setMaximum(10000);
  widget1 = sb;
  QTest::newRow("QSpinBox") << widget0 << widget1;

  QComboBox *cb;
  cb = new QComboBox;
  cb->addItem("1");
  cb->addItem("11");
  cb->addItem("1111");
  cb->addItem("11111");
  widget0 = cb;
  cb = new QComboBox;
  cb->addItem("1");
  cb->addItem("11");
  cb->addItem("1111");
  cb->addItem("11111");
  widget1 = cb;
  QTest::newRow("QComboBox") << widget0 << widget1;

  widget0 = new QPlainTextEdit;
  widget1 = new QPlainTextEdit;
  QTest::newRow("QPlainTextEdit") << widget0 << widget1;

  widget0 = new QTextEdit;
  widget1 = new QTextEdit;
  QTest::newRow("QTextEdit") << widget0 << widget1;
}

void DataWidgetMapperTest::modelItemEditableFlagWidgetsWithRoTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QFETCH(QWidget*, editor0);
  QFETCH(QWidget*, editor1);

  /*
   * Setup
   */
  model.populate(1, 2);
  mapper.setModel(&model);
  mapper.addMapping(editor0, 0);
  mapper.addMapping(editor1, 1);
  mapper.setCurrentRow(0);
  /*
   * Initial state
   */
  QCOMPARE(editor0->property("readOnly"), QVariant(false));
  QCOMPARE(editor1->property("readOnly"), QVariant(false));
  /*
   * Unset editable flag
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  mapper.setCurrentRow(0);
  QCOMPARE(editor0->property("readOnly"), QVariant(true));
  QCOMPARE(editor1->property("readOnly"), QVariant(false));
  /*
   * Set editable flag (again)
   */
  model.setItemEditable(index, true);
  mapper.setCurrentRow(0);
  QCOMPARE(editor0->property("readOnly"), QVariant(false));
  QCOMPARE(editor1->property("readOnly"), QVariant(false));
  /*
   * Cleanup
   */
  delete editor0;
  delete editor1;
}

void DataWidgetMapperTest::modelItemEditableFlagWidgetsWithRoTest_data()
{
  QTest::addColumn<QWidget*>("editor0");
  QTest::addColumn<QWidget*>("editor1");

  QWidget *widget0, *widget1;

  widget0 = new QLineEdit;
  widget1 = new QLineEdit;
  QTest::newRow("QLineEdit") << widget0 << widget1;

  widget0 = new QSpinBox;
  widget1 = new QSpinBox;
  QTest::newRow("QSpinBox") << widget0 << widget1;

  widget0 = new QPlainTextEdit;
  widget1 = new QPlainTextEdit;
  QTest::newRow("QPlainTextEdit") << widget0 << widget1;

  widget0 = new QTextEdit;
  widget1 = new QTextEdit;
  QTest::newRow("QTextEdit") << widget0 << widget1;
}

void DataWidgetMapperTest::modelItemEditableFlagWidgetsWithoutRoTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QFETCH(QWidget*, editor0);
  QFETCH(QWidget*, editor1);

  /*
   * Setup
   */
  model.populate(1, 2);
  mapper.setModel(&model);
  mapper.addMapping(editor0, 0);
  mapper.addMapping(editor1, 1);
  mapper.setCurrentRow(0);
  /*
   * Initial state
   */
  QVERIFY(editor0->isEnabled());
  QVERIFY(editor1->isEnabled());
  /*
   * Unset editable flag
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setItemEditable(index, false);
  mapper.setCurrentRow(0);
  QVERIFY(!editor0->isEnabled());
  QVERIFY(editor1->isEnabled());
  /*
   * Set editable flag (again)
   */
  model.setItemEditable(index, true);
  mapper.setCurrentRow(0);
  QVERIFY(editor0->isEnabled());
  QVERIFY(editor1->isEnabled());
  /*
   * Cleanup
   */
  delete editor0;
  delete editor1;
}

void DataWidgetMapperTest::modelItemEditableFlagWidgetsWithoutRoTest_data()
{
  QTest::addColumn<QWidget*>("editor0");
  QTest::addColumn<QWidget*>("editor1");

  QWidget *widget0, *widget1;

  QComboBox *cb;
  cb = new QComboBox;
  cb->addItem("1");
  cb->addItem("2");
  cb->addItem("3");
  widget0 = cb;
  cb = new QComboBox;
  cb->addItem("1");
  cb->addItem("2");
  cb->addItem("3");
  widget1 = cb;
  QTest::newRow("QComboBox") << widget0 << widget1;
}

void DataWidgetMapperTest::setDataFromModelQLineEditTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit editor0;
  QLineEdit editor1;

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  /*
   * Update model data for mapper's current row
   */
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "ABCD"));
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "1234"));
  /*
   * Check that editors are updated
   */
  QCOMPARE(editor0.text(), QString("ABCD"));
  QCOMPARE(editor1.text(), QString("1234"));
  /*
   * Update model data for a other row
   */
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "EFGH"));
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  QVERIFY(model.setData(index, "5678"));
  /*
   * Check that editors still display the data of row 0
   */
  QCOMPARE(editor0.text(), QString("ABCD"));
  QCOMPARE(editor1.text(), QString("1234"));
}

void DataWidgetMapperTest::submitDataQLineEditTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit editor0;
  QLineEdit editor1;

  /*
   * Setup with a 2 column model
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  /*
   * Edit
   */
  editor0.setText("ABCD");
  editor1.setText("1234");
  /*
   * Submit and check
   */
  QVERIFY(mapper.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(index.data().toString(), QString("ABCD"));
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(index.data().toString(), QString("1234"));
  /*
   * Populate model with 1 column
   */
  model.populate(3, 1);
  mapper.setCurrentRow(0);
  QVERIFY(!editor1.isEnabled());
  /*
   * Edit
   */
  editor0.setText("EFGH");
  /*
   * Submit and check
   */
  QVERIFY(mapper.submit());
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(index.data().toString(), QString("EFGH"));
}

void DataWidgetMapperTest::revertDataQLineEditTest()
{
  using Mdt::ItemEditor::DataWidgetMapper;

  DataWidgetMapper mapper;
  VariantTableModel model;
  QModelIndex index;
  QLineEdit editor0;
  QLineEdit editor1;
  QString originalText0;
  QString originalText1;

  /*
   * Setup
   */
  model.populate(3, 2);
  mapper.setModel(&model);
  mapper.addMapping(&editor0, 0);
  mapper.addMapping(&editor1, 1);
  mapper.setCurrentRow(0);
  // get original texts
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  originalText0 = index.data().toString();
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  originalText1 = index.data().toString();
  /*
   * Edit
   */
  editor0.setText("ABCD");
  editor1.setText("1234");
  /*
   * Revert and check
   */
  mapper.revert();
  // Check mapped widgets
  QCOMPARE(editor0.text(), originalText0);
  QCOMPARE(editor1.text(), originalText1);
  // Check model
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(index.data().toString(), originalText0);
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(index.data().toString(), originalText1);
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
