/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "MappedWidgetTest.h"
#include "Mdt/ItemEditor/MappedWidget.h"
#include "Mdt/ItemEditor/MappedWidgetGeneric.h"
#include "Mdt/ItemEditor/MappedWidgetQLineEdit.h"
#include "Mdt/ItemEditor/MappedWidgetQComboBox.h"
#include "Mdt/ItemEditor/MappedWidgetRelationalQComboBox.h"
#include "Mdt/ItemEditor/MappedWidgetList.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/TestLib/Widgets.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <memory>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;
using namespace Mdt::TestLib;

/*
 * EditionStartedSignalSpy
 */

EditionStartedSignalSpy::EditionStartedSignalSpy(MappedWidget*const mappedWidget)
 : QObject(),
   mMappedWidget(mappedWidget)
{
  Q_ASSERT(mMappedWidget != nullptr);

  connect(mMappedWidget, &MappedWidget::editionStarted, this, &EditionStartedSignalSpy::incrementEditionStartedCount);
}

int EditionStartedSignalSpy::editionStartedCount() const
{
  return mEditionStartedCount;
}

void EditionStartedSignalSpy::clear()
{
  mEditionStartedCount = 0;
}

void EditionStartedSignalSpy::incrementEditionStartedCount()
{
  ++mEditionStartedCount;
}

/*
 * Tests
 */

void MappedWidgetTest::constructDeleteTest()
{
  auto *w = new QWidget;

  w->setObjectName("TestWidget");

  auto *mw = new MappedWidgetGeneric(w, 1);
  QVERIFY(mw->widget() == w);
  QCOMPARE(mw->column(), 1);

  // MappedWidget must not own the widget
  delete mw;
  QCOMPARE(w->objectName(), QString("TestWidget"));

  delete w;
}

void MappedWidgetTest::widgetValueQLineEditTest()
{
  QLineEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetQLineEdit(&edit, 0) );
  QCOMPARE(edit.text(), QString(""));

  w->setWidgetValue("A");
  QCOMPARE(edit.text(), QString("A"));
  QCOMPARE(w->widgetValue(), QVariant("A"));
}

void MappedWidgetTest::widgetValueQLineEditGenericTest()
{
  QLineEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );

  QCOMPARE(edit.text(), QString(""));
  w->setWidgetValue("A");
  QCOMPARE(edit.text(), QString("A"));
  QCOMPARE(w->widgetValue(), QVariant("A"));
}

void MappedWidgetTest::widgetValueQComboBoxTest()
{
  QComboBox cb;
  cb.addItems({"A","B","C"});
  std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );
  QCOMPARE(cb.currentText(), QString("A"));

  w->setWidgetValue("B");
  QCOMPARE(cb.currentText(), QString("B"));
  QCOMPARE(w->widgetValue(), QVariant("B"));
}

void MappedWidgetTest::widgetValueRelationalQComboBoxTest()
{
  VariantTableModel typeModel;
  typeModel.resize(2, 2);
  QVERIFY(setModelData(typeModel, 0, 0, 1));
  QVERIFY(setModelData(typeModel, 0, 1, "Home"));
  QVERIFY(setModelData(typeModel, 1, 0, 2));
  QVERIFY(setModelData(typeModel, 1, 1, "Work"));

  QComboBox cb;
  cb.setModel(&typeModel);
  cb.setModelColumn(1);

  std::unique_ptr<MappedWidget> w( new MappedWidgetRelationalQComboBox(&cb, 3, 0) );
  QCOMPARE(cb.currentText(), QString("Home"));

  w->setWidgetValue(2);
  QCOMPARE(cb.currentText(), QString("Work"));
  QCOMPARE(w->widgetValue(), QVariant(2));

  w->setWidgetValue(1);
  QCOMPARE(cb.currentText(), QString("Home"));
  QCOMPARE(w->widgetValue(), QVariant(1));
}

