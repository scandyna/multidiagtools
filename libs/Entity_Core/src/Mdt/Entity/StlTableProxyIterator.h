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
#ifndef MDT_ENTITY_STL_TABLE_PROXY_ITERATOR_H
#define MDT_ENTITY_STL_TABLE_PROXY_ITERATOR_H

#include "StlTableProxyImplementationInterface.h"
#include <QtGlobal>
#include <iterator>
#include <type_traits>

#include <QDebug>

namespace Mdt{ namespace Entity{

  /*! \brief STL style iterator base for StlTableProxy iterators
   */
  template<typename EntityData, typename Derived>
  class StlTableProxyIteratorBase
  {
   public:

    /*! \brief STL style value type
     */
    using value_type = typename std::remove_cv<EntityData>::type;
//     using value_type = EntityData;
    

//     /*! \brief STL reference
//      */
//     using reference = value_type &;

//     /*! \brief Const reference
//      */
//     using const_reference = const value_type &;

    /*! \brief STL pointer
     */
    using pointer = value_type*;

    /*! \brief STL style difference
     */
    using difference_type = int;

    /*! \brief STL iterator category
     */
    using iterator_category = std::random_access_iterator_tag;

    /*! \brief Construct a past the end iterator
     */
    StlTableProxyIteratorBase() = default;

    /*! \brief Construct a iterator
     *
     * \pre \a row must be >= 0
     */
    StlTableProxyIteratorBase(int row)
     : mCurrentRow(row)
    {
      Q_ASSERT(mCurrentRow >= 0);
    }

    /*! \brief Copy construct a iterator from \a other
     */
    StlTableProxyIteratorBase(const StlTableProxyIteratorBase & other) = default;

    /*! \brief Copy assign \a other to this iterator
     */
    StlTableProxyIteratorBase & operator=(const StlTableProxyIteratorBase & other) = default;

    /*! \brief Get the current row to which this iterator refers in Impl
     */
    int currentRow() const noexcept
    {
      return mCurrentRow;
    }

    /*! \brief Compare iterator a and b
     */
    friend
    bool operator==(const Derived & a, const Derived & b)
    {
      return a.currentRow() == b.currentRow();
    }

    /*! \brief Compare iterator a and b
     */
    friend
    bool operator!=(const Derived & a, const Derived & b)
    {
      return !(a == b);
    }

    /*! \brief Compare iterator a and b
     */
    friend
    bool operator<(const Derived & a, const Derived & b)
    {
      return a.currentRow() < b.currentRow();
    }

    /*! \brief Compare iterator a and b
     */
    friend
    bool operator<=(const Derived & a, const Derived & b)
    {
      return a.currentRow() <= b.currentRow();
    }

    /*! \brief Compare iterator a and b
     */
    friend
    bool operator>(const Derived & a, const Derived & b)
    {
      return a.currentRow() > b.currentRow();
    }

    /*! \brief Compare iterator a and b
     */
    friend
    bool operator>=(const Derived & a, const Derived & b)
    {
      return a.currentRow() >= b.currentRow();
    }

    /*! \brief Increment iterator (pre-increment)
     */
    Derived & operator++()
    {
      ++mCurrentRow;
      return *derivedThis();
    }

    /*! \brief Increment iterator (post-increment)
     */
    Derived operator++(int)
    {
      Derived tmp(*derivedThis());
      ++*this;
      return tmp;
    }

    /*! \brief Decrement iterator (pre-increment)
     */
    Derived & operator--()
    {
      Q_ASSERT(mCurrentRow > 0);
      --mCurrentRow;
      return *derivedThis();
    }

    /*! \brief Decrement iterator (post-increment)
     */
    Derived operator--(int)
    {
      Derived tmp(*derivedThis());
      --*this;
      return tmp;
    }

    /*! \brief Increment this iterator by \a n
     */
    Derived & operator+=(difference_type n)
    {
      mCurrentRow += n;
      Q_ASSERT(mCurrentRow >= 0);
      return *derivedThis();
    }

    /*! \brief Decrement this iterator by \a n
     */
    Derived & operator-=(difference_type n)
    {
      mCurrentRow -= n;
      Q_ASSERT(mCurrentRow >= 0);
      return *derivedThis();
    }

    /*! \brief Get a iterator from this one incremented by \a n
     */
    friend
    Derived operator+(const Derived & it, difference_type n)
    {
      Derived rit(it);
      rit += n;
      return rit;
    }

