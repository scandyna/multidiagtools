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
#include "Mdt/Container/StlContainer.h"

using namespace Mdt::Container;
using Mdt::IndexRange::RowRange;

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
  /*
   * We signal cache was reset while clearing the cache,
   * then, fromBackendAppendRecord() will signal new incomming rows.
   * This has the adventage to also work with async fetching.
   */
  beginResetCache();
  mCache.clear();
  endResetCache();

  return fetchRecords( cachedRowCountLimit() );
}

void AbstractReadOnlyCache::fromBackendSetRecord(int row, const VariantRecord& record)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(record.columnCount() == columnCount());

  mCache[row] = record;
  emit dataAtRowsChanged(row, row);
}

void AbstractReadOnlyCache::fromBackendInsertRecords(int row, int count, const VariantRecord& record)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row <= rowCount());
  Q_ASSERT(count >= 1);
  Q_ASSERT( (rowCount()+count) <= cachedRowCountLimit() );
  Q_ASSERT(record.columnCount() == columnCount());

  RowRange rows;
  rows.setFirstRow(row);
  rows.setRowCount(count);
  beginInsertRows(rows);
  insertToContainer(mCache, row, count, record);
  endInsertRows();
}

void AbstractReadOnlyCache::fromBackendRemoveRows(int row, int count)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 0);
  Q_ASSERT( (row + count) <= rowCount() );

  RowRange rows;
  rows.setFirstRow(row);
  rows.setRowCount(count);
  beginRemoveRows(rows);
  removeFromContainer(mCache, row, count);
  endRemoveRows();
}

void AbstractReadOnlyCache::fromBackendSetData(int row, int column, const QVariant& data)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  mCache[row].setValue(column, data);
  emit dataAtRowsChanged(row, row);
}

void AbstractReadOnlyCache::fromBackendAppendRecord(const VariantRecord& record)
{
  Q_ASSERT(rowCount() < cachedRowCountLimit());
  Q_ASSERT(record.columnCount() == columnCount());

  fromBackendInsertRecords(rowCount(), 1, record);
}

void AbstractReadOnlyCache::setLastError(const Error& error)
{
  mLastError = error;
}

void AbstractReadOnlyCache::beginResetCache()
{
  emit cacheAboutToBeReset();
}

void AbstractReadOnlyCache::endResetCache()
{
  emit cacheReset();
}

void AbstractReadOnlyCache::beginInsertRows(RowRange rows)
{
  Q_ASSERT(rows.isValid());

  emit rowsAboutToBeInserted(rows.firstRow(), rows.lastRow());
}

void AbstractReadOnlyCache::endInsertRows()
{
  emit rowsInserted();
}

void AbstractReadOnlyCache::beginRemoveRows(RowRange rows)
{
  Q_ASSERT(rows.isValid());

  emit rowsAboutToBeRemoved(rows.firstRow(), rows.lastRow());
}

void AbstractReadOnlyCache::endRemoveRows()
{
  emit rowsRemoved();
}

}} // namespace Mdt{ namespace Entity{
