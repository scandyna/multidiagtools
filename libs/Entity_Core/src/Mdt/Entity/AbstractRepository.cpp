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
#include "AbstractRepository.h"

namespace Mdt{ namespace Entity{

AbstractRepository::AbstractRepository(QObject* parent)
 : QObject(parent)
{
}

void AbstractRepository::setLastError(const Mdt::Error & error)
{
  mLastError = error;
}

void AbstractRepository::beginResetCache()
{
  emit cacheAboutToBeReset();
}

void AbstractRepository::endResetCache()
{
  emit cacheReset();
}

void AbstractRepository::emitOperationAtRowsChanged(int firstRow, int lastRow)
{
  emit operationAtRowsChanged(firstRow, lastRow);
}

void AbstractRepository::emitOperationAtRowChanged(int row)
{
  emit operationAtRowsChanged(row, row);
}

void AbstractRepository::emitDataAtRowsChanged(int firstRow, int lastRow)
{
  emit dataAtRowsChanged(firstRow, lastRow);
}

void AbstractRepository::emitDataAtRowChanged(int row)
{
  emit dataAtRowsChanged(row, row);
}

void AbstractRepository::beginInsertRows(int firstRow, int lastRow)
{
  emit rowsAboutToBeInserted(firstRow, lastRow);
}

void AbstractRepository::endInsertRows()
{
  emit rowsInserted();
}

void AbstractRepository::beginRemoveRows(int firstRow, int lastRow)
{
  emit rowsAboutToBeRemoved(firstRow, lastRow);
}

void AbstractRepository::endRemoveRows()
{
  emit rowsRemoved();
}

}} // namespace Mdt{ namespace Entity{