    /*! \brief Get a iterator from this one incremented by \a n
     */
    friend
    Derived operator+(difference_type n, const Derived & it)
    {
      return operator+(it, n);
    }

    /*! \brief Get a iterator from this one decremented by \a n
     */
    friend
    Derived operator-(const Derived & it, difference_type n)
    {
      Derived rit(it);
      rit -= n;
      return rit;
    }

    /*! \brief Get a iterator from this one decremented by \a n
     */
    friend
    Derived operator-(difference_type n, const Derived & it)
    {
      return operator-(it, n);
    }

    /*! \brief Get the distance between itearor \a a and \a b
     */
    friend
    difference_type operator-(const Derived & a, const Derived & b)
    {
      return a.currentRow() - b.currentRow();
    }

   private:

    Derived *derivedThis()
    {
      return reinterpret_cast<Derived*>(this);
    }

    int mCurrentRow = -1;
//     const Impl *mImpl;
//     const StlTableProxyImplementationInterface *mImpl;
//     const Impl & mImpl;
  };

  /*! \brief STL const iterator for StlTableProxy
   */
  template<typename EntityData, typename Impl>
  class StlTableProxyConstIterator : public StlTableProxyIteratorBase<EntityData, StlTableProxyConstIterator<EntityData, Impl> >
  {
   public:

    using ParentClass = StlTableProxyIteratorBase<EntityData, StlTableProxyConstIterator<EntityData, Impl> >;
    using ParentClass::ParentClass;
    using ParentClass::operator=;
    using ParentClass::currentRow;

    /*! \brief STL style value type
     */
    using value_type = typename ParentClass::value_type;

    /*! \brief STL reference
     */
    using reference = const value_type &;

//     /*! \brief STL reference
//      */
//     using reference = typename ParentClass::reference;

//     /*! \brief Const reference
//      */
//     using const_reference = typename ParentClass::const_reference;

    /*! \brief STL pointer
     */
    using pointer = typename ParentClass::pointer;

    /*! \brief STL style difference
     */
    using difference_type = typename ParentClass::difference_type;

    /*! \brief STL iterator category
     */
    using iterator_category = typename ParentClass::iterator_category;

    /*! \brief Construct a past the end iterator
     */
    StlTableProxyConstIterator() = default;

    /*! \brief Construct a iterator
     *
     * \pre \a row must be >= 0
     */
    StlTableProxyConstIterator(int row, const Impl * impl)
     : ParentClass(row),
       mImpl(impl)
    {
      Q_ASSERT(mImpl != nullptr);
    }

    /*! \brief Dereference this iterator
     */
    reference operator*() const
    {
      return mImpl->recordAt( currentRow() );
    }

   private:

    const Impl *mImpl = nullptr;
  };

  /*! \internal Dereference proxy for STL table proxy iterator
   */
  template<typename EntityData, typename Impl>
  class StlTableProxyIteratorDereferenceProxy
  {
   public:

    using value_type = typename std::remove_cv<EntityData>::type;
    using const_reference = const value_type &;

    StlTableProxyIteratorDereferenceProxy() = delete;

    StlTableProxyIteratorDereferenceProxy(const StlTableProxyIteratorDereferenceProxy &) = default;
    StlTableProxyIteratorDereferenceProxy & operator=(const StlTableProxyIteratorDereferenceProxy &) = default;

    StlTableProxyIteratorDereferenceProxy(StlTableProxyIteratorDereferenceProxy &&) = default;
    StlTableProxyIteratorDereferenceProxy & operator=(StlTableProxyIteratorDereferenceProxy &&) = default;

    StlTableProxyIteratorDereferenceProxy(int row, Impl *impl)
     : mCurrentRow(row),
       mImpl(impl)
    {
      Q_ASSERT(mCurrentRow >= 0);
      Q_ASSERT(mImpl != nullptr);
    }

    operator const_reference() const
    {
      Q_ASSERT(mImpl != nullptr);
      Q_ASSERT(mCurrentRow >= 0);
      Q_ASSERT(mCurrentRow < mImpl->rowCount());

      qDebug() << "DP - get[" << mCurrentRow << "] " << mImpl->recordAt(mCurrentRow).firstName();
      return mImpl->recordAt(mCurrentRow);
    }

