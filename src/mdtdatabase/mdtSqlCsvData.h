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
#ifndef MDT_SQL_CSV_DATA_H
#define MDT_SQL_CSV_DATA_H

#include "mdtCsvData.h"
#include "mdtCsvRecordFormat.h"
#include "mdtExpected.h"
#include "mdtSqlFieldType.h"
#include <QSqlRecord>
#include <QMetaType>

/*! \brief Provide some helper functions for data conversion between SQL and CSV
 */
namespace mdtSqlCsvData
{
  /*! \brief Get CSV field type from mdtSqlFieldType
   */
  QMetaType::Type csvFieldTypeFromMdtSqlFieldType(mdtSqlFieldType::Type sqlType);

  /*! \brief Get a CSV record from qiven QSqlRecord
   */
  mdtCsvRecord csvRecordFromQsqlRecord(const QSqlRecord & sqlRecord);

  /*! \brief Get a CSV record format based on given QSqlRecord
   */
  mdtCsvRecordFormat csvRecordFormatFromQsqlRecord(const QSqlRecord & sqlRecord);

  /*! \brief Fill given QSqlRecord with given CSV record
   *
   * Because a QSqlRecord has more attributes than a CSV record,
   *  its not possible to build it, but only to fill a existing one.
   *
   * \return A bool (with true value) or a mdtError if a conversion fails.
   * \pre csvRecord and sqlRecord must contain the same count of fields.
   */
  mdtExpected<bool> fillQSqlRecord(QSqlRecord & sqlRecord, mdtCsvRecord csvRecord);
}

#endif // #ifndef MDT_SQL_CSV_DATA_H
