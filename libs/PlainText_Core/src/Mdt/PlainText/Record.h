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
#ifndef MDT_PLAIN_TEXT_RECORD_H
#define MDT_PLAIN_TEXT_RECORD_H

#include <QVector>
#include <QVariant>
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace PlainText{

  /*! \brief Record contains a list of field data
   *
   * Record can, for example, hold the result of a parsed CSV line.
   */
  class Record
  {
   public:

    /*! \brief Construct a empty record
     */
    Record() = default;

    /*! \brief Construct a record from initializer list
     */
    Record(std::initializer_list<QVariant> list);

    /*! \brief Copy construct a record from \a other
     */
    Record(const Record & other) = default;

    /*! \brief Copy assign \a other to this record
     */
    Record & operator=(const Record & other) = default;

    /*! \brief Move construct a record from \a other
     */
    Record(Record && other) = default;

    /*! \brief Move assign \a other to this record
     */
    Record & operator=(Record && other) = default;

    /*! \brief Get count of columns in the record
     */
    int columnCount() const
    {
      return mRecord.count();
    }

    /*! \brief Append a column to this record
     */
    void appendColumn();

    /*! \brief Set column count
     *
     * If \a count is > current columnCount(),
     *  default constructed elements are added to the end.
     *  If \a count is < current columnCount(),
     *  elements are removed from the end.
     *
     * \pre \a count must be > 0
     */
    void setColumnCount(int count);

    /*! \brief Set data for given column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    void setData(int column, const QVariant & data);

    /*! \brief Get data for given column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    QVariant data(int column) const
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
    QVariant value(int column) const
    {
      return mRecord.value(column);
    }

    /*! \brief Clear
     */
    void clear();

    /*! \brief Appends a column with \a data to this record
     */
    Record & operator<<(const QVariant & data);

   private:

    QVector<QVariant> mRecord;
  };

}} // namespace Mdt{ namespace PlainText{
Q_DECLARE_METATYPE(Mdt::PlainText::Record)

#endif // #ifndef MDT_PLAIN_TEXT_RECORD_H