    operator value_type &&()
    {
      Q_ASSERT(mImpl != nullptr);
      Q_ASSERT(mCurrentRow >= 0);
      Q_ASSERT(mCurrentRow < mImpl->rowCount());

      qDebug() << "DP - get&&[" << mCurrentRow << "] " << mImpl->recordAt(mCurrentRow).firstName();

      return std::forward( mImpl->recordAt(mCurrentRow) );
    }

    StlTableProxyIteratorDereferenceProxy operator=(const_reference data)
    {
      Q_ASSERT(mImpl != nullptr);
      Q_ASSERT(mCurrentRow >= 0);
      Q_ASSERT(mCurrentRow < mImpl->rowCount());

      qDebug() << "DP - set&[" << mCurrentRow << "] " << data.firstName();
      mImpl->setRecordAt(mCurrentRow, data);

      return *this;
    }

//     StlTableProxyIteratorDereferenceProxy operator=(value_type && data)
//     {
//       Q_ASSERT(mImpl != nullptr);
//       Q_ASSERT(mCurrentRow >= 0);
//       Q_ASSERT(mCurrentRow < mImpl->rowCount());
// 
//       qDebug() << "DP - set&&[" << mCurrentRow << "] " << data.firstName();
//       mImpl->setRecordAt(mCurrentRow, data);
// 
//       return *this;
//     }

   private:

    int mCurrentRow;
    Impl *mImpl;
  };

  template<typename EntityData, typename Impl>
  void swap(StlTableProxyIteratorDereferenceProxy<EntityData, Impl> a, StlTableProxyIteratorDereferenceProxy<EntityData, Impl> b)
  {
    using proxy = Mdt::Entity::StlTableProxyIteratorDereferenceProxy<EntityData, Impl>;
    using value_type = typename proxy::value_type;

    qDebug() << "Swap..";
    value_type va = a;
    value_type vb = b;
    b = va;
    a = vb;
    qDebug() << "Swap END";
  }

  /*! \brief STL iterator for StlTableProxy
   */
  template<typename EntityData, typename Impl>
  class StlTableProxyIterator : public StlTableProxyIteratorBase<EntityData, StlTableProxyIterator<EntityData, Impl> >
  {
   public:

    using ParentClass = StlTableProxyIteratorBase<EntityData, StlTableProxyIterator<EntityData, Impl> >;
    using ParentClass::ParentClass;
    using ParentClass::operator=;
    using ParentClass::currentRow;

    /*! \brief STL style value type
     */
    using value_type = typename ParentClass::value_type;

    /*! \brief STL reference
     */
    using reference = value_type &;

//     /*! \brief STL reference
//      */
//     using reference = typename ParentClass::reference;

//     /*! \brief Const reference
//      */
//     using const_reference = typename ParentClass::const_reference;

    /*! \brief STL pointer
     */
    using pointer = typename ParentClass::pointer;

    /*! \brief STL style difference
     */
    using difference_type = typename ParentClass::difference_type;

    /*! \brief STL iterator category
     */
    ///using iterator_category = std::output_iterator_tag;
    using iterator_category = typename ParentClass::iterator_category;

    /*! \brief Construct a past the end iterator
     */
    StlTableProxyIterator() = default;

    /*! \brief Construct a iterator
     *
     * \pre \a row must be >= 0
     */
    StlTableProxyIterator(int row, Impl * impl)
     : ParentClass(row),
       mImpl(impl)
    {
      Q_ASSERT(mImpl != nullptr);
    }

    /*! \brief Dereference this iterator
     */
    StlTableProxyIteratorDereferenceProxy<EntityData, Impl> operator*()
    {
      return StlTableProxyIteratorDereferenceProxy<EntityData, Impl>( currentRow(), mImpl );
    }

   private:

    Impl *mImpl = nullptr;
  };

}} // namespace Mdt{ namespace Entity{

// namespace std{
// 
//   template<typename EntityData, typename Impl>
//   void swap(Mdt::Entity::StlTableProxyIteratorDereferenceProxy<EntityData, Impl> & a, Mdt::Entity::StlTableProxyIteratorDereferenceProxy<EntityData, Impl> & b)
//   {
//     using proxy = Mdt::Entity::StlTableProxyIteratorDereferenceProxy<EntityData, Impl>;
//     using value_type = typename proxy::value_type;
// 
//     qDebug() << "Swap..";
//     value_type va = a;
//     value_type vb = b;
//     b = va;
//     a = vb;
//     qDebug() << "Swap END";
//   }
// 
// } // namespace std{

#endif // #ifndef MDT_ENTITY_STL_TABLE_PROXY_ITERATOR_H
