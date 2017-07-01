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
#ifndef MDT_PLAIN_TEXT_RECORD_TEMPLATE_H
#define MDT_PLAIN_TEXT_RECORD_TEMPLATE_H

#include <QVector>
#include <initializer_list>

namespace Mdt{ namespace PlainText{

  /*! \brief RecordTemplate contains a list of field data
   *
   * \sa Record
   * \sa StringRecord
   */
  template<typename T>
  class RecordTemplate
  {
   public:

    /*! \brief STL iterator
     */
    using iterator = typename QVector<T>::iterator;

    /*! \brief STL const iterator
     */
    using const_iterator = typename QVector<T>::const_iterator;

    /*! \brief Value type for STL compatibility
     */
    using value_type = typename QVector<T>::value_type;

    /*! \brief Reference type for STL compatibility
     */
    using reference = typename QVector<T>::reference;

    /*! \brief Const reference type for STL compatibility
     */
    using const_reference = typename QVector<T>::const_reference;

    /*! \brief Size type for STL compatibility
     */
    using size_type = typename QVector<T>::size_type;

    /*! \brief Pointer type for STL compatibility
     */
    using pointer = typename QVector<T>::pointer;

    /*! \brief Const pointer type for STL compatibility
     */
    using const_pointer = typename QVector<T>::const_pointer;

    /*! \brief Difference type for STL compatibility
     */
    using difference_type = typename QVector<T>::difference_type;

    /*! \brief Construct a empty record
     */
    RecordTemplate() = default;

    /*! \brief Construct a record from initializer list
     */
    RecordTemplate(std::initializer_list<T> list)
     : mRecord(list)
    {
    }

    /*! \brief Copy construct a record from \a other
     */
    RecordTemplate(const RecordTemplate & other) = default;

    /*! \brief Copy assign \a other to this record
     */
    RecordTemplate & operator=(const RecordTemplate & other) = default;

    /*! \brief Move construct a record from \a other
     */
    RecordTemplate(RecordTemplate && other) = default;

    /*! \brief Move assign \a other to this record
     */
    RecordTemplate & operator=(RecordTemplate && other) = default;

    /*! \brief Get count of columns in the record
     */
    int columnCount() const
    {
      return mRecord.count();
    }

    /*! \brief Append a column to this record
     */
    void appendColumn(const T & data = T())
    {
      mRecord.append(data);
    }

    /*! \brief Set column count
     *
     * If \a count is > current columnCount(),
     *  default constructed elements are added to the end.
     *  If \a count is < current columnCount(),
     *  elements are removed from the end.
     *
     * \pre \a count must be > 0
     */
    void setColumnCount(int count)
    {
      Q_ASSERT(count > 0);
      mRecord.resize(count);
    }

    /*! \brief Append data to this record
     */
    void push_back(const T & data)
    {
      mRecord.push_back(data);
    }

    /*! \brief Insert data to this record
     *
     * Inserts \a data in front of the item pointed to by the iterator \a before.
     *  Returns an iterator pointing at the inserted item.
     */
    iterator insert(iterator before, const T & data)
    {
      return mRecord.insert(before, data);
    }

    /*! \brief Set data for given column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    void setData(int column, const T & data)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      mRecord[column] = data;
    }

    /*! \brief Get data for given column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    T data(int column) const
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());
      return mRecord.at(column);
    }

    /*! \brief Get value for given column
     *
     * If \a column is out of range,
     *  a default constructed value is returned
     */
    T value(int column) const
    {
      return mRecord.value(column);
    }

    /*! \brief Clear
     */
    void clear()
    {
      mRecord.clear();
    }

    /*! \brief Appends a column with \a data to this record
     */
    RecordTemplate & operator<<(const T & data)
    {
      mRecord << data;
      return *this;
    }

    /*! \brief Get an STL-style iterator pointing to the first item in the record
     */
    iterator begin()
    {
      return mRecord.begin();
    }

    /*! \brief Get an STL-style iterator pointing past the last item in the record
     */
    iterator end()
    {
      return mRecord.end();
    }

    /*! \brief Get an STL-style const iterator pointing to the first item in the record
     */
    const_iterator cbegin() const
    {
      return mRecord.cbegin();
    }

    /*! \brief Get an STL-style const iterator pointing past the last item in the record
     */
    const_iterator cend() const
    {
      return mRecord.cend();
    }

   private:

    QVector<T> mRecord;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_RECORD_TEMPLATE_H
