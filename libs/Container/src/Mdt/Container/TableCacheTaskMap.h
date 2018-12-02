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
#ifndef MDT_CONTAINER_TABLE_CACHE_TASK_MAP_H
#define MDT_CONTAINER_TABLE_CACHE_TASK_MAP_H

#include "TableCacheTaskState.h"
#include "TableCacheTask.h"
#include "TableCacheRowTask.h"
#include "TableCacheRowTaskList.h"
#include "RowList.h"
#include "MdtContainerExport.h"
#include <vector>

namespace Mdt{ namespace Container{

  /*! \internal A item in TableCacheTaskMap
   */
  class MDT_CONTAINER_EXPORT TableCacheTaskMapItem
  {
   public:

    constexpr TableCacheTaskMapItem(int row, TableCacheTask task, TableCacheTaskState state) noexcept
     : mRow(row),
       mTask(task),
       mState(state)
    {
    }

    constexpr TableCacheTaskMapItem(const TableCacheTaskMapItem & other) noexcept = default;
    constexpr TableCacheTaskMapItem & operator=(const TableCacheTaskMapItem & other) noexcept = default;
    constexpr TableCacheTaskMapItem(TableCacheTaskMapItem && other) noexcept = default;
    constexpr TableCacheTaskMapItem & operator=(TableCacheTaskMapItem && other) noexcept = default;

    constexpr void shiftRow(int count) noexcept
    {
      mRow += count;
    }

    constexpr int row() const noexcept
    {
      return mRow;
    }

    constexpr int taskId() const noexcept
    {
      return mTask.id();
    }

    constexpr bool isPending() const noexcept
    {
      return mState == TableCacheTaskState::Pending;
    }

    constexpr void setFailed() noexcept
    {
      mState = TableCacheTaskState::Failed;
    }

    constexpr bool isFailed() const noexcept
    {
      return mState == TableCacheTaskState::Failed;
    }

   private:

    int mRow;
    TableCacheTask mTask;
    TableCacheTaskState mState;
  };

  /*!\brief A map of tasks in a table cache
   *
   * For a example that uses TableCacheTaskMap,
   *  see Mdt::ItemModel::AbstractReadOnlyCachedTableModel
   */
  class MDT_CONTAINER_EXPORT TableCacheTaskMap
  {
   public:

    /*! \brief Get count of tasks in this map
     */
    int taskCount() const noexcept
    {
      return mMap.size();
    }

    /*! \brief Check if this map is empty
     */
    bool isEmpty() const noexcept
    {
      return mMap.empty();
    }

    /*! \brief Clear this map
     */
    void clear();

    /*! \brief Begin a row task
     *
     * \pre \a row must be >= 0
     * \pre no task must allready exists for \a row
     */
    TableCacheRowTask beginRowTask(int row);

    /*! \brief Begin a list of row tasks
     */
    TableCacheRowTaskList beginRowTasks(const RowList & rows);

    /*! \brief Shift the rows starting from \a row with a offset of \a count
     *
     * \pre \a row must be >= 0
     * \pre \a count must be <> 0
     * \note \a count can also be < 0
     */
    void shiftRows(int row, int count);

    /*! \brief Get the row that actually correspond to \a task
     *
     * The returned row can be different than when the task was created,
     *  for example if some rows have been shifted in between
     *  (this is also why this task concept exists).
     *
     * Returns a row in a valid range if it exists for \a task ,
     *  otherwise -1
     *
     * \pre \a task must not be null
     */
    int getRowForTask(const TableCacheTask & task) const;

    /*! \brief Set the task corresponding to \a row done
     *
     * \pre \a row must be >= 0
     * \pre a task must exist for \a row
     */
    void setTaskDoneForRow(int row);

    /*! \brief Set the task corresponding to \a row failed
     *
     * \pre \a row must be >= 0
     * \pre a task must exist for \a row
     */
    void setTaskFailedForRow(int row);

    /*! \brief Check if a task is pending for \a row
     *
     * \pre \a row must be >= 0
     */
    bool isTaskPendingForRow(int row) const;

    /*! \brief Check if a task is failed for \a row
     *
     * \pre \a row must be >= 0
     */
    bool isTaskFailedForRow(int row) const;

   private:

    using Map = std::vector<TableCacheTaskMapItem>;

    using iterator = Map::iterator;
    using const_iterator = Map::const_iterator;

    bool mapContainsRow(int row) const;

    iterator findIteratorForRow(int row);
    const_iterator findConstIteratorForRow(int row) const;

    TableCacheTask createTask();

    int mLastTaskId = 0;
    Map mMap;
  };

}} // namespace Mdt{ namespace Container{

#endif // MDT_CONTAINER_TABLE_CACHE_TASK_MAP_H
