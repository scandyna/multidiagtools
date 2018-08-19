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
#ifndef ASYNC_PERSON_CACHE_H
#define ASYNC_PERSON_CACHE_H

#include "PersonCache.h"

#include "Mdt/Entity/AbstractAsyncReadOnlyCache.h"
#include <QStringList>

class AsyncPersonCache : public Mdt::Entity::AbstractAsyncReadOnlyCache
{
 Q_OBJECT

  using BaseClass = AbstractAsyncReadOnlyCache;

 public:

  explicit AsyncPersonCache(QObject *parent = nullptr);

  int columnCount() const noexcept override
  {
    return mImpl.columnCount();
  }

  QString horizontalHeaderName(int column) const override
  {
    return mImpl.horizontalHeaderName(column);
  }

  int storageRowCount() const
  {
    return mImpl.storageRowCount();
  }

  const Person & recordFromStorage(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < storageRowCount());

    return mImpl.recordFromStorage(row);
  }

  void appendRecordToStorage(Person record)
  {
    mImpl.appendRecordToStorage(record);
  }

  void clearStorage()
  {
    mImpl.clearStorage();
  }

 signals:

  void newRecordAvailable(const Mdt::Entity::VariantRecord & record);

 private slots:

  void sendNewRecordToCache();

 private:

  bool fetchRecords(int count) override;

  PersonCacheCommonImpl mImpl;
  std::vector<Mdt::Entity::VariantRecord> mRecordsToSend;
};

void populatePersonStorage(AsyncPersonCache & pc, const QStringList & names);

#endif // #ifndef ASYNC_PERSON_CACHE_H
