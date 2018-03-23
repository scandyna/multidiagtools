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
#include "SetupWidgetTest.h"
#include "Mdt/Entity/SetupWidget.h"
#include "Mdt/Entity/FieldAttributes.h"
#include <QLineEdit>
#include <QComboBox>

#include <QDebug>

using namespace Mdt::Entity;

/*
 * Tests
 */

void SetupWidgetTest::setupQLineEditTest()
{
  QLineEdit le;
  QCOMPARE(le.maxLength(), 32767);

  FieldAttributes fa(FieldMaxLength(150));
  setupWidget(&le, fa);
  QCOMPARE(le.maxLength(), 150);

  setupWidget(&le, FieldAttributes());
  QCOMPARE(le.maxLength(), 32767);
}

void SetupWidgetTest::setupQComboBoxTest()
{
  QComboBox cb;

  /*
   * Check a not editable line edit is handled
   * (cb.lineEdit() will return a nullptr)
   */
  setupWidget( &cb, FieldAttributes(FieldMaxLength(5)) );

  cb.setEditable(true);
  setupWidget( &cb, FieldAttributes(FieldMaxLength(10)) );
  QCOMPARE(cb.lineEdit()->maxLength(), 10);
}

void SetupWidgetTest::setWidgetPropertyTest()
{
  int initialDynamicPropertiesCount;

  QLineEdit le;
  initialDynamicPropertiesCount = le.dynamicPropertyNames().count();
  QVERIFY( setWidgetProperty(&le, "maxLength", 150) );
  QCOMPARE( le.maxLength(), 150 );
  QCOMPARE( le.dynamicPropertyNames().count(), initialDynamicPropertiesCount );
  QVERIFY( !setWidgetProperty(&le, "NonExistingProperty", 2) );
  QCOMPARE( le.dynamicPropertyNames().count(), initialDynamicPropertiesCount );
}

void SetupWidgetTest::setupWidgetLineditTest()
{
  QLineEdit le;
  QWidget *widget = &le;
  QCOMPARE(le.maxLength(), 32767);

  FieldAttributes fa(FieldMaxLength(150));
  setupWidget(widget, fa);
  QCOMPARE(le.maxLength(), 150);

  setupWidget(&le, FieldAttributes());
  QCOMPARE(le.maxLength(), 32767);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SetupWidgetTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
