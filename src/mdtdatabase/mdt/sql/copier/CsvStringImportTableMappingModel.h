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
#ifndef MDT_SQL_CSV_STRING_IMPORT_TABLE_MAPPING_MODEL_H
#define MDT_SQL_CSV_STRING_IMPORT_TABLE_MAPPING_MODEL_H

#include "TableMappingModel.h"
#include "CsvStringImportTableMapping.h"
#include "mdtError.h"
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>
#include <QString>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table model to access SQL CSV string import table mapping
   *
   * \sa mdtSqlCsvStringImportTableMapping
   */
  class CsvStringImportTableMappingModel : public TableMappingModel
  {
  Q_OBJECT

  public:

    /*! \brief Constructor
     */
    CsvStringImportTableMappingModel(QObject *parent = nullptr);

    /*! \brief Set source CSV string
     *
     * Will also reset field mapping.
     */
    bool setSourceCsvString(const QString & csv, const mdtCsvParserSettings & settings);

    /*! \brief Set destination table
     *
     * Will also reset field mapping.
     */
    bool setDestinationTable(const QString & tableName, const QSqlDatabase & db);

    /*! \brief Get table mapping
     */
    std::shared_ptr<CsvStringImportTableMapping> mapping() const
    {
      return pvMapping;
    }

  private:

    /*! \brief Reference internal table mapping
     */
    const std::shared_ptr<const TableMapping> mappingBase() const override
    {
      return pvMapping;
    }

    /*! \brief Reference internal table mapping
     */
    const std::shared_ptr<TableMapping> mappingBase() override
    {
      return pvMapping;
    }

    std::shared_ptr<CsvStringImportTableMapping> pvMapping;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_CSV_STRING_IMPORT_TABLE_MAPPING_MODEL_H