void MappedWidgetTest::widgetValueQSpinBoxTest()
{
  QSpinBox sb;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&sb, 0) );

  w->setWidgetValue(5);
  QCOMPARE(sb.value(), 5);
  QCOMPARE(w->widgetValue(), QVariant(5));
}

void MappedWidgetTest::widgetValueQDoubleSpinBoxTest()
{
  QDoubleSpinBox sb;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&sb, 0) );

  w->setWidgetValue(3.2);
  QCOMPARE(sb.value(), 3.2);
  QCOMPARE(w->widgetValue(), QVariant(3.2));
}

void MappedWidgetTest::widgetValueQPlainTextEditTest()
{
  QPlainTextEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );
  QCOMPARE(edit.toPlainText(), QString(""));

  w->setWidgetValue("A");
  QCOMPARE(edit.toPlainText(), QString("A"));
  QCOMPARE(w->widgetValue(), QVariant("A"));
}

void MappedWidgetTest::widgetValueQTextEditTest()
{
  QTextEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );
  QCOMPARE(edit.toPlainText(), QString(""));

  w->setWidgetValue("A");
  QCOMPARE(edit.toPlainText(), QString("A"));
  QCOMPARE(w->widgetValue(), QVariant("A"));
}

void MappedWidgetTest::widgetEditablePropertyTest()
{
  QLineEdit lineEdit;
  MappedWidgetGeneric mappedLineEdit(&lineEdit, 0);
  QVERIFY(!mappedLineEdit.widgetEditableProperty().isNull());
  QCOMPARE(mappedLineEdit.widgetEditableProperty().editablePropertyName(), QByteArray("readOnly"));
  QVERIFY(mappedLineEdit.widgetEditableProperty().isReversed());

  QComboBox comboBox;
  MappedWidgetGeneric mappedComboBox(&comboBox, 0);
  QVERIFY(!mappedComboBox.widgetEditableProperty().isNull());
  QCOMPARE(mappedComboBox.widgetEditableProperty().editablePropertyName(), QByteArray("editable"));
  QVERIFY(!mappedComboBox.widgetEditableProperty().isReversed());

  QCheckBox checkBox;
  MappedWidgetGeneric mappedCheckBox(&checkBox, 0);
  QVERIFY(!mappedCheckBox.widgetEditableProperty().isNull());
  QCOMPARE(mappedCheckBox.widgetEditableProperty().editablePropertyName(), QByteArray("checkable"));
  QVERIFY(!mappedCheckBox.widgetEditableProperty().isReversed());

  QWidget widget;
  MappedWidgetGeneric mappedWidget(&widget, 0);
  QVERIFY(mappedWidget.widgetEditableProperty().isNull());
}

void MappedWidgetTest::widgetEditableUnhandledWidgetTest()
{
  QWidget widget;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&widget, 0) );

  QVERIFY(widget.isEnabled());
  w->setWidgetEditable(false);
  QVERIFY(!widget.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(widget.isEnabled());

  w->setWidgetEnabled(false);
  QVERIFY(!widget.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!widget.isEnabled());
  w->setWidgetEnabled(true);
  QVERIFY(widget.isEnabled());
}

void MappedWidgetTest::widgetEditableQLineEditGenericTest()
{
  QLineEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );

  w->setWidgetEditable(false);
  QVERIFY(edit.isReadOnly());
  QVERIFY(edit.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!edit.isReadOnly());
  QVERIFY(edit.isEnabled());
}

void MappedWidgetTest::widgetEditableQLineEditTest()
{
  QLineEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetQLineEdit(&edit, 0) );

  w->setWidgetEditable(false);
  QVERIFY(edit.isReadOnly());
  QVERIFY(edit.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!edit.isReadOnly());
  QVERIFY(edit.isEnabled());
}

// void MappedWidgetTest::widgetEditableQComboBoxTest()
// {
//   QComboBox cb;
//   cb.addItems({"A","B","C"});
//   std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );
// 
//   w->setWidgetEditable(false);
//   QVERIFY(!cb.isEditable());
//   QVERIFY(cb.isEnabled());
//   w->setWidgetEditable(true);
//   QVERIFY(cb.isEditable());
//   QVERIFY(cb.isEnabled());
// }

