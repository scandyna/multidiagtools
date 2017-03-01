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
#include "WidgetEditablePropertyMapTest.h"
#include "Mdt/ItemEditor/WidgetEditablePropertyMap.h"
#include <QScopedPointer>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QTimeEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QFontComboBox>
#include <QCheckBox>
#include <QRadioButton>

using namespace Mdt::ItemEditor;

/*
 * Init/cleanup
 */

void WidgetEditablePropertyMapTest::initTestCase()
{
}

void WidgetEditablePropertyMapTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void WidgetEditablePropertyMapTest::editablePropertyTest()
{
  /*
   * Null property
   */
  WidgetEditableProperty np;
  QVERIFY(np.isNull());
  QVERIFY(np.editablePropertyName().isEmpty());
  QVERIFY(!np.isReversed());
  /*
   * Valid property
   */
  WidgetEditableProperty p("readOnly", true);
  QVERIFY(!p.isNull());
  QCOMPARE(p.editablePropertyName(), QByteArray("readOnly"));
  QVERIFY(p.isReversed());
}


void WidgetEditablePropertyMapTest::qtWidgetsEditablePropertyTest()
{
  WidgetEditablePropertyMap map;
  WidgetEditableProperty p;

  // QLineEdit
  p = map.getEditableProperty<QLineEdit>();
  QCOMPARE(p.editablePropertyName(), QByteArray("readOnly"));
  QVERIFY(p.isReversed());
  // QAbstractSpinBox
  p = map.getEditableProperty<QAbstractSpinBox>();
  QCOMPARE(p.editablePropertyName(), QByteArray("readOnly"));
  QVERIFY(p.isReversed());
  // QTextEdit
  p = map.getEditableProperty<QTextEdit>();
  QCOMPARE(p.editablePropertyName(), QByteArray("readOnly"));
  QVERIFY(p.isReversed());
  // QPlainTextEdit
  p = map.getEditableProperty<QPlainTextEdit>();
  QCOMPARE(p.editablePropertyName(), QByteArray("readOnly"));
  QVERIFY(p.isReversed());
  // QComboBox
  p = map.getEditableProperty<QComboBox>();
  QCOMPARE(p.editablePropertyName(), QByteArray("editable"));
  QVERIFY(!p.isReversed());
  // QCheckBox
  p = map.getEditableProperty<QCheckBox>();
  QCOMPARE(p.editablePropertyName(), QByteArray("checkable"));
  QVERIFY(!p.isReversed());
  // QRadioButton
  p = map.getEditableProperty<QRadioButton>();
  QCOMPARE(p.editablePropertyName(), QByteArray("checkable"));
  QVERIFY(!p.isReversed());
}

void WidgetEditablePropertyMapTest::qtWidgetsObjectsEditablePropertyTest()
{
  QFETCH(QWidget*, widget);
  QFETCH(QByteArray, propertyName);
  QFETCH(bool, reversed);

  WidgetEditablePropertyMap map;
  WidgetEditableProperty p;
  QScopedPointer<QWidget> widgetGuard(widget);

  /*
   * Check editable property
   */
  p = map.getEditableProperty(widget);
  QCOMPARE(p.editablePropertyName(), propertyName);
  QCOMPARE(p.isReversed(), reversed);
  /*
   * Check setting editable property to the widget
   */
  QVERIFY(map.setWidgetEditable(widget, true));
  QCOMPARE(widget->property(propertyName), QVariant(!reversed));
  QVERIFY(map.setWidgetEditable(widget, false));
  QCOMPARE(widget->property(propertyName), QVariant(reversed));
}

