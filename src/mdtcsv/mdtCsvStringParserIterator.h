/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CSV_STRING_PARSER_ITERATOR_H
#define MDT_CSV_STRING_PARSER_ITERATOR_H

#include <QChar>
#include <QString>
#include <iterator>
#include <cstddef>

//#include <QDebug>

/*! \brief Iterator that acts on QString
 *
 * This iterator was made for mdtCsvStringParser
 *  to be able to parse QString directly.
 *
 * Parsing is done by using Boost.Spirit,
 *  witch handles wchar_t and std::wstring,
 *  but not QString.
 *  The trick here is when accessing
 *  the item of the iterator,
 *  QChar::unicode() is invoked.
 *  This way, Spirit sees wchar_t
 *  and can do needed comparisons.
 *
 * \todo Should be renamed mdtCsvStringParserIterator (also rename the header and #ifndef)
 */
struct mdtCsvStringParserIterator
{
  static_assert(sizeof(wchar_t) >= 2, "wchar_t is < 16 bit");

  typedef wchar_t value_type;
  typedef std::ptrdiff_t difference_type;
  typedef const value_type & reference;
  typedef const value_type* pointer;
  typedef std::random_access_iterator_tag iterator_category;

  /*! \brief Default constructor
   *
   * Construct a invalid iterator
   */
  mdtCsvStringParserIterator()
   : index(nullptr)
  {
  }

  /*! \brief Constructor
   */
  mdtCsvStringParserIterator(QString::const_iterator it)
   : index(it)
  {
    Q_ASSERT(index != nullptr);
  }

  /*! \brief Assignement
   */
  mdtCsvStringParserIterator & operator=(const mdtCsvStringParserIterator & other)
  {
    index = other.index;
    return *this;
  }

  /*! \brief Assignement
   */
  mdtCsvStringParserIterator & operator=(QString::const_iterator & it)
  {
    index = it;
    return *this;
  }

  /*! \brief Increment iterator (pre-increment)
   */
  mdtCsvStringParserIterator & operator++()
  {
    Q_ASSERT(index != nullptr);
    ++index;
    return *this;
  }

  /*! \brief Increment iterator (post-increment)
   */
  mdtCsvStringParserIterator operator++(int)
  {
    Q_ASSERT(index != nullptr);
    mdtCsvStringParserIterator tmp(*this);
    ++*this;
    return tmp;
  }

  /*! \brief Decrement iterator (pre-decrement)
   */
  mdtCsvStringParserIterator & operator--()
  {
    Q_ASSERT(index != nullptr);
    --index;
    return *this;
  }

  /*! \brief Decrement iterator (post-decrement)
   */
  mdtCsvStringParserIterator operator--(int)
  {
    Q_ASSERT(index != nullptr);
    mdtCsvStringParserIterator tmp(*this);
    --*this;
    return tmp;
  }

  /*! \brief Returns a iterator that is advanced by n positions
   */
  mdtCsvStringParserIterator operator+(difference_type n) const
  {
    Q_ASSERT(index != nullptr);
    return mdtCsvStringParserIterator(index + n);
  }

  /*! \brief Returns a iterator that is rewind by n positions
   */
  mdtCsvStringParserIterator operator-(difference_type n) const
  {
    Q_ASSERT(index != nullptr);
    return mdtCsvStringParserIterator(index - n);
  }

  /*! \brief Advance iterator by n positions
   */
  mdtCsvStringParserIterator & operator+=(difference_type n)
  {
    Q_ASSERT(index != nullptr);
    index += n;
    return *this;
  }

  /*! \brief Rewind iterator by n positions
   */
  mdtCsvStringParserIterator & operator-=(difference_type n)
  {
    Q_ASSERT(index != nullptr);
    index -= n;
    return *this;
  }

  /*! \brief Get pointed value
   */
  value_type operator*() const
  {
    Q_ASSERT(index != nullptr);
    return index->unicode();
  }

  /*! \brief Get value by index
   */
  value_type operator[](difference_type i) const
  {
    Q_ASSERT(index != nullptr);
    return index[i].unicode();
  }

//   /*! \brief Get a pointer to value
//    */
//   pointer operator->() const
//   {
//     Q_ASSERT(index != nullptr);
//     return &**this;
//   }

  /*! \brief Returns true if iterator a refers to same item than iterator b
   */
  friend
  bool operator==(const mdtCsvStringParserIterator & a, const mdtCsvStringParserIterator & b)
  {
    return (a.index == b.index);
  }

  /*! \brief Returns true if iterator a refers not to same item than iterator b
   */
  friend
  bool operator!=(const mdtCsvStringParserIterator & a, const mdtCsvStringParserIterator & b)
  {
    return !(a == b);
  }

  /*! \brief Returns true if iterator a refers to a element prior to item referenced by iterator b
   */
  friend
  bool operator<(const mdtCsvStringParserIterator & a, const mdtCsvStringParserIterator & b)
  {
    return (a.index < b.index);
  }

  /*! \brief Returns true if iterator a <= iterator b
   */
  friend
  bool operator<=(const mdtCsvStringParserIterator & a, const mdtCsvStringParserIterator & b)
  {
    return (a.index <= b.index);
  }

  /*! \brief Returns true if iterator a > iterator b
   */
  friend
  bool operator>(const mdtCsvStringParserIterator & a, const mdtCsvStringParserIterator & b)
  {
    return (a.index > b.index);
  }

  /*! \brief Returns true if iterator a >= iterator b
   */
  friend
  bool operator>=(const mdtCsvStringParserIterator & a, const mdtCsvStringParserIterator & b)
  {
    return (a.index >= b.index);
  }

 private:

  QString::const_iterator index;
  mutable value_type value;
};

#endif // #ifndef MDT_CSV_STRING_PARSER_ITERATOR_H
