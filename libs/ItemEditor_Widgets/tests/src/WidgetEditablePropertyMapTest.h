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
#ifndef MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_TEST_H
#define MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_TEST_H

#include "TestBase.h"
#include <QWidget>

class CustomTestWidget : public QWidget
{
 Q_OBJECT

 public:

  Q_PROPERTY(bool editable READ isEditable WRITE setEditable)

  void setEditable(bool e)
  {
    mEditable = e;
  }

  bool isEditable() const
  {
    return mEditable;
  }

 private:

  bool mEditable = true;
};

class WidgetEditablePropertyMapTest : public TestBase
{
  Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void editablePropertyTest();

  void qtWidgetsEditablePropertyTest();
  void qtWidgetsObjectsEditablePropertyTest();
  void qtWidgetsObjectsEditablePropertyTest_data();
  
  void setEditablePropertyTest();
  void instancesTest();

  void setSpinBoxReadOnlyBenchmark();
  void setSpinBoxReadOnlyPropertyBenchmark();
  void setSpinBoxEditableBenchmark();
};

#endif // #ifndef MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_TEST_H
