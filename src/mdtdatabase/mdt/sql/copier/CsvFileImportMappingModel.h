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
#ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_MAPPING_MODEL_H
#define MDT_SQL_COPIER_CSV_FILE_IMPORT_MAPPING_MODEL_H

#include "MappingModel.h"
#include "mdtExpected.h"
#include "CsvFileImportMapping.h"
#include "CsvFileImportTableMapping.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table model to access CSV file import mapping
   */
  class CsvFileImportMappingModel : public MappingModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    CsvFileImportMappingModel(QObject *parent = nullptr);

    // Copy disabled
    CsvFileImportMappingModel(const CsvFileImportMappingModel &) = delete;
    CsvFileImportMappingModel & operator=(const CsvFileImportMappingModel &) = delete;

    // Move disabled
    CsvFileImportMappingModel(CsvFileImportMappingModel &&) = delete;
    CsvFileImportMappingModel & operator=(CsvFileImportMappingModel &&) = delete;

    /*! \brief Set destination database
     *
     * Will also reset mapping.
     */
    mdtExpected<bool> setDestinationDatabase(const QSqlDatabase & db);

    /*! \brief Get destination database
     */
    QSqlDatabase destinationDatabase() const
    {
      return pvMapping.destinationDatabase();
    }

    /*! \brief Reset mapping
     */
    mdtExpected<bool> resetMapping();

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get source table name
     */
    QString sourceTableName(int row) const override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return pvMapping.sourceTableName(row);
    }

    /*! \brief Get destination table name
     */
    QString destinationTableName(int row) const override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return pvMapping.destinationTableName(row);
    }

    /*! \brief Get table mapping state at given row
     *
     * \pre row must be in a valid range
     */
    TableMapping::MappingState tableMappingState(int row) const override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return pvMapping.tableMapping(row)->mappingState();
    }

    /*! \brief Get table mapping at given index
     *
     * If modifications are made to returned table mapping,
     *  tableMappingUpdated() must be called, so that modifications can be displayed to the user.
     *
     * \pre row must be in a valid range
     */
    const std::shared_ptr<CsvFileImportTableMapping> tableMapping(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return pvMapping.tableMapping(row);
    }

    /*! \brief Tell the model that a table mapping was updated
     *
     * \pre row must be in a valid range
     */
    void tableMappingUpdated(int row);

   private:

    CsvFileImportMapping pvMapping;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_MAPPING_MODEL_H
