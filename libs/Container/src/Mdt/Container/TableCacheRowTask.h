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
#ifndef MDT_CONTAINER_TABLE_CACHE_ROW_TASK_H
#define MDT_CONTAINER_TABLE_CACHE_ROW_TASK_H

#include "TableCacheTask.h"
#include "MdtContainerExport.h"
#include "Mdt/Assert.h"
#include <QtGlobal>

namespace Mdt{ namespace Container{

  /*! \brief A task for a row in a table cache
   */
  class MDT_CONTAINER_EXPORT TableCacheRowTask
  {
   public:

    /*! \brief Construct a null task
     */
    constexpr TableCacheRowTask() noexcept = default;

    /*! \brief Construct a task
     *
     * \pre \a row must be >= 0
     * \pre \a task must not be null
     */
    constexpr TableCacheRowTask(int row, const TableCacheTask & task) noexcept
     : mRow(row),
       mTask(task)
    {
      MDT_ASSERT(row >= 0);
      MDT_ASSERT(!task.isNull());
    }

    /*! \brief Copy construct a task from \a other
     */
    constexpr TableCacheRowTask(const TableCacheRowTask & other) noexcept = default;

    /*! \brief Copy assign \a other to this task
     */
    constexpr  TableCacheRowTask & operator=(const TableCacheRowTask & other) noexcept = default;

    /*! \brief Move construct a task from \a other
     */
    constexpr TableCacheRowTask(TableCacheRowTask && other) noexcept = default;

    /*! \brief Move assign \a other to this task
     */
    constexpr  TableCacheRowTask & operator=(TableCacheRowTask && other) noexcept = default;

    /*! \brief Get the row
     */
    constexpr int row() const noexcept
    {
      return mRow;
    }

    /*! \brief Get task
     */
    constexpr TableCacheTask task() const noexcept
    {
      return mTask;
    }

    /*! \brief Get task id
     */
    constexpr int taskId() const noexcept
    {
      return mTask.id();
    }

    /*! \brief Check if this task is null
     */
    constexpr bool isNull() const noexcept
    {
      if(mRow < 0){
        return true;
      }
      if(mTask.isNull()){
        return true;
      }
      return false;
    }

   private:

    int mRow = -1;
    TableCacheTask mTask;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_ROW_TASK_H
