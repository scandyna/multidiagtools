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

using namespace Mdt::Entity;

AsyncPersonCache::AsyncPersonCache(QObject* parent)
 : BaseClass(parent)
{
  connect(this, &AsyncPersonCache::newRecordAvailable, this, &AsyncPersonCache::fromBackendAppendRecord);
}

bool AsyncPersonCache::fetchRecords(int count)
{
  mRecordsToSend = mImpl.getRecordsFromStorage(count);
  std::reverse(mRecordsToSend.begin(), mRecordsToSend.end());

  for(const auto & record : mRecordsToSend){
    QTimer::singleShot(25, this, &AsyncPersonCache::sendNewRecordToCache);
  }
  return true;
}

void AsyncPersonCache::sendNewRecordToCache()
{
  Q_ASSERT(!mRecordsToSend.empty());

  emit newRecordAvailable( mRecordsToSend.back() );
  mRecordsToSend.pop_back();
}


void populatePersonStorage(AsyncPersonCache & pc, const QStringList & names)
{
  populatePersonStorageImpl(pc, names);
}
