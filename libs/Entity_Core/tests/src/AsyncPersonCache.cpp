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
#include "AsyncPersonCache.h"
#include <QTimer>
#include <algorithm>

#include <QDebug>

using namespace Mdt::Entity;

bool AsyncPersonCacheCommonImpl::fetchRecords(int count)
{
  mRecordsToSend = mImpl.getRecordsFromStorage(count);
  std::reverse(mRecordsToSend.begin(), mRecordsToSend.end());

  QTimer::singleShot(mBackendLatency, this, &AsyncPersonCacheCommonImpl::sendNewRecordToCache);

  return true;
}

void AsyncPersonCacheCommonImpl::sendNewRecordToCache()
{
  Q_ASSERT(!mRecordsToSend.empty());

  qDebug() << "emit newRecordAvailable..";
  emit newRecordAvailable( mRecordsToSend.back() );
  mRecordsToSend.pop_back();

  if(!mRecordsToSend.empty()){
    QTimer::singleShot(mBackendLatency, this, &AsyncPersonCacheCommonImpl::sendNewRecordToCache);
  }
}


AsyncPersonCache::AsyncPersonCache(QObject* parent)
 : BaseClass(parent)
{
  connect(&mImpl, &AsyncPersonCacheCommonImpl::newRecordAvailable, this, &AsyncPersonCache::fromBackendAppendRecord);
}

bool AsyncPersonCache::fetchRecords(int count)
{
  return mImpl.fetchRecords(count);
}

bool AsyncEditPersonCache::fetchRecords(int count)
{
  return mImpl.fetchRecords(count);
}

bool AsyncEditPersonCache::addRecordToBackend(int row)
{

}

bool AsyncEditPersonCache::updateRecordInBackend(int row)
{

}

bool AsyncEditPersonCache::removeRecordFromBackend(int row)
{

}


void populatePersonStorage(AsyncPersonCache & pc, const QStringList & names)
{
  populatePersonStorageImpl(pc, names);
}

void populatePersonStorage(AsyncEditPersonCache& pc, const QStringList& names)
{
  populatePersonStorageImpl(pc, names);
}
