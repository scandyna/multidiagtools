/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "AbstractReadOnlyCache.h"

namespace Mdt{ namespace Entity{

AbstractReadOnlyCache::AbstractReadOnlyCache(QObject* parent)
 : QObject(parent)
{
}

void AbstractReadOnlyCache::setCachedRowCountLimit(int limit)
{
  Q_ASSERT(limit > 0);

  mCachedRowCountLimit = limit;
}

QString AbstractReadOnlyCache::horizontalHeaderName(int) const
{
  return QString();
}

bool AbstractReadOnlyCache::fetchAll()
{
  beginResetCache();
  mCache.clear();
  const bool ok = fetchRecords( cachedRowCountLimit() );
  endResetCache();

  return ok;
}

void AbstractReadOnlyCache::setDataFromBackend(int row, int column, const QVariant& data)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  mCache[row].setValue(column, data);
}

void AbstractReadOnlyCache::appendRecordFromBackend(const VariantRecord& record)
{
  Q_ASSERT(rowCount() < cachedRowCountLimit());
  Q_ASSERT(record.columnCount() == columnCount());

  mCache.push_back(record);
}

void AbstractReadOnlyCache::setLastError(const Error& error)
{
  mLastError = error;
}

// void AbstractReadOnlyCache::appendRecordToCache(const VariantRecord& record)
// {
//   Q_ASSERT(rowCount() < cachedRowCountLimit());
//   Q_ASSERT(record.columnCount() == columnCount());
// 
//   mCache.push_back(record);
// }

void AbstractReadOnlyCache::beginResetCache()
{
  emit cacheAboutToBeReset();
}

void AbstractReadOnlyCache::endResetCache()
{
  emit cacheReset();
}

}} // namespace Mdt{ namespace Entity{
