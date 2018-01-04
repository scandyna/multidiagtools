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

#include "MdtItemModelExport.h"
#include <QtGlobal>
#include <iterator>
#include <algorithm>

namespace Mdt{ namespace ItemModel{

    /*! \brief Get the size of \a container
     */
    template<typename Container>
    int MDT_ITEMMODEL_EXPORT containerSize(const Container & container)
    {
      return std::distance(container.cbegin(), container.cend());
    }

    /*! \brief Check if \a container is empty
     */
    template<typename Container>
    bool MDT_ITEMMODEL_EXPORT containerIsEmpty(const Container & container)
    {
      return ( container.cbegin() == container.cend() );
    }

    /*! \brief Compare size of a container in a stateful way
     *
     * This functor compares the size of the container
     *  with the size of its previous call.
     */
    class MDT_ITEMMODEL_EXPORT StateFulCompareContainerSize
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
          mPrevisouSize = containerSize(container);
          return true;
        }
        const int size = containerSize(container);
        if(size != mPrevisouSize){
          mPrevisouSize = size;
          return false;
        }
        return true;
      }

    private:

      int mPrevisouSize = -1;
    };

    /*! \brief Check that each record of the table has the same column count
     */
    template<typename Table>
    bool MDT_ITEMMODEL_EXPORT eachRecordHasSameColumnCount(const Table & table)
    {
      return std::all_of(table.cbegin(), table.cend(), StateFulCompareContainerSize());
    }

    /*! \brief Get a const iterator to the element at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < containerSize(container) )
     */
    template<typename Container>
    auto constIteratorAtIndex(const Container & container, int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < containerSize(container));

      return std::next(container.cbegin(), index);
    }

    /*! \brief Get a iterator to the element at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < containerSize(container) )
     */
    template<typename Container>
    auto iteratorAtIndex(Container & container, int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < containerSize(container));

      return std::next(container.begin(), index);
    }

    /*! \brief Get a const iterator to the element at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < containerSize(table) )
     * \pre \a column must be in valid range ( 0 <= \a column < containerSize(*table.cbegin()) )
     */
    template<typename Table>
    auto constIteratorAtRowColumn(const Table & table, int row, int column)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < containerSize(table));
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < containerSize(*table.cbegin()));

      const auto recordIt = constIteratorAtIndex(table, row);

      return constIteratorAtIndex(*recordIt, column);
    }

    /*! \brief Get a iterator to the element at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < containerSize(table) )
     * \pre \a column must be in valid range ( 0 <= \a column < containerSize(*table.cbegin()) )
     */
    template<typename Table>
    auto iteratorAtRowColumn(Table & table, int row, int column)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < containerSize(table));
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < containerSize(*table.cbegin()));

      auto recordIt = iteratorAtIndex(table, row);

      return iteratorAtIndex(*recordIt, column);
    }

    /*! \brief Insert \a count copies of \a value to \a container
     */
    template<typename Container, typename T>
    void MDT_ITEMMODEL_EXPORT insertToContainer(Container & container, int pos, int count, const T & value)
    {
      container.insert( std::next(container.begin() , pos), count, value );
    }

    /*! \brief Insert \a count copies of \a value to \a container
     *
     * This is a specialization for QList
     */
    template<typename T>
    void MDT_ITEMMODEL_EXPORT insertToContainer(QList<T> & container, int pos, int count, const T & value)
    {
      container.reserve(container.size() + count);
      for(int i = 0; i < count; ++i){
        container.insert(pos, value);
      }
    }

    /*! \brief Insert \a value to the end of \a container
     */
    template<typename Container, typename T>
    void MDT_ITEMMODEL_EXPORT appendToContainer(Container & container, const T & value)
    {
      insertToContainer(container, containerSize(container), 1, value);
    }

    /*! \brief Insert \a value to the beginning of \a container
     */
    template<typename Container, typename T>
    void MDT_ITEMMODEL_EXPORT prependToContainer(Container & container, const T & value)
    {
      insertToContainer(container, 0, 1, value);
    }

    /*! \brief Remove \a count element starting from \a pos from \a container
     *
     * If \a pos + \a count is grater than upper bound of \a container,
     *  it will be ajusted.
     *
     * \pre \a pos must be >= 0
     * \pre \a count must be >= 1
     * \pre \a pos + \a count must be in valid range ( 1 <= \a pos + \a count <= containerSize( \a container ) )
     */
    template<typename Container>
    void MDT_ITEMMODEL_EXPORT removeFromContainer(Container & container, int pos, int count)
    {
      Q_ASSERT(pos >= 0);
      Q_ASSERT(count >= 0);
      Q_ASSERT( (pos + count) <= containerSize(container) );
      container.erase( std::next(container.begin() , pos), std::next(container.begin() , pos + count) );
    }

    /*! \brief Remove the first element from \a container
     *
     * \pre \a container must not be empty
     */
    template<typename Container>
    void MDT_ITEMMODEL_EXPORT removeFirstFromContainer(Container & container)
    {
      Q_ASSERT(!containerIsEmpty(container));
      removeFromContainer(container, 0, 1);
    }

    /*! \brief Remove the last element from \a container
     *
     * \pre \a container must not be empty
     */
    template<typename Container>
    void MDT_ITEMMODEL_EXPORT removeLastFromContainer(Container & container)
    {
      Q_ASSERT(!containerIsEmpty(container));
      removeFromContainer(container, containerSize(container)-1, 1);
    }

}}

#endif // #ifndef MDT_ITEM_MODEL_STL_CONTAINER_H
