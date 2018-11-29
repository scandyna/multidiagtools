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

  const TableCacheRowTask task( row, createTask() );
  mMap.emplace( std::make_pair(row, TableCacheTaskMapItem(task.task(), TableCacheTaskState::Pending)) );

  return task;
}

int TableCacheTaskMap::getRowForTask(const TableCacheTask & task) const
{
  Q_ASSERT(!task.isNull());

  const auto pred = [task](const std::pair<Row, TableCacheTaskMapItem> & p){
    return p.second.taskId() == task.id();
  };

  const auto it = std::find_if(mMap.cbegin(), mMap.cend(), pred);
  if(it == mMap.cend()){
    return -1;
  }

  return it->first;
}

void TableCacheTaskMap::setTaskDoneForRow(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(mapContainsRow(row));

  mMap.erase(row);
}

void TableCacheTaskMap::setTaskFailedForRow(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(mapContainsRow(row));

  // std::map::operator[] requires TableCacheTaskMapItem to be default constructible
  auto it = mMap.find(row);
  Q_ASSERT(it != mMap.end());
  it->second.setFailed();
}

bool TableCacheTaskMap::isTaskPendingForRow(int row) const
{
  Q_ASSERT(row >= 0);

  const auto it = mMap.find(row);
  if(it == mMap.cend()){
    return false;
  }

  return it->second.isPending();
}

bool TableCacheTaskMap::isTaskFailedForRow(int row) const
{
  Q_ASSERT(row >= 0);

  const auto it = mMap.find(row);
  if(it == mMap.cend()){
    return false;
  }

  return it->second.isFailed();
}

bool TableCacheTaskMap::mapContainsRow(int row) const
{
  Q_ASSERT(row >= 0);

  return mMap.find(row) != mMap.cend();
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
