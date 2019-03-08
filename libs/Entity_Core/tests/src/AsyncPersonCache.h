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

#include "Mdt/Entity/AbstractEditableCache.h"

#include <QObject>
#include <QStringList>

class AsyncPersonCacheCommonImpl : public QObject
{
 Q_OBJECT

 public:

  int columnCount() const noexcept
  {
    return mImpl.columnCount();
  }

  QString horizontalHeaderName(int column) const
  {
    return mImpl.horizontalHeaderName(column);
  }

  void setBackendLatency(int ms)
  {
    Q_ASSERT(ms >= 0);

    mBackendLatency = ms;
  }

  int backendLatency() const noexcept
  {
    return mBackendLatency;
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

  void updateRecordInStorage(int row, const Person & record)
  {
    mImpl.updateRecordInStorage(row, record);
  }

  void removeRecordFromStorage(int row)
  {
    mImpl.removeRecordFromStorage(row);
  }

  void clearStorage()
  {
    mImpl.clearStorage();
  }

  std::vector<Mdt::Container::VariantRecord> getRecordsFromStorage(int count)
  {
    return mImpl.getRecordsFromStorage(count);
  }

  int getNextId() const
  {
    return mImpl.getNextId();
  }

  bool fetchRecords(int count);

 signals:

  void newRecordAvailable(const Mdt::Container::VariantRecord & record);

 private slots:

  void sendNewRecordToCache();

 private:

  PersonCacheCommonImpl mImpl;
  std::vector<Mdt::Container::VariantRecord> mRecordsToSend;
  int mBackendLatency = 25;
};

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

  void setBackendLatency(int ms)
  {
    Q_ASSERT(ms >= 0);

    mImpl.setBackendLatency(ms);
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

 private:

  bool fetchRecords(int count) override;

  AsyncPersonCacheCommonImpl mImpl;
};

class AsyncEditPersonCache : public Mdt::Entity::AbstractEditableCache
{
 Q_OBJECT

 public:

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

 private:

  bool fetchRecords(int count) override;
  bool addRecordToBackend(int row) override;
  bool updateRecordInBackend(int row) override;
  bool removeRecordFromBackend(int row) override;

  AsyncPersonCacheCommonImpl mImpl;
};

void populatePersonStorage(AsyncPersonCache & pc, const QStringList & names);
void populatePersonStorage(AsyncEditPersonCache & pc, const QStringList & names);

#endif // #ifndef ASYNC_PERSON_CACHE_H
