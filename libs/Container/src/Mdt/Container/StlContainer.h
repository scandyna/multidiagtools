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
#ifndef MDT_CONTAINER_STL_CONTAINER_H
#define MDT_CONTAINER_STL_CONTAINER_H

#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace Mdt{ namespace Container{

  namespace Impl{

    /*! \brief Implementation for initializeContainer()
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

    /*! \brief Implementation for initializeContainer()
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

    /*! \brief Implementation for initializeContainer()
     *
     * This is a specialization for QStringList
     */
    template<>
    struct InitializeContainer<QStringList>
    {
      static QStringList initialize(int count, const QString & str)
      {
        QStringList list;
        list.reserve(count);
        for(int i = 0; i < count; ++i){
          list.append(str);
        }
        return list;
      }
    };

  } // namespace Impl{

  /*! \brief Get a container initialized with \a count copies of \a value
   *
   * \pre \a count must be >= 1
   */
  template<typename Container>
  Container initializeContainer(int count, const typename Container::value_type & value)
  {
    Q_ASSERT(count >= 1);

    return Impl::InitializeContainer<Container>::initialize(count, value);
  }

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

  /*! \brief Call insert on a container
   *
   * \pre \a count must be >= 1
   * \return a iterator pointing to the first of the inserted items.
   */
  template<typename Container, typename T>
  typename Container::iterator callInsert(Container & container, typename Container::iterator pos, int count, const T & value)
  {
    Q_ASSERT(count >= 1);
    return container.insert(pos, count, value);
  }

  /*! \brief Call insert on a container
   *
   * This is a specialization for QList.
   *
   * \pre \a count must be >= 1
   * \return a iterator pointing to the first of the inserted items.
   */
  template<typename T>
  typename QList<T>::iterator callInsert(QList<T> & container, typename QList<T>::iterator pos, int count, const T & value)
  {
    Q_ASSERT(count >= 1);

    /*
     * After a call to reserve(),
     * QList will detach, and pos becomes a invalid iterator.
     * So, calculate the index referred by pos
     * then get a valid iterator back.
     */
    const int index = std::distance(container.begin(), pos);
    Q_ASSERT(index >= 0);
    container.reserve(container.size() + count);
    auto it = std::next(container.begin(), index);
    for(int i = 0; i < count; ++i){
      it = container.insert(it, value);
    }

    return it;
  }

  /*! \brief Insert \a count copies of \a value to \a container
   *
   * \tparam Container STL compatible container that provides a %insert() method of this form:
   *    \code
   *    iterator insert(iterator pos, size_type count, const T & value);
   *    \endcode
   *
   * \pre \a pos must be >= 0
   * \pre \a count must be >= 1
   */
  template<typename Container, typename T>
  void insertToContainer(Container & container, int pos, int count, const T & value)
  {
    Q_ASSERT(pos >= 0);
    Q_ASSERT(count >= 1);
    callInsert(container, std::next(container.begin() , pos), count, value);
  }

  /*! \brief Insert \a value to the end of \a container
   */
  template<typename Container, typename T>
  void appendToContainer(Container & container, const T & value)
  {
    container.push_back(value);
  }

  /*! \brief Insert \a value to the beginning of \a container
   */
  template<typename Container, typename T>
  void prependToContainer(Container & container, const T & value)
  {
    insertToContainer(container, 0, 1, value);
  }

  /*! \brief Remove all elements from \a first up to (but not including) \a last
   *
   * \return a iterator following the last removed element.
   */
  template<typename Container>
  typename Container::iterator callErase(Container & container, typename Container::iterator first, typename Container::iterator last)
  {
    return container.erase(first, last);
  }

  /*! \brief Remove \a count element starting from \a pos from \a container
   *
   * \tparam Container STL compatible container that provides a %erase() method of this form:
   *    \code
   *    iterator erase(iterator first, iterator last);
   *    \endcode
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
   callErase( container, std::next(container.begin() , pos), std::next(container.begin() , pos + count) );
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