void widgetEditableQComboBoxTest(QComboBox & cb)
{
  std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );

  QVERIFY(cb.isEnabled());
  w->setWidgetEditable(false);
  QVERIFY(!cb.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(cb.isEnabled());

  w->setWidgetEnabled(false);
  QVERIFY(!cb.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!cb.isEnabled());
  w->setWidgetEnabled(true);
  QVERIFY(cb.isEnabled());
}

void MappedWidgetTest::widgetEditableReadOnlyQComboBoxTest()
{
  QComboBox cb;
  cb.addItems({"A","B","C"});
  cb.setEditable(false);

  widgetEditableQComboBoxTest(cb);
  
//   std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );
// 
// //   QVERIFY(cb.view() != nullptr);
// //   cb.view()->setEnabled(false);
//   
//   cb.setEnabled(false);
//   
//   cb.show();
//   while(cb.isVisible()){
//     QTest::qWait(500);
//   }
//   QFAIL("Not complete");
}

void MappedWidgetTest::widgetEditableEditableQComboBoxTest()
{
  QComboBox cb;
  cb.addItems({"A","B","C"});
  cb.setEditable(true);

  widgetEditableQComboBoxTest(cb);
  
//   std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );
// 
// //   cb.setEnabled(false);
// //   QVERIFY(cb.lineEdit() != nullptr);
// //   cb.lineEdit()->setEnabled(true);
//   
//   QVERIFY(cb.view() != nullptr);
// //   cb.view()->setSelectionMode(QAbstractItemView::NoSelection);
//   cb.view()->setEnabled(false);
//   
//   cb.show();
//   while(cb.isVisible()){
//     QTest::qWait(500);
//   }
// 
//   QFAIL("Not complete");
}

void MappedWidgetTest::widgetEditableQSpinBoxTest()
{
  QSpinBox spinBox;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&spinBox, 0) );

  w->setWidgetEditable(false);
  QVERIFY(spinBox.isReadOnly());
  QVERIFY(spinBox.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!spinBox.isReadOnly());
  QVERIFY(spinBox.isEnabled());
}

void MappedWidgetTest::widgetEditableQDoubleSpinBoxTest()
{
  QDoubleSpinBox spinBox;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&spinBox, 0) );

  w->setWidgetEditable(false);
  QVERIFY(spinBox.isReadOnly());
  QVERIFY(spinBox.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!spinBox.isReadOnly());
  QVERIFY(spinBox.isEnabled());
}

void MappedWidgetTest::widgetEditableQPlainTextEditTest()
{
  QPlainTextEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );

  w->setWidgetEditable(false);
  QVERIFY(edit.isReadOnly());
  QVERIFY(edit.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!edit.isReadOnly());
  QVERIFY(edit.isEnabled());
}

void MappedWidgetTest::widgetEditableQTextEditTest()
{
  QTextEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );

  w->setWidgetEditable(false);
  QVERIFY(edit.isReadOnly());
  QVERIFY(edit.isEnabled());
  w->setWidgetEditable(true);
  QVERIFY(!edit.isReadOnly());
  QVERIFY(edit.isEnabled());
}

