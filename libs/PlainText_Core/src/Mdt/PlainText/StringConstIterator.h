/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_PLAIN_TEXT_STRING_CONST_ITERATOR_H
#define MDT_PLAIN_TEXT_STRING_CONST_ITERATOR_H

#include <QChar>
#include <QString>
#include <string>
#include <iterator>
#include <cstddef>

namespace Mdt{ namespace PlainText{

/*! \brief Iterator that acts on QString
 *
 * Iterator that can be used by parsers based on Boost.Spirit
 *  to be able to parse QString directly.
 *
 * Boost.Spirit handles wchar_t and std::wstring,
 *  but not QString.
 *  The trick here is when accessing
 *  the item of the iterator,
 *  QChar::unicode() is invoked.
 *  This way, Spirit sees wchar_t
 *  and can do needed comparisons.
 */
struct StringConstIterator
{
  static_assert(sizeof(wchar_t) >= 2, "wchar_t is < 16 bit");

  using value_type = wchar_t;
  using reference = const value_type &;
  using pointer = const value_type*;
  using difference_type = QString::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  /*! \brief Default constructor
   *
   * Construct a invalid iterator
   */
  StringConstIterator()
   : mIndex(nullptr)
  {
  }

  /*! \brief Constructor
   */
  StringConstIterator(QString::const_iterator it)
   : mIndex(it)
  {
    Q_ASSERT(mIndex != nullptr);
  }

  /*! \brief Assignement
   */
  StringConstIterator & operator=(const StringConstIterator & other)
  {
    mIndex = other.mIndex;
    return *this;
  }

  /*! \brief Assignement
   */
  StringConstIterator & operator=(QString::const_iterator & it)
  {
    mIndex = it;
    return *this;
  }

  /*! \brief Increment iterator (pre-increment)
   */
  StringConstIterator & operator++()
  {
    Q_ASSERT(mIndex != nullptr);
    ++mIndex;
    return *this;
  }

  /*! \brief Increment iterator (post-increment)
   */
  StringConstIterator operator++(int)
  {
    Q_ASSERT(mIndex != nullptr);
    StringConstIterator tmp(*this);
    ++*this;
    return tmp;
  }

  /*! \brief Decrement iterator (pre-decrement)
   */
  StringConstIterator & operator--()
  {
    Q_ASSERT(mIndex != nullptr);
    --mIndex;
    return *this;
  }

  /*! \brief Decrement iterator (post-decrement)
   */
  StringConstIterator operator--(int)
  {
    Q_ASSERT(mIndex != nullptr);
    StringConstIterator tmp(*this);
    --*this;
    return tmp;
  }

  /*! \brief Returns a iterator resulting from \a a + \a n
   */
  friend
  StringConstIterator operator+(const StringConstIterator & a, difference_type n)
  {
    Q_ASSERT(a.mIndex != nullptr);
    return StringConstIterator(a.mIndex + n);
  }

  /*! \brief Returns a iterator resulting from \a n + \a a
   */
  friend
  StringConstIterator operator+(difference_type n, const StringConstIterator & a)
  {
    Q_ASSERT(a.mIndex != nullptr);
    return StringConstIterator(a.mIndex + n);
  }

  /*! \brief Returns the difference resulting from \a b - \a a
   */
  friend
  difference_type operator-(const StringConstIterator & b, const StringConstIterator & a)
  {
    Q_ASSERT(a.mIndex != nullptr);
    Q_ASSERT(b.mIndex != nullptr);
    return b.mIndex - a.mIndex;
  }

  /*! \brief Returns a iterator that is rewind by n positions
   */
  StringConstIterator operator-(difference_type n) const
  {
    Q_ASSERT(mIndex != nullptr);
    return StringConstIterator(mIndex - n);
  }

  /*! \brief Advance iterator by n positions
   */
  StringConstIterator & operator+=(difference_type n)
  {
    Q_ASSERT(mIndex != nullptr);
    mIndex += n;
    return *this;
  }

  /*! \brief Rewind iterator by n positions
   */
  StringConstIterator & operator-=(difference_type n)
  {
    Q_ASSERT(mIndex != nullptr);
    mIndex -= n;
    return *this;
  }

  /*! \brief Get pointed value
   */
  value_type operator*() const
  {
    Q_ASSERT(mIndex != nullptr);
    return mIndex->unicode();
  }

  /*! \brief Get value by index
   */
  value_type operator[](difference_type i) const
  {
    Q_ASSERT(mIndex != nullptr);
    return mIndex[i].unicode();
  }

  /*! \brief Returns true if iterator a refers to same item than iterator b
   */
  friend
  bool operator==(const StringConstIterator & a, const StringConstIterator & b)
  {
    return (a.mIndex == b.mIndex);
  }

  /*! \brief Returns true if iterator a refers not to same item than iterator b
   */
  friend
  bool operator!=(const StringConstIterator & a, const StringConstIterator & b)
  {
    return !(a == b);
  }

  /*! \brief Returns true if iterator a refers to a element prior to item referenced by iterator b
   */
  friend
  bool operator<(const StringConstIterator & a, const StringConstIterator & b)
  {
    return (a.mIndex < b.mIndex);
  }

  /*! \brief Returns true if iterator a <= iterator b
   */
  friend
  bool operator<=(const StringConstIterator & a, const StringConstIterator & b)
  {
    return (a.mIndex <= b.mIndex);
  }

  /*! \brief Returns true if iterator a > iterator b
   */
  friend
  bool operator>(const StringConstIterator & a, const StringConstIterator & b)
  {
    return (a.mIndex > b.mIndex);
  }

  /*! \brief Returns true if iterator a >= iterator b
   */
  friend
  bool operator>=(const StringConstIterator & a, const StringConstIterator & b)
  {
    return (a.mIndex >= b.mIndex);
  }

 private:

  QString::const_iterator mIndex;
};

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_STRING_CONST_ITERATOR_H
