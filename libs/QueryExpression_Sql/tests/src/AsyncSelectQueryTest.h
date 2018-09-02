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
#ifndef MDT_TEST_MAIN_H
#define MDT_TEST_MAIN_H

#include "SelectQueryTestBase.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQuery.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include <memory>
#include <vector>

/*
 * Test use case
 */

class CollectPersons : public QObject
{
  using AsyncSelectQuery = Mdt::QueryExpression::AbstractAsyncSelectQuery;

 Q_OBJECT

 public:

  explicit CollectPersons(QObject *parent = nullptr);

  void setSelectQuery(std::unique_ptr<AsyncSelectQuery> && query);

  void getAllPerson();

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

  void setLastError(const Mdt::Error & error);

 private:

  std::unique_ptr<AsyncSelectQuery> mSelectQuery;
  std::vector<Mdt::Container::VariantRecord> mRecordList;
  Mdt::Error mLastError;
};

/*
 * Test class
 */
class AsyncSelectQueryTest : public SelectQueryTestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void execQueryTest();
  void execQueryMaxRowsTest();
  void fieldOrderTest();
  void collectPersonsUseCaseTest();
};

#endif // #ifndef MDT_TEST_MAIN_H
