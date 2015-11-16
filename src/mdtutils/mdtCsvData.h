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

#include <QVector>
#include <string>

/*! \brief CSV record of data
 *
 * Container for a CSV record (i.e. a row, or a line of data)
 */
struct mdtCsvRecord
{
  /*! \brief Raw record data container
   *
   * \note It was choosen to use a QVector because it offers copy on write
   */
  QVector<std::string> rawColumnDataList;

  /*! \brief Clear
   */
  void clear()
  {
    rawColumnDataList.clear();
  }
};

#endif // #ifndef MDT_CSV_DATA_H
