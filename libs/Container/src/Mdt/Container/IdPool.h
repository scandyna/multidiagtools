/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_CONTAINER_ID_POOL_H
#define MDT_CONTAINER_ID_POOL_H

#include "Mdt/Algorithm.h"
#include <type_traits>
#include <vector>
#include <algorithm>

namespace Mdt{ namespace Container{

  /*! \brief Pool of IDs
   *
   * Getting a new ID can be a simple increment:
   * \code
   * int nextId()
   * {
   *   ++mLastId;
   *   return mLastId;
   * }
   * \endcode
   *
   * The problem of above code is that it does not track the used IDs,
   *  and will just give a higher one at each call.
   *
   * For a example, take a task map to manage asynchronous tasks:
   * \code
   * struct Task
   * {
   *   Task(int id)
   *    : id(id)
   *   {
   *   }
   *
   *   int id;
   *   // Maybe other usefull memebers here..
   * };
   *
   * class TaskMap
   * {
   *  public:
   *
   *   Task createNewTask()
   *   {
   *     return Task( getNextTaskId() );
   *   }
   *
   *   void setTaskDone(const Task & task);
   *
   *  private:
   *
   *   int getNextTaskId();
   * };
   * \endcode
   *
   * If we continiously create new tasks, and later set them done,
   *  the task id is allways incremented:
   * \code
   * void submitWork(const Task & task);
   *
   * // At some point a function is called when work have been done
   * onWorkDone(const Task & task, TaskMap & map)
   * {
   *   map.setTaskDone(task);
   * }
   *
   * submitWork( map.createNewTask() ); // ID: 1
   * submitWork( map.createNewTask() ); // ID: 2
   * submitWork( map.createNewTask() ); // ID: 3
   * onWorkDone(...); // ID: 2
   * submitWork( map.createNewTask() ); // ID: 4
   * ...
   * submitWork( map.createNewTask() ); // ID: INT_MAX
   * submitWork( map.createNewTask() ); // ID: 0
   * submitWork( map.createNewTask() ); // ID: 1
   * onWorkDone(...); // ID: INT_MAX
   * onWorkDone(...); // ID: 0
   *
   * // At this point, we have maybe 5 tasks in fly:
   * //  [1,1,88,93,102,1025]
   * // Notice also that task with ID 1 also exists twice
   * \endcode
   *
   * This is what IdPool helps to solve.
   */
  template<typename T>
  class IdPool
  {
    static_assert( std::is_integral<T>::value, "T must be a integral type" );

    using Pool = std::vector<T>;
    using iterator = typename Pool::iterator;
    using const_iterator = typename Pool::const_iterator;

   public:

    /*! \brief Construct a empty ID pool
     */
    IdPool() noexcept = default;

    /*! \brief Copy construct a ID pool from \a other
     */
    IdPool(const IdPool & other) = default;

    /*! \brief Copy assign \a other to this ID pool
     */
    IdPool & operator=(const IdPool & other) = default;

    /*! \brief Move construct a ID pool from \a other
     */
    IdPool(IdPool && other) noexcept = default;

    /*! \brief Move assign \a other to this ID pool
     */
    IdPool & operator=(IdPool && other) noexcept = default;

    /*! \brief Get the next available id
     */
    T getNextAvailableId()
    {
      if(mPool.empty()){
        mPool.push_back(1);
        return 1;
      }

      const auto first = mPool.cbegin();
      Q_ASSERT(first != mPool.cend());
      if(*first > 1){
        addIdBefore(1, first);
        return 1;
      }

      const auto it = findLastConsecutiveId();
      Q_ASSERT(it != mPool.cend());
      const T id = *it + 1;
      addIdAfter(id, it);

      return id;
    }

    /*! \brief Release \a id
     */
    void releaseId(T id)
    {
      mPool.erase( std::remove(mPool.begin(), mPool.end(), id) );
    }

   private:

    /*! \brief Returns a iterator to the last item that is consecutive
     *
     * []: iterator will point past the end
     * [1]: iterator will point to value 1, index 0
     * [1,2,3,5,6]: iterator will point to value 3, index 2
     */
    const_iterator findLastConsecutiveId() const
    {
      return Mdt::Algorithm::findLastConsecutiveIntegralValue(mPool.cbegin(), mPool.cend());
    }

    /*! \brief Add a id to this pool
     */
    void addIdBefore(T id, const_iterator it)
    {
      mPool.insert(it, id);

      Q_ASSERT(std::is_sorted(mPool.cbegin(), mPool.cend()));
      Q_ASSERT(std::unique(mPool.begin(), mPool.end()) == mPool.end());
    }

    /*! \brief Add a id to this pool
     */
    void addIdAfter(T id, const_iterator it)
    {
      mPool.insert(it+1, id);

      Q_ASSERT(std::is_sorted(mPool.cbegin(), mPool.cend()));
      Q_ASSERT(std::unique(mPool.begin(), mPool.end()) == mPool.end());
    }

    Pool mPool;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_ID_POOL_H
