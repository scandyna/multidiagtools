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
#ifndef MDT_CONTAINER_STL_CONTAINER_ITERATOR_ADAPTER_H
#define MDT_CONTAINER_STL_CONTAINER_ITERATOR_ADAPTER_H

#include "StlContainer.h"
#include <QtGlobal>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace Mdt{ namespace Container{

  /*! \brief Class template that provides the requirements various adapters (mainly record adapters)
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
    static int containerSize(const Container & container) noexcept
    {
      return Mdt::Container::containerSize(container);
    }

    /*! \brief Check if \a container is empty
     */
    static bool containerIsEmpty(const Container & container)
    {
      return Mdt::Container::containerIsEmpty(container);
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

      return Mdt::Container::initializeContainer<Container>(count, value);
    }

  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_STL_CONTAINER_ITERATOR_ADAPTER_H