void WidgetEditablePropertyMapTest::qtWidgetsObjectsEditablePropertyTest_data()
{
  QTest::addColumn<QWidget*>("widget");
  QTest::addColumn<QByteArray>("propertyName");
  QTest::addColumn<bool>("reversed");

  QTest::newRow("QLineEdit") << reinterpret_cast<QWidget*>(new QLineEdit) << QByteArray("readOnly") << true;
  QTest::newRow("QSpinBox") << reinterpret_cast<QWidget*>(new QSpinBox) << QByteArray("readOnly") << true;
  QTest::newRow("QDoubleSpinBox") << reinterpret_cast<QWidget*>(new QDoubleSpinBox) << QByteArray("readOnly") << true;
  QTest::newRow("QDateTimeEdit") << reinterpret_cast<QWidget*>(new QDateTimeEdit) << QByteArray("readOnly") << true;
  QTest::newRow("QTimeEdit") << reinterpret_cast<QWidget*>(new QTimeEdit) << QByteArray("readOnly") << true;
  QTest::newRow("QDateEdit") << reinterpret_cast<QWidget*>(new QDateEdit) << QByteArray("readOnly") << true;
  QTest::newRow("QTextEdit") << reinterpret_cast<QWidget*>(new QTextEdit) << QByteArray("readOnly") << true;
  QTest::newRow("QPlainTextEdit") << reinterpret_cast<QWidget*>(new QPlainTextEdit) << QByteArray("readOnly") << true;
  QTest::newRow("QComboBox") << reinterpret_cast<QWidget*>(new QComboBox) << QByteArray("editable") << false;
  QTest::newRow("QFontComboBox") << reinterpret_cast<QWidget*>(new QFontComboBox) << QByteArray("editable") << false;
  QTest::newRow("QCheckBox") << reinterpret_cast<QWidget*>(new QCheckBox) << QByteArray("checkable") << false;
  QTest::newRow("QRadioButton") << reinterpret_cast<QWidget*>(new QRadioButton) << QByteArray("checkable") << false;
}

void WidgetEditablePropertyMapTest::setEditablePropertyTest()
{
  WidgetEditableProperty p;
  /*
   * Initial state
   */
  WidgetEditablePropertyMap map;
  QVERIFY(map.getEditableProperty<CustomTestWidget>().isNull());
  /*
   * Set for custom widget
   */
  map.setEditableProperty<CustomTestWidget>("A", true);
  p = map.getEditableProperty<CustomTestWidget>();
  QCOMPARE(p.editablePropertyName(), QByteArray("A"));
  QVERIFY(p.isReversed());
  /*
   * Update for custom widget
   */
  map.setEditableProperty<CustomTestWidget>("editable", false);
  p = map.getEditableProperty<CustomTestWidget>();
  QCOMPARE(p.editablePropertyName(), QByteArray("editable"));
  QVERIFY(!p.isReversed());
  /*
   * Check setting widget editable
   */
  CustomTestWidget widget;
  QVERIFY(map.setWidgetEditable(&widget, true));
  QVERIFY(widget.isEditable());
  QVERIFY(map.setWidgetEditable(&widget, false));
  QVERIFY(!widget.isEditable());
}

void WidgetEditablePropertyMapTest::instancesTest()
{
  WidgetEditableProperty p;
  /*
   * Set a property for instance 1
   */
  WidgetEditablePropertyMap m1;
  m1.setEditableProperty<CustomTestWidget>("A", true);
  p = m1.getEditableProperty<CustomTestWidget>();
  QCOMPARE(p.editablePropertyName(), QByteArray("A"));
  QVERIFY(p.isReversed());
  /*
   * Check that above property is alos visible on a new instance
   */
  WidgetEditablePropertyMap m2;
  p = m2.getEditableProperty<CustomTestWidget>();
  QCOMPARE(p.editablePropertyName(), QByteArray("A"));
  QVERIFY(p.isReversed());
  /*
   * Change property on second instance
   */
  m2.setEditableProperty<CustomTestWidget>("B", false);
  p = m1.getEditableProperty<CustomTestWidget>();
  QCOMPARE(p.editablePropertyName(), QByteArray("B"));
  QVERIFY(!p.isReversed());
  p = m2.getEditableProperty<CustomTestWidget>();
  QCOMPARE(p.editablePropertyName(), QByteArray("B"));
  QVERIFY(!p.isReversed());
}

void WidgetEditablePropertyMapTest::setSpinBoxReadOnlyBenchmark()
{
  QSpinBox sb;
  QBENCHMARK{
    sb.setReadOnly(true);
    QVERIFY(sb.isReadOnly());
    sb.setReadOnly(false);
    QVERIFY(!sb.isReadOnly());
  }
}

void WidgetEditablePropertyMapTest::setSpinBoxReadOnlyPropertyBenchmark()
{
  QSpinBox sb;
  QBENCHMARK{
    sb.setProperty("readOnly", true);
    QVERIFY(sb.isReadOnly());
    sb.setProperty("readOnly", false);
    QVERIFY(!sb.isReadOnly());
  }
}

void WidgetEditablePropertyMapTest::setSpinBoxEditableBenchmark()
{
  WidgetEditablePropertyMap map;
  QSpinBox sb;
  QBENCHMARK{
    map.setWidgetEditable(&sb, true);
    QVERIFY(!sb.isReadOnly());
    map.setWidgetEditable(&sb, false);
    QVERIFY(sb.isReadOnly());
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  WidgetEditablePropertyMapTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
