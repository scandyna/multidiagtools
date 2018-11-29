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
#ifndef MDT_CONTAINER_TABLE_CACHE_TASK_H
#define MDT_CONTAINER_TABLE_CACHE_TASK_H

#include "MdtContainerExport.h"
#include <QtGlobal>

namespace Mdt{ namespace Container{

  /*! \brief Represents a task in a table cache
   */
  class MDT_CONTAINER_EXPORT TableCacheTask
  {
   public:

    /*! \brief Construct a null task
     */
    constexpr TableCacheTask() noexcept = default;

    /*! \brief Construct a task with a id
     *
     * \pre \a id must be > 0
     */
    constexpr explicit TableCacheTask(int id) noexcept
     : mId(id)
    {
      Q_ASSERT(id > 0);
    }

    /*! \brief Copy construct a task from \a other
     */
    constexpr TableCacheTask(const TableCacheTask & other) noexcept = default;

    /*! \brief Copy assign \a other to this task
     */
    constexpr  TableCacheTask & operator=(const TableCacheTask & other) noexcept = default;

    /*! \brief Move construct a task from \a other
     */
    constexpr TableCacheTask(TableCacheTask && other) noexcept = default;

    /*! \brief Move assign \a other to this task
     */
    constexpr  TableCacheTask & operator=(TableCacheTask && other) noexcept = default;

    /*! \brief Get the id of this task
     */
    constexpr int id() const noexcept
    {
      return mId;
    }

    /*! \brief Check if this task is null
     */
    constexpr bool isNull() const noexcept
    {
      return mId < 1;
    }

   private:

    int mId = 0;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_TASK_H
