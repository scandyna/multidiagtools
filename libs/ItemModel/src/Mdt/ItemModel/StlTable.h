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
#ifndef MDT_ITEM_MODEL_STL_CONTAINER_H
#define MDT_ITEM_MODEL_STL_CONTAINER_H

#include <QtGlobal>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace Mdt{ namespace ItemModel{

    /*! \brief Compare size of a container in a stateful way
     *
     * This functor compares the size of the container
     *  with the size of its previous call.
     */
    template<typename RecordAdapter>
    class StateFulCompareContainerSize
    {
     public:

      /*! \brief Compare size of \a container with previous size
       *
       * At the first call, previous size will be stored
       *  and true is returned.
       */
      template<typename Container>
      bool operator()(const Container & container)
      {
        if(mPrevisouSize < 0){
          mPrevisouSize = mRecordAdapter.containerSize(container);
          return true;
        }
        const int size = mRecordAdapter.containerSize(container);
        if(size != mPrevisouSize){
          mPrevisouSize = size;
          return false;
        }
        return true;
      }

    private:

      int mPrevisouSize = -1;
      RecordAdapter mRecordAdapter;
    };

    /*! \brief Check that each record of the table has the same column count
     */
    template<typename Table, typename RecordAdapter>
    bool eachRecordHasSameColumnCount(const Table & table, const RecordAdapter &)
    {
      return std::all_of(table.cbegin(), table.cend(), StateFulCompareContainerSize<RecordAdapter>());
    }

}}

#endif // #ifndef MDT_ITEM_MODEL_STL_CONTAINER_H
