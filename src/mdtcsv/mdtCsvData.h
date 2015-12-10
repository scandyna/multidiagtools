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
#ifndef MDT_CSV_DATA_H
#define MDT_CSV_DATA_H

#include <QMetaType>
#include <QVector>
#include <QString>

#include <QVariant>

#include <string>

/*! \brief CSV record of data template
 *
 * Container for a CSV record (i.e. a row, or a line of data)
 *
 * \tparam T Type for column data
 * \note Some part of this API documentation refers to following standards:
 *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
 *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
 */
template <typename T>
struct mdtCsvRecordTemplate
{
  /*! \brief Constructor
   */
  mdtCsvRecordTemplate()
   : pvErrorOccured(false)
  {
  }

  /*! \brief Get error flag
   *
   * This flag is set by worker objects,
   *  such as CSV parser, when a error occured.
   *  The error is not stored in the record,
   *  but must be retrieved from worker.
   */
  bool errorOccured() const
  {
    return pvErrorOccured;
  }

  /*! \brief Set error flag
   */
  void setErrorOccured()
  {
    pvErrorOccured = true;
  }

  /*! \brief Record data container
   *
   * \note It was choosen to use a QVector because it offers copy on write
   */
  QVector<T> columnDataList;

  /*! \brief Get count of columns in the record
   */
  int count() const
  {
    return columnDataList.size();
  }

  /*! \brief Clear
   *
   * Will also clear error flag
   */
  void clear()
  {
    columnDataList.clear();
    pvErrorOccured = false;
  }

 private:

  bool pvErrorOccured;
};

/*! \brief CSV record of data
 *
 * Container for a CSV record (i.e. a row, or a line of data)
 */
typedef mdtCsvRecordTemplate<QVariant> mdtCsvRecord;

/*! \brief CSV data
 *
 * CSV data is a list of mdtCsvRecord
 *
 * \note Some part of this API documentation refers to following standards:
 *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
 *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
 */
struct mdtCsvData
{
  /*! \brief Constructor
   */
  mdtCsvData()
   : pvErrorOccured(false)
  {
  }

  /*! \brief Get error flag
   *
   * This flag is set by worker objects,
   *  such as CSV parser, when a error occured.
   *  The error is not stored in the record,
   *  but must be retrieved from worker.
   */
  bool errorOccured() const
  {
    return pvErrorOccured;
  }

  /*! \brief Set error flag
   */
  void setErrorOccured()
  {
    pvErrorOccured = true;
  }

  /*! \brief Record list
   *
   * \note It was choosen to use a QVector because it offers copy on write
   */
  QVector<mdtCsvRecord> recordList;

  /*! \brief Add a record
   */
  void addRecord(const mdtCsvRecord & record)
  {
    recordList.append(record);
  }

  /*! \brief Get records count
   */
  int recordCount() const
  {
    return recordList.size();
  }

  /*! \brief Clear
   */
  void clear()
  {
    recordList.clear();
    pvErrorOccured = false;
  }

 private:

  bool pvErrorOccured;
};
Q_DECLARE_METATYPE(mdtCsvData)

#endif // #ifndef MDT_CSV_DATA_H
