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
#ifndef MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_H
#define MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_H

#include "mdtSqlCsvImportTableMapping.h"
#include "mdtCsvFileInfo.h"
#include <QFileInfo>
#include <QByteArray>

/*! \brief Table mapping for SQL CSV file import
 */
class mdtSqlCsvFileImportTableMapping : public mdtSqlCsvImportTableMapping
{
 public:

  /*! \brief Set source CSV file
   *
   * Will also reset field mapping.
   *
   * \sa resetFieldMapping()
   */
  bool setSourceCsvFile(const QFileInfo & file, const QByteArray & fileEncoding, const mdtCsvParserSettings & settings);

 private:

  /*! \brief Reference CSV source info
   */
  mdtCsvSourceInfo & sourceTable()
  {
    return pvSourceTable;
  }

  /*! \brief Reference CSV source info (read only)
   */
  const mdtCsvSourceInfo & sourceTable() const
  {
    return pvSourceTable;
  }

  mdtCsvFileInfo pvSourceTable;
};

#endif // #ifndef MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_H