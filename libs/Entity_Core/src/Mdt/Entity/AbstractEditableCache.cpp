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
#include "AbstractEditableCache.h"
#include "Mdt/IndexRange/RowRange.h"

using Mdt::Container::TableCacheOperation;
using Mdt::IndexRange::RowRange;

namespace Mdt{ namespace Entity{

void AbstractEditableCache::setData(int row, int column, const QVariant& data)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  mOperationMap.setOperationAtRow(row, TableCacheOperation::Update);
  setDataFromBackend(row, column, data);
  emit operationAtRowsChanged(row, row);
}

void AbstractEditableCache::insertRecords(int row, int count, const VariantRecord & record)
{
  insertRecordsFromBackend(row, count, record);
  RowRange rows;
  rows.setFirstRow(row);
  rows.setRowCount(count);
  emit operationAtRowsChanged(rows.firstRow(), rows.lastRow());
}

}} // namespace Mdt{ namespace Entity{
