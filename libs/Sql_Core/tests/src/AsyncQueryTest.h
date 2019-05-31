/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_ASYNC_QUERY_TEST_H
#define MDT_SQL_ASYNC_QUERY_TEST_H

#include "TestBase.h"
#include "AsyncTestQueryReceiver.h"

class AsyncQueryTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();
  void cleanup();

  void connectionIidTest();

  void queryIidTest();

  void insertAsync();
  void insertSync();
  void insertError();
  void insertMultipleQueries();

  void updateAsyncTest();
  void updateSyncTest();
  void updateMultipleQueriesTest();

  void deleteAsyncTest();
  void deleteSyncTest();
  void deleteMultipleQueriesTest();

  void selectAsyncTest();
  void selectAsyncSingleRecordTest();
  void selectSyncTest();
  void selectMultipleQueriesTest();
};

#endif // #ifndef MDT_SQL_ASYNC_QUERY_TEST_H
