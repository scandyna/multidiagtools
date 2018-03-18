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
#ifndef MDT_ENTITY_DATA_LIST_H
#define MDT_ENTITY_DATA_LIST_H

#include "DataTemplate.h"
#include "Mdt/Container/StlContainer.h"
#include <QtGlobal>
#include <iterator>
#include <vector>

namespace Mdt{ namespace Entity{

  /*! \brief Class template to represent a list of entity data
   *
   * Example:
   * \code
   * class ArticleData : public DataTemplate<ArticleDataStruct, ArticleDef>
   * {
   * };
   *
   * using ArticleDataList = DataList<ArticleData>;
   * \endcode
   *
   * To add functionnality to the specific data list, inheritance could be used:
   * \code
   * class ArticleDataList : public DataList<ArticleData>
   * {
   *  public:
   *
   *   int rowOfArticleName(const QString & name) const;
   * };
   * \endcode
   * Note that adding members to the derived class is not recommanded
   *  (there values can be loosed in case of slicing).
   */
  template<typename EntityData, typename Container = std::vector<EntityData> >
  class DataList
  {
   public:

    /*! \brief STL-style const iterator
     */
    using const_iterator = typename Container::const_iterator;

    /*! \brief STL-style iterator
     */
    using iterator = typename Container::iterator;

    /*! \brief STL-style const pointer
     */
    using const_pointer = typename Container::const_pointer;

    /*! \brief STL-style pointer
     */
    using pointer = typename Container::pointer;

    /*! \brief STL-style const reference
     */
    using const_reference = typename Container::const_reference;

    /*! \brief STL-style reference
     */
    using reference = typename Container::reference;

    /*! \brief STL-style size
     */
    using size_type = typename Container::size_type;

    /*! \brief STL-syle difference
     */
    using difference_type = typename Container::difference_type;

    /*! \brief STL-style value
     */
    using value_type = typename Container::value_type;

    /*! \brief Construct a empty data list
     */
    explicit DataList() = default;

    /*! \brief Construct a data list with a initial size of \a size default constructed elements
     */
    explicit DataList(int size)
     : mContainer( Mdt::Container::initializeContainer<Container>(size, value_type{}) )
    {
    }

    /*! \brief Copy construct a data list from \a other
     */
    DataList(const DataList & other) = default;

    /*! \brief Copy assign \a other to this data list
     */
    DataList & operator=(const DataList & other) = default;

    /*! \brief Move construct a data list from \a other
     */
    DataList(DataList && other) = default;

    /*! \brief Move assign \a other to this data list
     */
    DataList & operator=(DataList && other) = default;

    /*! \brief Check if this data list is empty
     */
    bool isEmpty() const
    {
      return Mdt::Container::containerIsEmpty(mContainer);
    }

    /*! \brief Get count of element in this data list
     *
     * \sa size()
     */
    int count() const
    {
      return size();
    }

    /*! \brief Get the size of this list
     *
     * \sa count()
     */
    int size() const
    {
      return Mdt::Container::containerSize(mContainer);
    }

    /*! \brief Access element at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < size() ).
     * \sa operator[]
     */
    const EntityData & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return *Mdt::Container::constIteratorAtIndex(mContainer, index);
    }

    /*! \brief Access element at \a index as modifiable reference
     *
     * \pre \a index must be in valid range ( 0 <= \a index < size() ).
     * \sa at()
     */
    EntityData & operator[](int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return *Mdt::Container::iteratorAtIndex(mContainer, index);
    }

    /*! \brief Insert \a count copies of \a data before \a pos into this list
     *
     * \pre \a count must be >= 1
     */
    iterator insert(iterator pos, int count, const EntityData & data)
    {
      Q_ASSERT(count >= 0);
      return Mdt::Container::callInsert(mContainer, pos, count, data);
    }

    /*! \brief Add \a data to the end of this list
     */
    void append(const EntityData & data)
    {
      Mdt::Container::appendToContainer(mContainer, data);
    }

    /*! \brief Remove all data from first up to (but not including) last
     *
     * \return a %iterator to the same item that end() referred to before the call.
     */
    iterator erase(iterator first, iterator last)
    {
      return Mdt::Container::callErase(mContainer, first, last);
    }

    /*! \brief Remove last element from this data list
     *
     * \pre this data list must not be empty
     */
    void removeLast()
    {
      Q_ASSERT(!isEmpty());
      Mdt::Container::removeLastFromContainer(mContainer);
    }

    /*! \brief Clear this data list
     */
    void clear()
    {
      mContainer.clear();
    }

    /*! \brief Get a STL-style iterator pointing to the first element in this data list
     */
    iterator begin()
    {
      return std::begin(mContainer);
    }

    /*! \brief Get a STL-style iterator pointing past the last element in this data list
     */
    iterator end()
    {
      return std::end(mContainer);
    }

    /*! \brief Get a STL-style const iterator pointing to the first element in this data list
     */
    const_iterator cbegin() const
    {
      return std::cbegin(mContainer);
    }

    /*! \brief Get a STL-style const iterator pointing past the last element in this data list
     */
    const_iterator cend() const
    {
      return std::cend(mContainer);
    }

    /*! \brief Get a STL-style const iterator pointing to the first element in this data list
     */
    const_iterator begin() const
    {
      return cbegin();
    }

    /*! \brief Get a STL-style const iterator pointing past the last element in this data list
     */
    const_iterator end() const
    {
      return cend();
    }

   private:

    Container mContainer;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_DATA_LIST_H
