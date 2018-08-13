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
#include "AbstractCacheTableModel.h"

namespace Mdt{ namespace Entity{

AbstractCacheTableModel::AbstractCacheTableModel(QObject* parent)
 : ParentClass(parent)
{
}

void AbstractCacheTableModel::disconnectCommonSignalsAndSlots()
{
  disconnect(mCacheAboutToBeResetConnection);
  disconnect(mCacheResetConnection);
  disconnect(mDataAtRowsChangedConnection);
  disconnect(mRowsAboutToBeInsertedConnection);
  disconnect(mRowsInsertedConnection);
}

void AbstractCacheTableModel::emitDataAtRowsChanged(int firstRow, int lastRow)
{
  Q_ASSERT(firstRow >= 0);
  Q_ASSERT(firstRow < rowCount());
  Q_ASSERT(lastRow >= 0);
  Q_ASSERT(lastRow < rowCount());

  const auto topLeft = index(firstRow, 0);
  const auto bottomRight = index(lastRow, columnCount()-1);

  emit dataChanged(topLeft, bottomRight);
}

void AbstractCacheTableModel::beginInsertRows(int firstRow, int lastRow)
{
  Q_ASSERT(firstRow >= 0);
  Q_ASSERT(firstRow <= rowCount());
  Q_ASSERT(lastRow >= firstRow);

  ParentClass::beginInsertRows(QModelIndex(), firstRow, lastRow);
}

void AbstractCacheTableModel::endInsertRows()
{
  ParentClass::endInsertRows();
}

void AbstractCacheTableModel::beginRemoveRows(int firstRow, int lastRow)
{

}

void AbstractCacheTableModel::endRemoveRows()
{

}

}} // namespace Mdt{ namespace Entity{
