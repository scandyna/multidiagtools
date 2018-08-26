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
#ifndef MDT_SQL_ASYNC_QUERY_TEST_H
#define MDT_SQL_ASYNC_QUERY_TEST_H

#include "TestBase.h"
#include "Mdt/Container/VariantRecord.h"
#include <vector>

/*
 * Helper class to receive events from a async query
 */
class AsyncTestQueryReceiver : public QObject
{
 Q_OBJECT

 public:

  bool hasResult() const
  {
    return recordCount() > 0;
  }

  int recordCount() const
  {
    return mRecordList.size();
  }

  Mdt::Container::VariantRecord recordAt(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < recordCount());

    return mRecordList[index];
  }

  void clear()
  {
    mRecordList.clear();
  }

 public slots:

  void storeNewRecord(const Mdt::Container::VariantRecord & record);

 private:

  std::vector<Mdt::Container::VariantRecord> mRecordList;
};

class AsyncQueryTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void generateConnectionNameTest();
  void generateConnectionNameTest_data();

  void connectionSetupTest();

  void createQueryTest();
  void simpleSelectTest();
  void twoQueriesSelectTest();
};

#endif // #ifndef MDT_SQL_ASYNC_QUERY_TEST_H
