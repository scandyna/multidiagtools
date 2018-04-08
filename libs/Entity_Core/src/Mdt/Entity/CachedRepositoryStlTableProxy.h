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
#ifndef MDT_ENTITY_CACHED_REPOSITOR_STL_TABLE_PROXY_H
#define MDT_ENTITY_CACHED_REPOSITOR_STL_TABLE_PROXY_H

#include "AbstractCachedRepository.h"
#include "RepositoryHandle.h"
#include "StlTableProxyIterator.h"
#include <iterator>

namespace Mdt{ namespace Entity{

  /*! \internal
   */
  template<typename RepositoryInterface>
  class CachedRepositoryStlTableProxyImpl
  {
   public:

    using value_type = typename RepositoryInterface::value_type;

    CachedRepositoryStlTableProxyImpl() = default;

    void setRepository(const RepositoryHandle<RepositoryInterface> & repository)
    {
      mRepository = repository;
    }

    int rowCount() const
    {
      if(mRepository.isNull()){
        return 0;
      }
      return mRepository.constRepository().rowCount();
    }

    const value_type & constRecordAt(int row) const
    {
      Q_ASSERT(!mRepository.isNull());

      return mRepository.constRepository().constRecordAt(row);
    }

    value_type & recordAt(int row)
    {
      Q_ASSERT(!mRepository.isNull());

      return mRepository.repository().recordAt(row);
    }

    void insertRecords(int pos, int count, const value_type & record)
    {
      Q_ASSERT(!mRepository.isNull());

      mRepository.repository().insertRecords(pos, count, record);
    }

    void removeRecords(int pos, int count)
    {
      Q_ASSERT(!mRepository.isNull());

      mRepository.repository().removeRecords(pos, count);
    }

   private:

    RepositoryHandle<RepositoryInterface> mRepository;
  };

  /*! \brief STL like table interface to a repository based on AbstractCachedRepository
   *
   * \tparam RepositoryInterface A repository interface (in the domain model)
   *    based on AbstractCachedRepository .
   */
  template<typename RepositoryInterface>
  class CachedRepositoryStlTableProxy
  {
   public:

    /*! \brief STL style value type
     */
    using value_type = typename RepositoryInterface::value_type;

    /*! \brief STL style const iterator
     */
    using const_iterator = StlTableProxyConstIterator<value_type, CachedRepositoryStlTableProxyImpl<RepositoryInterface> >;

    /*! \brief STL style iterator
     */
    using iterator = StlTableProxyIterator<value_type, CachedRepositoryStlTableProxyImpl<RepositoryInterface> >;

    /*! \brief Construct a null proxy
     */
    CachedRepositoryStlTableProxy() = default;

    /*! \brief Set \a repository to this proxy
     */
    void setRepository(const RepositoryHandle<RepositoryInterface> & repository)
    {
      mImpl.setRepository(repository);
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return mImpl.rowCount();
    }

    /*! \brief Get a const iterator to the first element
     */
    const_iterator cbegin() const
    {
      return const_iterator(0, &mImpl);
    }

    /*! \brief Get a const iterator past the last element
     */
    const_iterator cend() const
    {
      return const_iterator( rowCount(), &mImpl );
    }

    /*! \brief Get a iterator to the first element
     */
    iterator begin()
    {
      return iterator(0, &mImpl);
    }

    /*! \brief Get a iterator past the last element
     */
    iterator end()
    {
      return iterator( rowCount(), &mImpl );
    }

    /*! \brief Insert \a count copies of \a record before \a pos
     */
    iterator insert(iterator pos, int count, const value_type & record)
    {
      mImpl.insertRecords( std::distance(begin(), pos), count, record );

      return pos;
    }

    /*! \brief Remove records in the range [\a first, \a last)
     */
    iterator erase(iterator first, iterator last)
    {
      mImpl.removeRecords( std::distance(begin(), first), std::distance(first, last) );

      if(last < end()){
        return last;
      }
      return end();
    }

   private:

    CachedRepositoryStlTableProxyImpl<RepositoryInterface> mImpl;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_CACHED_REPOSITOR_STL_TABLE_PROXY_H
