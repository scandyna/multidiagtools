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
#include "WidgetEditionStartedSignalTest.h"
#include "Mdt/TestLib/Widgets.h"
#include <QMetaObject>
#include <QMetaMethod>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

// #include <QDebug>

using namespace Mdt::ItemEditor;
using namespace Mdt::TestLib;

/*
 * Signal spy
 */

EditionStartedSignalSpy::EditionStartedSignalSpy(const QWidget& widget)
 : QObject()
{
  connect(&mWidgetEditionStartedSignal, &WidgetEditionStartedSignal::editionStarted, this, &EditionStartedSignalSpy::incrementEditionStartedCount);
  setup(widget);
}

int EditionStartedSignalSpy::editionStartedCount() const
{
  return mEditionStartedCount;
}

void EditionStartedSignalSpy::removeWidget(const QWidget& widget)
{
  mWidgetEditionStartedSignal.removeWidget(&widget);
}

void EditionStartedSignalSpy::clear()
{
  mEditionStartedCount = 0;
}

void EditionStartedSignalSpy::incrementEditionStartedCount()
{
  ++mEditionStartedCount;
}

void EditionStartedSignalSpy::setup(const QWidget& widget)
{
  mWidgetEditionStartedSignal.addWidget(&widget);
//   WidgetEditionStartedSignal es;
// 
//   QMetaMethod signalMethod = es.getEditionStartedSignal(&widget);
//   Q_ASSERT(signalMethod.isValid());
// 
//   QMetaMethod slotMethod = findSlot("incrementEditionStartedCount()");
//   Q_ASSERT(slotMethod.isValid());
// 
//   connect(&widget, signalMethod, this, slotMethod);
}

// QMetaMethod EditionStartedSignalSpy::findSlot(const char* name)
// {
//   QMetaMethod slotMethod;
// 
//   int slotIndex = staticMetaObject.indexOfSlot(name);
//   Q_ASSERT(slotIndex >= 0);
//   slotMethod = staticMetaObject.method(slotIndex);
// 
//   return slotMethod;
// }

/*
 * Helpers
 */

void editWidget(QWidget & widget, const QString & str)
{
  QTest::keyClicks(&widget, str);
}

/*
 * Tests
 */

void WidgetEditionStartedSignalTest::qLineEditTest()
{
  QLineEdit edit;
  EditionStartedSignalSpy editionStartedSpy(edit);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  edit.setText("A");
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  editWidget(edit, "B");
  QCOMPARE(editionStartedSpy.editionStartedCount(), 1);
  editionStartedSpy.clear();

  editionStartedSpy.removeWidget(edit);

  editWidget(edit, "B");
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);
}

void populateThenSetModelTest(QComboBox & cb)
{
  QStringListModel model({"A","B","C"});

  EditionStartedSignalSpy editionStartedSpy(cb);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  cb.setModel(&model);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);
}

void WidgetEditionStartedSignalTest::readOnlyComboBoxPopulateThenSetModelTest()
{
  QComboBox cb;
  cb.setEditable(false);

  populateThenSetModelTest(cb);
}

void WidgetEditionStartedSignalTest::editableComboBoxPopulateThenSetModelTest()
{
  QComboBox cb;
  cb.setEditable(true);

  populateThenSetModelTest(cb);
}

void setModelThenPopulateTest(QComboBox & cb)
{
  QStringListModel model;

  EditionStartedSignalSpy editionStartedSpy(cb);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  cb.setModel(&model);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  model.setStringList({"A","B","C"});
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);
}

void WidgetEditionStartedSignalTest::readOnlyComboBoxSetModelThenPopulateTest()
{
  QComboBox cb;
  cb.setEditable(false);

  setModelThenPopulateTest(cb);
}

void WidgetEditionStartedSignalTest::editableComboBoxSetModelThenPopulateTest()
{
  QComboBox cb;
  cb.setEditable(true);

  setModelThenPopulateTest(cb);
}

void selectRowTest(QComboBox & cb)
{
  QStringListModel model({"A","B","C"});

  cb.setModel(&model);
  EditionStartedSignalSpy editionStartedSpy(cb);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  cb.setCurrentIndex(0);
  QCOMPARE(cb.currentIndex(), 0);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  mouseSelectRow(cb, 1);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 1);
  QCOMPARE(cb.currentIndex(), 1);
  editionStartedSpy.clear();

  cb.setCurrentIndex(0);
  QCOMPARE(cb.currentIndex(), 0);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  mouseSelectRow(cb, 2);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 1);
  QCOMPARE(cb.currentIndex(), 2);
  editionStartedSpy.clear();

  cb.clearEditText();
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  QTest::keyClick(&cb, 'A');
  QCOMPARE(editionStartedSpy.editionStartedCount(), 1);
  editionStartedSpy.clear();

  QTest::keyClick(&cb, Qt::Key_Enter);
  QCOMPARE(cb.currentIndex(), 0);
  editionStartedSpy.clear();

  editionStartedSpy.removeWidget(cb);

  mouseSelectRow(cb, 2);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);
  QCOMPARE(cb.currentIndex(), 2);

  cb.clearEditText();
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  // Without waiting, test fails with non editable combobox
  QTest::qWait(QApplication::keyboardInputInterval() + 30);

  QTest::keyClick(&cb, 'A');
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  QTest::keyClick(&cb, Qt::Key_Enter);
  QCOMPARE(cb.currentIndex(), 0);
}

void WidgetEditionStartedSignalTest::readOnlyComboBoxSelectRowTest()
{
  QComboBox cb;
  cb.setEditable(false);
  cb.setInsertPolicy(QComboBox::NoInsert);

  selectRowTest(cb);
}

void WidgetEditionStartedSignalTest::editableComboBoxSelectRowTest()
{
  QComboBox cb;
  cb.setEditable(true);

  selectRowTest(cb);
}

void spinboxTest(QAbstractSpinBox & sb)
{
  EditionStartedSignalSpy editionStartedSpy(sb);
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  QTest::keyClicks(&sb, "1");
  QCOMPARE(editionStartedSpy.editionStartedCount(), 1);
  QCOMPARE(sb.text(), QString("1"));
  editionStartedSpy.clear();

  sb.clear();
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);

  editionStartedSpy.removeWidget(sb);

  QTest::keyClicks(&sb, "2");
  QCOMPARE(editionStartedSpy.editionStartedCount(), 0);
  QCOMPARE(sb.text(), QString("2"));
}

void WidgetEditionStartedSignalTest::qSpinboxTest()
{
  QSpinBox sb;

  spinboxTest(sb);
}

void WidgetEditionStartedSignalTest::qDoubleSpinboxTest()
{
  QDoubleSpinBox sb;

  spinboxTest(sb);
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  WidgetEditionStartedSignalTest test;

  return QTest::qExec(&test, argc, argv);
}
