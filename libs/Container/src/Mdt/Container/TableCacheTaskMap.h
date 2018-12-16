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

#include "TableCacheTask.h"
#include "TableCacheRowTask.h"
#include "TableCacheRowTaskList.h"
#include "RowList.h"
#include "Mdt/Assert.h"
#include "MdtContainerExport.h"
#include <vector>

namespace Mdt{ namespace Container{

  /*! \internal A item in TableCacheTaskMap
   */
  class MDT_CONTAINER_EXPORT TableCacheTaskMapItem
  {
   public:

    constexpr TableCacheTaskMapItem(int row, TableCacheTask task, bool isPending, bool isFailed) noexcept
     : mRow(row),
       mTask(task),
       mIsPending(isPending),
       mIsFailed(isFailed)
    {
    }

    constexpr TableCacheTaskMapItem(const TableCacheTaskMapItem & other) noexcept = default;
    constexpr TableCacheTaskMapItem & operator=(const TableCacheTaskMapItem & other) noexcept = default;
    constexpr TableCacheTaskMapItem(TableCacheTaskMapItem && other) noexcept = default;
    constexpr TableCacheTaskMapItem & operator=(TableCacheTaskMapItem && other) noexcept = default;

    constexpr void shiftRow(int count) noexcept
    {
      mRow += count;
      MDT_ASSERT(mRow >= 0);
    }

    constexpr int row() const noexcept
    {
      return mRow;
    }

    constexpr int taskId() const noexcept
    {
      return mTask.id();
    }

    constexpr void setPending() noexcept
    {
      mIsPending = true;
    }

    constexpr bool isPending() const noexcept
    {
      return mIsPending;
    }

    constexpr void setFailed() noexcept
    {
      mIsFailed = true;
      mIsPending = false;
    }

    constexpr bool isFailed() const noexcept
    {
      return mIsFailed;
    }

   private:

    int mRow;
    TableCacheTask mTask;
    bool mIsPending;
    bool mIsFailed;
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
     */
    TableCacheRowTask beginRowTask(int row);

    /*! \brief Begin a list of row tasks
     */
    TableCacheRowTaskList beginRowTasks(const RowList & rows);

    /*! \brief Shift the rows for a insertion of \a count rows starting before \a row
     *
     * \pre \a row must be >= 0
     * \pre \a count must be >= 1
     */
    void shiftRowsForInsert(int row, int count);

    /*! \brief Shift the rows for a removal of \a count rows starting from \a row
     *
     * \pre \a row must be >= 0
     * \pre \a count must be >= 1
     */
    void shiftRowsForRemove(int row, int count);

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
