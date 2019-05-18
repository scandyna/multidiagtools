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
#ifndef MDT_SQL_ASYNC_QUERY_BENCHMARK_H
#define MDT_SQL_ASYNC_QUERY_BENCHMARK_H

#include "TestBase.h"
#include "AsyncTestQueryReceiver.h"
#include "Mdt/QueryExpression/SelectStatement.h"

class AsyncQueryBenchmark : public TestBase
{
 Q_OBJECT

 signals:

  void testSignal();

 private slots:

  void initTestCase();
  void cleanupTestCase();
  void cleanup();

  void variantRecordFromSqlRecordBenchmark();
  void isSignalConnectedBenchmark();

  void selectQSqlQueryBenchmark();
  void selectQSqlQueryBenchmark_data();

  void selectSqlQueryBenchmark();
  void selectSqlQueryBenchmark_data();

  void selectAsyncSqlQuerySyncUsageNextBenchmark();
  void selectAsyncSqlQuerySyncUsageNextBenchmark_data();

  void selectAsyncSqlQuerySyncUsageFetchRecordsBenchmark();
  void selectAsyncSqlQuerySyncUsageFetchRecordsBenchmark_data();

 private:

  void prepareSelectBenchmarkData();
  bool insertCountClients(int n);
  Mdt::QueryExpression::SelectStatement selectAllClientStatement() const;

};

#endif // #ifndef MDT_SQL_ASYNC_QUERY_BENCHMARK_H