void MappedWidgetTest::editionStartedQLineEditTest()
{
  QLineEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetQLineEdit(&edit, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  w->setWidgetValue("A");
  QCOMPARE(spy.editionStartedCount(), 0);

  QTest::keyClicks(&edit, "B");
  QCOMPARE(spy.editionStartedCount(), 1);
  spy.clear();

  w.reset(nullptr);
  QTest::keyClicks(&edit, "C");
  QCOMPARE(spy.editionStartedCount(), 0);
}

void MappedWidgetTest::editionStartedQLineEditGenericTest()
{
  QLineEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  QTest::keyClicks(&edit, "B");
  QCOMPARE(spy.editionStartedCount(), 1);
  spy.clear();

  w.reset(nullptr);
  QTest::keyClicks(&edit, "C");
  QCOMPARE(spy.editionStartedCount(), 0);
}

void editionStartedQComboBoxTest(QComboBox & cb)
{
  QStringListModel model({"A","B","C"});
  cb.setModel(&model);

  std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  w->setWidgetValue("A");
  QCOMPARE(cb.currentIndex(), 0);
  QCOMPARE(spy.editionStartedCount(), 0);

  mouseSelectRow(cb, 1);
  QCOMPARE(spy.editionStartedCount(), 1);
  QCOMPARE(cb.currentIndex(), 1);
  spy.clear();

  cb.setCurrentIndex(0);
  QCOMPARE(cb.currentIndex(), 0);
  QCOMPARE(spy.editionStartedCount(), 0);

  mouseSelectRow(cb, 2);
  QCOMPARE(spy.editionStartedCount(), 1);
  QCOMPARE(cb.currentIndex(), 2);
  spy.clear();

  cb.clearEditText();
  QCOMPARE(spy.editionStartedCount(), 0);

  QTest::keyClick(&cb, 'A');
  QCOMPARE(spy.editionStartedCount(), 1);
  spy.clear();

  QTest::keyClick(&cb, Qt::Key_Enter);
  QCOMPARE(cb.currentIndex(), 0);
  spy.clear();

  w.reset(nullptr);

  mouseSelectRow(cb, 2);
  QCOMPARE(spy.editionStartedCount(), 0);
  QCOMPARE(cb.currentIndex(), 2);

  cb.clearEditText();
  QCOMPARE(spy.editionStartedCount(), 0);

  // Without waiting, test fails with non editable combobox
  QTest::qWait(QApplication::keyboardInputInterval() + 30);

  QTest::keyClick(&cb, 'A');
  QCOMPARE(spy.editionStartedCount(), 0);

  QTest::keyClick(&cb, Qt::Key_Enter);
  QCOMPARE(cb.currentIndex(), 0);
}

void MappedWidgetTest::editionStartedReadOnlyQComboBoxTest()
{
  QComboBox cb;
  cb.setEditable(false);
  cb.setInsertPolicy(QComboBox::NoInsert);

  editionStartedQComboBoxTest(cb);
}

void MappedWidgetTest::editionStartedEditableQComboBoxTest()
{
  QComboBox cb;
  cb.setEditable(true);

  editionStartedQComboBoxTest(cb);
}

void MappedWidgetTest::editionStartedQSpinBoxTest()
{
  QSpinBox sb;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&sb, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  QTest::keyClicks(&sb, "1");
  QCOMPARE(spy.editionStartedCount(), 1);
  spy.clear();

  w.reset(nullptr);
  QTest::keyClicks(&sb, "2");
  QCOMPARE(spy.editionStartedCount(), 0);
}

void MappedWidgetTest::editionStartedQDoubleSpinBoxTest()
{
  QDoubleSpinBox sb;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&sb, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  QTest::keyClicks(&sb, "1");
  QCOMPARE(spy.editionStartedCount(), 1);
  spy.clear();

  w.reset(nullptr);
  QTest::keyClicks(&sb, "2");
  QCOMPARE(spy.editionStartedCount(), 0);
}

void MappedWidgetTest::editionStartedQPlainTextEditTest()
{
  QPlainTextEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  QTest::keyClicks(&edit, "B");
  QCOMPARE(spy.editionStartedCount(), 1);
  spy.clear();

  w.reset(nullptr);
  QTest::keyClicks(&edit, "C");
  QCOMPARE(spy.editionStartedCount(), 0);
}

void MappedWidgetTest::editionStartedQTextEditTest()
{
  QTextEdit edit;
  std::unique_ptr<MappedWidget> w( new MappedWidgetGeneric(&edit, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  QTest::keyClicks(&edit, "B");
  QCOMPARE(spy.editionStartedCount(), 1);
  spy.clear();

  w.reset(nullptr);
  QTest::keyClicks(&edit, "C");
  QCOMPARE(spy.editionStartedCount(), 0);
}

void populateThenSetModelTest(QComboBox & cb)
{
  QStringListModel model({"A","B","C"});

  std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  cb.setModel(&model);
  QCOMPARE(spy.editionStartedCount(), 0);
}

void MappedWidgetTest::readOnlyComboBoxPopulateThenSetModelTest()
{
  QComboBox cb;
  cb.setEditable(false);

  populateThenSetModelTest(cb);
}

void MappedWidgetTest::editableComboBoxPopulateThenSetModelTest()
{
  QComboBox cb;
  cb.setEditable(true);

  populateThenSetModelTest(cb);
}

void setModelThenPopulateTest(QComboBox & cb)
{
  QStringListModel model;

  std::unique_ptr<MappedWidget> w( new MappedWidgetQComboBox(&cb, 0) );
  EditionStartedSignalSpy spy(w.get());

  QCOMPARE(spy.editionStartedCount(), 0);
  cb.setModel(&model);
  QCOMPARE(spy.editionStartedCount(), 0);

  model.setStringList({"A","B","C"});
  QCOMPARE(spy.editionStartedCount(), 0);
}

void MappedWidgetTest::readOnlyComboBoxSetModelThenPopulateTest()
{
  QComboBox cb;
  cb.setEditable(false);

  setModelThenPopulateTest(cb);
}

void MappedWidgetTest::editableComboBoxSetModelThenPopulateTest()
{
  QComboBox cb;
  cb.setEditable(true);

  setModelThenPopulateTest(cb);
}

void MappedWidgetTest::listAddGetRemoveTest()
{
  auto *widget1 = new QWidget;
  auto *widget2 = new QWidget;
  QScopedPointer<QWidget> widget1Guard(widget1);
  QScopedPointer<QWidget> widget2Guard(widget2);
  /*
   * Initial state
   */
  MappedWidgetList list;
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
  QCOMPARE(list.getIndexForWidget(widget1), -1);
  QCOMPARE(list.getIndexForColumn(0), -1);
  /*
   * Map widget1 to column 1
   */
  list.addMapping(widget1, 1);
  QCOMPARE(list.size(), 1);
  QVERIFY(!list.isEmpty());
  QCOMPARE(list.getIndexForWidget(widget1), 0);
  QCOMPARE(list.getIndexForColumn(1), 0);
  for(const auto & mw : list){
    QVERIFY(mw->widget() == widget1);
  }
  /*
   * Remove first mapping
   */
  list.removeMappingAt(0);
  QCOMPARE(list.size(), 0);
  /*
   * Map widget1
   */
  list.addMapping(widget1, 2);
  QCOMPARE(list.size(), 1);
  QCOMPARE((*list.begin())->widget(), widget1);
  QCOMPARE((*list.begin())->column(), 2);
  QCOMPARE(list.getIndexForWidget(widget1), 0);
  QCOMPARE(list.getIndexForColumn(2), 0);
  /*
   * Add another mapping
   */
  list.addMapping(widget2, 1);
  QCOMPARE(list.size(), 2);
  QCOMPARE(list.getIndexForWidget(widget2), 1);
  QCOMPARE(list.getIndexForColumn(1), 1);
  QCOMPARE(list.widgetAt(1), widget2);
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.size(), 0);
  QVERIFY(list.isEmpty());
}

void MappedWidgetTest::listAddMappingKnownEditorWidgetsTest()
{
  MappedWidgetList list;

  QLineEdit lineEdit;
  list.addMapping(&lineEdit, 0);
  QCOMPARE(list.size(), 1);
  list.mappedWidgetAt(0)->setWidgetValue("leA");
  QCOMPARE(lineEdit.text(), QString("leA"));
  QVERIFY(!lineEdit.isReadOnly());
  list.mappedWidgetAt(0)->setWidgetEditable(false);
  QVERIFY(lineEdit.isReadOnly());

  QComboBox comboBox;
  QStringListModel comboBoxModel({"cbA","cbB","cbC"});
  comboBox.setModel(&comboBoxModel);
  list.addMapping(&comboBox, 1);
  QCOMPARE(list.size(), 2);
  list.mappedWidgetAt(1)->setWidgetValue("cbB");
  QCOMPARE(comboBox.currentText(), QString("cbB"));
  QVERIFY(!comboBox.isEditable());
  QVERIFY(comboBox.isEnabled());
  list.mappedWidgetAt(1)->setWidgetEditable(false);
  QVERIFY(!comboBox.isEditable());
  QVERIFY(!comboBox.isEnabled());
  list.mappedWidgetAt(1)->setWidgetEditable(true);
  QVERIFY(!comboBox.isEditable());
  QVERIFY(comboBox.isEnabled());

  QSpinBox spinBox;
  list.addMapping(&spinBox, 2);
  QCOMPARE(list.size(), 3);
  list.mappedWidgetAt(2)->setWidgetValue(12);
  QCOMPARE(spinBox.value(), 12);
  QVERIFY(!spinBox.isReadOnly());
  list.mappedWidgetAt(2)->setWidgetEditable(false);
  QVERIFY(spinBox.isReadOnly());

  QDoubleSpinBox doubleSpinBox;
  list.addMapping(&doubleSpinBox, 3);
  QCOMPARE(list.size(), 4);
  list.mappedWidgetAt(3)->setWidgetValue(15.6);
  QCOMPARE(doubleSpinBox.value(), 15.6);
  QVERIFY(!doubleSpinBox.isReadOnly());
  list.mappedWidgetAt(3)->setWidgetEditable(false);
  QVERIFY(doubleSpinBox.isReadOnly());

  QPlainTextEdit plainTextEdit;
  list.addMapping(&plainTextEdit, 4);
  QCOMPARE(list.size(), 5);
  list.mappedWidgetAt(4)->setWidgetValue("pteA");
  QCOMPARE(plainTextEdit.toPlainText(), QString("pteA"));
  QVERIFY(!plainTextEdit.isReadOnly());
  list.mappedWidgetAt(4)->setWidgetEditable(false);
  QVERIFY(plainTextEdit.isReadOnly());

  QTextEdit textEdit;
  list.addMapping(&textEdit, 5);
  QCOMPARE(list.size(), 6);
  list.mappedWidgetAt(5)->setWidgetValue("teA");
  QCOMPARE(textEdit.toPlainText(), QString("teA"));
  QVERIFY(!textEdit.isReadOnly());
  list.mappedWidgetAt(5)->setWidgetEditable(false);
  QVERIFY(textEdit.isReadOnly());

  VariantTableModel typeModel;
  typeModel.resize(2, 2);
  QVERIFY(setModelData(typeModel, 0, 0, 1));
  QVERIFY(setModelData(typeModel, 0, 1, "Home"));
  QVERIFY(setModelData(typeModel, 1, 0, 2));
  QVERIFY(setModelData(typeModel, 1, 1, "Work"));

  QComboBox typeComboBox;
  typeComboBox.setModel(&typeModel);
  typeComboBox.setModelColumn(1);
  list.addRelationalMapping(&typeComboBox, 6, 0);
  QCOMPARE(list.size(), 7);
  list.mappedWidgetAt(6)->setWidgetValue(2);
  QCOMPARE(typeComboBox.currentText(), QString("Work"));
  QVERIFY(!typeComboBox.isEditable());
  QVERIFY(typeComboBox.isEnabled());
  list.mappedWidgetAt(6)->setWidgetEditable(false);
  QVERIFY(!typeComboBox.isEditable());
  QVERIFY(!typeComboBox.isEnabled());
  list.mappedWidgetAt(6)->setWidgetEditable(true);
  QVERIFY(!typeComboBox.isEditable());
  QVERIFY(typeComboBox.isEnabled());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  MappedWidgetTest test;

  return QTest::qExec(&test, argc, argv);
}
