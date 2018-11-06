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
#ifndef CACHED_TABLE_MODEL_TEST_H
#define CACHED_TABLE_MODEL_TEST_H

#include "TestBase.h"

class CachedTableModelTest : public QObject
{
 Q_OBJECT

 private slots:

  void initTestCase();

  void readOnlyRowCountLimitTest();
  void editableRowCountLimitTest();
  void readOnlyRowCountLimitSignalTest();
  void editableRowCountLimitSignalTest();

  void readOnlyHorizontalHeaderTest();
  void editableHorizontalHeaderTest();

  void readOnlyFlagsTest();
  void editableFlagsTest();

  void readOnlyDataTest();
  void editableDataTest();

  void readOnlyFromBackendInsertRecordsTest();
  void editableFromBackendInsertRecordsTest();
  void readOnlyFromBackendInsertRecordsSignalTest();
  void editableFromBackendInsertRecordsSignalTest();

  void readOnlyFetchAllTest();
  void editableFetchAllTest();
  void readOnlyFetchAllSignalTest();
  void editableFetchAllSignalTest();

  void readOnlyFromBackendSetDataTest();
  void editableFromBackendSetDataTest();
  void readOnlyFromBackendSetDataSignalTest();
  void editableFromBackendSetDataSignalTest();

  void setDataTest();
  void setDataSignalTest();

  void setDataThenFromBackendInsertRecordsTest();
  void setDataThenFromBackendInsertRecordsSignalTest();

  void setDataThenSubmitTest();
  void setDataThenSubmitSignalTest();

  void setDataThenSubmitAndFromBackendInsertRecordsTest();
  void setDataThenSubmitAndFromBackendInsertRecordsSignalTest();

  void insertRowsTest();
  void insertRowsSignalTest();

  void insertRowsThenSetDataThenSubmitTest();
  void insertRowsThenSetDataThenSubmitSignalTest();
};

#endif // #ifndef CACHED_TABLE_MODEL_TEST_H
