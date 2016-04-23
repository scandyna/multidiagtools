/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_COPIER_CSV_STRING_IMPORT_TABLE_MAPPING_H
#define MDT_SQL_COPIER_CSV_STRING_IMPORT_TABLE_MAPPING_H

#include "CsvImportTableMapping.h"
#include "mdtCsvStringInfo.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table mapping for SQL CSV string import
   */
  class CsvStringImportTableMapping : public CsvImportTableMapping
  {
  public:

    /*! \brief Set source CSV string
     *
     * Will also reset field mapping.
     *
     * \sa resetFieldMapping()
     */
    bool setSourceCsvString(const QString & csv, const mdtCsvParserSettings & settings);

    /*! \brief Get source table name
     */
    QString sourceTableName() const override
    {
      return "???";
    }

  private:

    /*! \brief Check if source CSV is set
     */
    bool sourceCsvIsSet() const override
    {
      return false;
    }

    /*! \brief Parse CSV header
     */
    bool parseSourceHeader() override;

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

    mdtCsvStringInfo pvSourceTable;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_STRING_IMPORT_TABLE_MAPPING_H
