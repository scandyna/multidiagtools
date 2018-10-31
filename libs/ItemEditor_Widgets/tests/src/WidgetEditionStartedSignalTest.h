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
#ifndef MDT_TEST_MAIN
#define MDT_TEST_MAIN

#include "TestBase.h"
#include "Mdt/ItemEditor/WidgetEditionStartedSignal.h"
#include <QObject>

class EditionStartedSignalSpy : public QObject
{
 Q_OBJECT

 public:

  EditionStartedSignalSpy(const QWidget & widget);

  int editionStartedCount() const;
  void removeWidget(const QWidget & widget);
  void clear();

 public slots:

  void incrementEditionStartedCount();

 private:

  void setup(const QWidget & widget);
//   QMetaMethod findSlot(const char * name);

  int mEditionStartedCount = 0;
  Mdt::ItemEditor::WidgetEditionStartedSignal mWidgetEditionStartedSignal;
};


class WidgetEditionStartedSignalTest : public TestBase
{
  Q_OBJECT

 private slots:

  void qLineEditTest();

  void readOnlyComboBoxPopulateThenSetModelTest();
  void editableComboBoxPopulateThenSetModelTest();

  void readOnlyComboBoxSetModelThenPopulateTest();
  void editableComboBoxSetModelThenPopulateTest();

  void readOnlyComboBoxSelectRowTest();
  void editableComboBoxSelectRowTest();

  void qSpinboxTest();
  void qDoubleSpinboxTest();
};

#endif // #ifndef MDT_TEST_MAIN
