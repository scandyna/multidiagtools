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
#ifndef MDT_CSV_TABLE_VIEW_DATA_MAPPER_H
#define MDT_CSV_TABLE_VIEW_DATA_MAPPER_H

#include "mdtCsvData.h"
#include <QPointer>

class QTableView;

/*! \brief Helper class to get data in a model
 *
 * This class is used to export data that are displayed in a QTableView
 *  as the user sees it.
 *  For this, we must take in acount that columns (and maybe rows)
 *  are hidden, colums moved, rows moved (typically sorted).
 */
class mdtCsvTableViewDataMapper
{
 public:

  /*! \brief Default constructor
   */
  mdtCsvTableViewDataMapper();

  /*! \brief Set table view
   *
   * \pre view must not be a null pointer
   * \pre view must have a model
   * \pre view must have horizontal and vertical header views
   */
  void setView(QTableView *view);

  /*! \brief Reset
   */
  void reset();

  /*! \brief Move to next row
   *
   * Returns true if next row is not out of bound.
   *  Data is fetched from model if needed.
   *
   * \pre Table view must be set before.
   */
  bool next();

  /*! \brief Get current row
   */
  int currentRow() const
  {
    return pvCurrentRow;
  }

  /*! \brief Get a CSV record for horizontal header
   *
   * \note The CSV record is rebuilt at each call.
   * \pre Table view must be set before.
   */
  mdtCsvRecord getHeaderRecord() const;

  /*! \brief Get a CSV record for current row
   *
   * \note The CSV record is rebuilt at each call.
   * \pre Table view must be set before.
   * \pre Must be called only after next() returned true
   */
  mdtCsvRecord getCurrentRecord() const;

 private:

  int pvCurrentRow;
  QPointer<QTableView> pvView;
};

#endif // #ifndef MDT_CSV_TABLE_VIEW_DATA_MAPPER_H
