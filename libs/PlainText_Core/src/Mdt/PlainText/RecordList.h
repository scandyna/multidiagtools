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
#ifndef MDT_PLAIN_TEXT_RECORD_LIST_H
#define MDT_PLAIN_TEXT_RECORD_LIST_H

#include "Record.h"
#include <QVector>
#include <QVariant>
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace PlainText{

  /*! \brief RecordList is a list of Record
   */
  class RecordList
  {
   public:

    /*! \brief Construct a empty record list
     */
    RecordList() = default;

    /*! \brief Construct a record list from initializer lists
     */
    RecordList(std::initializer_list<Record> list);

    /*! \brief Copy construct a record list from \a other
     */
    RecordList(const RecordList & other) = default;

    /*! \brief Copy assign \a other to this record list
     */
    RecordList & operator=(const RecordList & other) = default;

    /*! \brief Move construct a record list from \a other
     */
    RecordList(RecordList && other) = default;

    /*! \brief Move assign \a other to this record list
     */
    RecordList & operator=(RecordList && other) = default;

    /*! \brief Append a record
     */
    void appendRecord(const Record & record);

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
    void setData(int row, int column, const QVariant & data);

    /*! \brief Get data for given row and column
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     * \pre \a column must be in valid range ( 0 <= column < columnCount(row) )
     */
    QVariant data(int row, int column) const
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
    QVariant value(int row, int column) const
    {
      return mRecordList.value(row).value(column);
    }

    /*! \brief Clear
     */
    void clear();

    /*! \brief Appends a row with \a record to this record list
     */
    RecordList & operator<<(const Record & record);

   private:

    QVector<Record> mRecordList;
  };

}} // namespace Mdt{ namespace PlainText{
Q_DECLARE_METATYPE(Mdt::PlainText::RecordList)

#endif // #ifndef MDT_PLAIN_TEXT_RECORD_LIST_H
