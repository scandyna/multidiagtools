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
#ifndef MDT_SQL_ASYNC_TEST_QUERY_RECEIVER_H
#define MDT_SQL_ASYNC_TEST_QUERY_RECEIVER_H

#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include <QObject>
#include <vector>

/*
 * Helper class to receive events from a async query
 */
class AsyncTestQueryReceiver : public QObject
{
 Q_OBJECT

 public:

  bool isQueryDone() const
  {
    return mQueryDone;
  }

  bool hasError() const
  {
    return !mLastError.isNull();
  }

  bool isFinished() const
  {
    return isQueryDone() || hasError();
  }

  bool hasRecords() const
  {
    return recordCount() > 0;
  }

  bool hasLastInsertId() const
  {
    return !mLastInsertId.isNull();
  }

  bool hasResult() const
  {
    return hasRecords() || hasLastInsertId();
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

  QVariant lastInsertId() const
  {
    return mLastInsertId;
  }

  void clear()
  {
    mQueryDone = false;
    mRecordList.clear();
    mLastInsertId.clear();
    mLastError.clear();
  }

 public slots:

  void setQueryDone();
  void setLastError(const Mdt::Error & error);
  void storeNewRecord(const Mdt::Container::VariantRecord & record);
  void setLastInsertId(const QVariant & id);

 private:

  bool mQueryDone = false;
  std::vector<Mdt::Container::VariantRecord> mRecordList;
  QVariant mLastInsertId;
  Mdt::Error mLastError;
};

#endif // #ifndef MDT_SQL_ASYNC_TEST_QUERY_RECEIVER_H
