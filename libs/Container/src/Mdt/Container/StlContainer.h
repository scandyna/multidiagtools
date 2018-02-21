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
#ifndef MDT_CONTAINER_STL_CONTAINER_H
#define MDT_CONTAINER_STL_CONTAINER_H

#include <QtGlobal>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace Mdt{ namespace Container{

    /*! \brief Get the size of \a container
     */
    template<typename Container>
    int containerSize(const Container & container)
    {
      return std::distance(container.cbegin(), container.cend());
    }

    /*! \brief Check if \a container is empty
     */
    template<typename Container>
    bool containerIsEmpty(const Container & container)
    {
      return ( container.cbegin() == container.cend() );
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

    /*! \brief Get const value at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < containerSize(container) )
     */
    template<typename Container>
    auto constValueAtIndex(const Container & container, int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < containerSize(container));

      return *constIteratorAtIndex(container, index);
    }

    /*! \brief Insert \a count copies of \a value to \a container
     */
    template<typename Container, typename T>
    void insertToContainer(Container & container, int pos, int count, const T & value)
    {
      container.insert( std::next(container.begin() , pos), count, value );
    }

    /*! \brief Insert \a count copies of \a value to \a container
     *
     * This is a specialization for QList
     */
    template<typename T>
    void insertToContainer(QList<T> & container, int pos, int count, const T & value)
    {
      container.reserve(container.size() + count);
      for(int i = 0; i < count; ++i){
        container.insert(pos, value);
      }
    }

    /*! \brief Insert \a value to the end of \a container
     */
    template<typename Container, typename T>
    void appendToContainer(Container & container, const T & value)
    {
      insertToContainer(container, containerSize(container), 1, value);
    }

    /*! \brief Insert \a value to the beginning of \a container
     */
    template<typename Container, typename T>
    void prependToContainer(Container & container, const T & value)
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
    void removeFromContainer(Container & container, int pos, int count)
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
    void removeFirstFromContainer(Container & container)
    {
      Q_ASSERT(!containerIsEmpty(container));
      removeFromContainer(container, 0, 1);
    }

    /*! \brief Remove the last element from \a container
     *
     * \pre \a container must not be empty
     */
    template<typename Container>
    void removeLastFromContainer(Container & container)
    {
      Q_ASSERT(!containerIsEmpty(container));
      removeFromContainer(container, containerSize(container)-1, 1);
    }

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_STL_CONTAINER_H
