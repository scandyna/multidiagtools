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
#ifndef MDT_ITEM_MODEL_STL_CONTAINER_ITERATOR_ADAPTER_H
#define MDT_ITEM_MODEL_STL_CONTAINER_ITERATOR_ADAPTER_H

#include <QtGlobal>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace Mdt{ namespace ItemModel{

  namespace Impl{

    /*! \brief Implementation for StlContainerIteratorAdapter::initializeContainer()
     *
     * This version works for containers that provides
     *  a constructor of the form:
     *  \code
     *  Container(size_type count, const T & value);
     *  \endcode
     */
    template<typename Container>
    struct InitializeContainer
    {
      template<typename T>
      static Container initialize(int count, const T & value)
      {
        return Container(count, value);
      }
    };

    /*! \brief Implementation for StlContainerIteratorAdapter::initializeContainer()
     *
     * This is a specialization for QList
     */
    template<typename T>
    struct InitializeContainer< QList<T> >
    {
      static QList<T> initialize(int count, const T & value)
      {
        QList<T> list;
        list.reserve(count);
        for(int i = 0; i < count; ++i){
          list.append(value);
        }
        return list;
      }
    };

  } // namespace Impl{

  /*! \brief Class template that provides the requirements for AbstractStlTableModel subclasses
   *
   * This implementation works with STL iterators.
   */
  template<typename Container>
  class StlContainerIteratorAdapter
  {
   public:

    /*! \brief STL compatible value type
     */
    using value_type = typename Container::value_type;

    /*! \brief Get the size of \a container
     */
    static int containerSize(const Container & container)
    {
      return std::distance(container.cbegin(), container.cend());
    }

    /*! \brief Check if \a container is empty
     */
    static bool containerIsEmpty(const Container & container)
    {
      return ( container.cbegin() == container.cend() );
    }

    /*! \brief Get value at \a index in \a container
     *
     * \pre \a index must be in valid range ( 0 <= \a index < containerSize(container) )
     */
    static value_type valueAtIndex(const Container & container, int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < containerSize(container));

      return *std::next(container.cbegin(), index);
    }

    /*! \brief Set \a value at \a index in \a container
     *
     * \pre \a index must be in valid range ( 0 <= \a index < containerSize(container) )
     */
    static void setValueAtIndex(Container & container, int index, const value_type & value)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < containerSize(container));

      *std::next(container.begin(), index) = value;
    }

    /*! \brief Initialize \a container with \a count copies of \a value
     *
     * \pre \a count must be >= 1
     */
    static Container initializeContainer(int count, const value_type & value)
    {
      Q_ASSERT(count >= 1);

      return Impl::InitializeContainer<Container>::initialize(count, value);
    }

  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_STL_CONTAINER_ITERATOR_ADAPTER_H
