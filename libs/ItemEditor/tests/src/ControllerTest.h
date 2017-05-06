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
#ifndef MDT_ITEM_EDITOR_CONTROLLER_TEST_H
#define MDT_ITEM_EDITOR_CONTROLLER_TEST_H

#include "TestBase.h"

class ControllerTest : public TestBase
{
  Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void statePermissionTest();
  void basicStateTest();
  void setModelTest();
  void setModelSignalTest();
  void modelSizeChangedTest();
  void modelSizeChangedSignalsTest();
  void entityNameTest();
  void addRemoveProxyModelWithoutSourceModelTest();
  void addRemoveProxyModelTest();
  void addRemoveProxyModelSignalTest();
  void filterCheckModelTest();
  void sortEnableDisableSetTest();
//   void filterCheckModelSignalTest();
  void primaryKeyTest();
  void foreignKeyTest();
  void currentRowTest();
  void currentRowSignalsTest();
  void currentDataTest();
  void currentDataFilterTest();
  void currentDataSortTest();
  void selectedRowsTest();
  void navigationSlotsTest();
  void insertTest();
  void insertRevertTest();
  void insertFromModelTest();
  void insertFromModelAndRemoveTest();
  void removeTest();
  void removeMultipleRowsTest();
  void removeMultipleRowsTest_data();
  void removeFromModelTest();
};

#endif // #ifndef MDT_ITEM_EDITOR_CONTROLLER_TEST_H
