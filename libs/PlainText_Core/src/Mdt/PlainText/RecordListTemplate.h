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
#ifndef MDT_PLAIN_TEXT_RECORD_LIST_TEMPLATE_H
#define MDT_PLAIN_TEXT_RECORD_LIST_TEMPLATE_H

#include <QVector>
#include <initializer_list>

namespace Mdt{ namespace PlainText{

  /*! \brief RecordListTemplate is a list of RecordTemplate
   *
   * \sa RecordList
   * \sa StringRecordList
   */
  template<typename RecordType, typename T>
  class RecordListTemplate
  {
   public:

    /*! \brief STL iterator
     */
    using iterator = typename QVector<RecordType>::iterator;

    /*! \brief STL const iterator
     */
    using const_iterator = typename QVector<RecordType>::const_iterator;

    /*! \brief Value type for STL compatibility
     */
    using value_type = typename QVector<RecordType>::value_type;

    /*! \brief Reference type for STL compatibility
     */
    using reference = typename QVector<RecordType>::reference;

    /*! \brief Const reference type for STL compatibility
     */
    using const_reference = typename QVector<RecordType>::const_reference;

    /*! \brief Size type for STL compatibility
     */
    using size_type = typename QVector<RecordType>::size_type;

    /*! \brief Pointer type for STL compatibility
     */
    using pointer = typename QVector<RecordType>::pointer;

    /*! \brief Const pointer type for STL compatibility
     */
    using const_pointer = typename QVector<RecordType>::const_pointer;

    /*! \brief Difference type for STL compatibility
     */
    using difference_type = typename QVector<RecordType>::difference_type;

    /*! \brief Construct a empty record list
     */
    RecordListTemplate() = default;

    /*! \brief Construct a record list from initializer lists
     */
    RecordListTemplate(std::initializer_list<RecordType> list)
     : mRecordList(list)
    {
    }

    /*! \brief Copy construct a record list from \a other
     */
    RecordListTemplate(const RecordListTemplate & other) = default;

    /*! \brief Copy assign \a other to this record list
     */
    RecordListTemplate & operator=(const RecordListTemplate & other) = default;

    /*! \brief Move construct a record list from \a other
     */
    RecordListTemplate(RecordListTemplate && other) = default;

    /*! \brief Move assign \a other to this record list
     */
    RecordListTemplate & operator=(RecordListTemplate && other) = default;

    /*! \brief Append a record
     */
    void appendRecord(const RecordType & record)
    {
      mRecordList.append(record);
    }

    /*! \brief Append record to this record list
     */
    void push_back(const RecordType & record)
    {
      mRecordList.push_back(record);
    }

    /*! \brief Insert record to this record list
     *
     * Inserts \a record in front of the item pointed to by the iterator \a before.
     *  Returns an iterator pointing at the inserted item.
     */
    iterator insert(iterator before, const RecordType & record)
    {
      return mRecordList.insert(before, record);
    }

    /*! \brief Check if this record list is empty
     */
    bool isEmpty() const
    {
      return mRecordList.isEmpty();
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return mRecordList.count();
    }

    /*! \brief Get column count for given row
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     */
    int columnCount(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mRecordList.at(row).columnCount();
    }

    /*! \brief Set data for given row and column
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     * \pre \a column must be in valid range ( 0 <= column < columnCount(row) )
     */
    void setData(int row, int column, const T & data)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount(row));
      mRecordList[row].setData(column, data);
    }

    /*! \brief Get data for given row and column
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     * \pre \a column must be in valid range ( 0 <= column < columnCount(row) )
     */
    T data(int row, int column) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount(row));
      return mRecordList.at(row).data(column);
    }

    /*! \brief Get value for given row and column
     *
     * If \a row or \a column is out of range,
     *  a default constructed value is returned
     */
    T value(int row, int column) const
    {
      return mRecordList.value(row).value(column);
    }

    /*! \brief Clear
     */
    void clear()
    {
      mRecordList.clear();
    }

    /*! \brief Appends a row with \a record to this record list
     */
    RecordListTemplate & operator<<(const RecordType & record)
    {
      mRecordList << record;
      return *this;
    }

    /*! \brief Get an STL-style iterator pointing to the first item in the record list
     */
    iterator begin()
    {
      return mRecordList.begin();
    }

    /*! \brief Get an STL-style iterator pointing past the last item in the record list
     */
    iterator end()
    {
      return mRecordList.end();
    }

    /*! \brief Get an STL-style const iterator pointing to the first item in the record list
     */
    const_iterator begin() const
    {
      return mRecordList.cbegin();
    }

    /*! \brief Get an STL-style const iterator pointing past the last item in the record list
     */
    const_iterator end() const
    {
      return mRecordList.cend();
    }

    /*! \brief Get an STL-style const iterator pointing to the first item in the record list
     */
    const_iterator cbegin() const
    {
      return mRecordList.cbegin();
    }

    /*! \brief Get an STL-style const iterator pointing past the last item in the record list
     */
    const_iterator cend() const
    {
      return mRecordList.cend();
    }

    /*! \brief Removes all the items from begin up to (but not including) end
     *
     * Returns an iterator to the same item that end referred to before the call.
     */
    iterator erase(iterator begin, iterator end)
    {
      return mRecordList.erase(begin, end);
    }

   private:

    QVector<RecordType> mRecordList;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_RECORD_LIST_TEMPLATE_H
