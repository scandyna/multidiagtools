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
#include "TableCacheTaskMap.h"
#include <algorithm>

namespace Mdt{ namespace Container{

void TableCacheTaskMap::clear()
{
  mMap.clear();
  mLastTaskId = 0;
}

TableCacheRowTask TableCacheTaskMap::beginRowTask(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(!mapContainsRow(row));

  const TableCacheRowTask rowTask( row, createTask() );
  mMap.emplace_back(row, rowTask.task(), TableCacheTaskState::Pending);

  return rowTask;
}

TableCacheRowTaskList TableCacheTaskMap::beginRowTasks(const RowList & rows)
{
  TableCacheRowTaskList rowTaskList;

  for(const int row : rows){
    rowTaskList.emplace_back( beginRowTask(row) );
  }

  return rowTaskList;
}

void TableCacheTaskMap::shiftRows(int row, int count)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(count != 0);

  for(auto & item : mMap){
    if(item.row() >= row){
      item.shiftRow(count);
      Q_ASSERT(item.row() >= 0);
    }
  }
}

int TableCacheTaskMap::getRowForTask(const TableCacheTask & task) const
{
  Q_ASSERT(!task.isNull());

  const auto pred = [task](const TableCacheTaskMapItem & item){
    return item.taskId() == task.id();
  };

  const auto it = std::find_if(mMap.cbegin(), mMap.cend(), pred);
  if(it == mMap.cend()){
    return -1;
  }

  return it->row();
}

void TableCacheTaskMap::setTaskDoneForRow(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(mapContainsRow(row));

  mMap.erase( findConstIteratorForRow(row) );
}

void TableCacheTaskMap::setTaskFailedForRow(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(mapContainsRow(row));

  auto it = findIteratorForRow(row);
  Q_ASSERT(it != mMap.end());
  it->setFailed();
}

bool TableCacheTaskMap::isTaskPendingForRow(int row) const
{
  Q_ASSERT(row >= 0);

  const auto it = findConstIteratorForRow(row);
  if(it == mMap.cend()){
    return false;
  }

  return it->isPending();
}

bool TableCacheTaskMap::isTaskFailedForRow(int row) const
{
  Q_ASSERT(row >= 0);

  const auto it = findConstIteratorForRow(row);
  if(it == mMap.cend()){
    return false;
  }

  return it->isFailed();
}

bool TableCacheTaskMap::mapContainsRow(int row) const
{
  Q_ASSERT(row >= 0);

  return findConstIteratorForRow(row) != mMap.cend();
}

TableCacheTaskMap::iterator TableCacheTaskMap::findIteratorForRow(int row)
{
  Q_ASSERT(row >= 0);

  const auto pred = [row](const TableCacheTaskMapItem & item){
    return item.row() == row;
  };

  return std::find_if(mMap.begin(), mMap.end(), pred);
}

TableCacheTaskMap::const_iterator TableCacheTaskMap::findConstIteratorForRow(int row) const
{
  Q_ASSERT(row >= 0);

  const auto pred = [row](const TableCacheTaskMapItem & item){
    return item.row() == row;
  };

  return std::find_if(mMap.cbegin(), mMap.cend(), pred);
}

TableCacheTask TableCacheTaskMap::createTask()
{
  ++mLastTaskId;
  if(mLastTaskId < 0){
    mLastTaskId = 1;
  }

  return TableCacheTask(mLastTaskId);
}

}} // namespace Mdt{ namespace Container{
