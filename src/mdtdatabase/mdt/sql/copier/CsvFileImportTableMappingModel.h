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
#ifndef MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_MODEL_H
#define MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_MODEL_H

#include "TableMappingModel.h"
#include "CsvFileImportTableMapping.h"
#include "mdt/csv/RecordFormat.h"
#include "mdtError.h"
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>
#include <QString>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table model to access SQL CSV file import table mapping
   *
   * \sa CsvFileImportTableMapping
   */
  class CsvFileImportTableMappingModel : public TableMappingModel
  {
  Q_OBJECT

  public:

    /*! \brief Constructor
     */
    CsvFileImportTableMappingModel(QObject *parent = nullptr);

    /*! \brief Set source CSV settings
     *
     * Will also reset field mapping.
     *
     * \pre settings must be valid
     */
    bool setSourceCsvSettings(const mdtCsvParserSettings & settings);

    /*! \brief Set source CSV file
     *
     * Will also reset field mapping.
     *
     * \pre sourceCsvSettings must be valid
     */
    bool setSourceFile(const QFileInfo & file, const QByteArray & fileEncoding);

    /*! \brief Set source CSV record format
     *
     * \pre format's field count must be the same as source CSV field count
     */
    void setSourceRecordFormat(const mdt::csv::RecordFormat & format);

    /*! \brief Get source CSV record format
     */
    mdt::csv::RecordFormat sourceRecordFormat() const;

    /*! \brief Set destination database
     *
     * Will also clear mapping
     */
    void setDestinationDatabase(const QSqlDatabase & db);

    /*! \brief Set destination table
     *
     * Will also reset field mapping.
     */
    bool setDestinationTable(const QString & tableName);

    /*! \brief Set mapping
     */
    void setMapping(const std::shared_ptr<CsvFileImportTableMapping> & m);

    /*! \brief Get table mapping
     */
    std::shared_ptr<CsvFileImportTableMapping> mapping() const
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

    std::shared_ptr<CsvFileImportTableMapping> pvMapping;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_MODEL_H
