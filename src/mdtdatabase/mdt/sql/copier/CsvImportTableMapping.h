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
#ifndef MDT_SQL_COPIER_CSV_IMPORT_TABLE_MAPPING_H
#define MDT_SQL_COPIER_CSV_IMPORT_TABLE_MAPPING_H

#include "TableMapping.h"
#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include "mdt/csv/RecordFormat.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlDriverType.h"
#include <QSqlDatabase>
#include <QString>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table mapping for SQL CSV import
   */
  class CsvImportTableMapping : public TableMapping
  {
  public:

    /*! \brief Constructor
     */
    CsvImportTableMapping() = default;

    // Copy disabled
    CsvImportTableMapping(const CsvImportTableMapping &) = delete;
    CsvImportTableMapping & operator=(const CsvImportTableMapping &) = delete;
    // Move disabled
    CsvImportTableMapping(CsvImportTableMapping &&) = delete;
    CsvImportTableMapping & operator=(CsvImportTableMapping &&) = delete;

    /*! \brief Set source CSV settings
     *
     * If a CSV source was allready set,
     *  its header will be parsed again.
     *  In this case, field mapping will also be reset.
     *
     * \pre settings must be valid
     * \sa resetFieldMapping()
     */
    bool setSourceCsvSettings(const mdtCsvParserSettings & settings);

    /*! \brief Get source CSV settings
     */
    mdtCsvParserSettings sourceCsvSettings() const
    {
      return pvSourceCsvSettings;
    }

    /*! \brief Set source record format
     *
     * \pre format's fieldCount must be the same as sourceTableFieldCount
     */
    void setSourceRecordFormat(const mdt::csv::RecordFormat & format);

    /*! \brief Get source record format
     */
    mdt::csv::RecordFormat sourceRecordFormat() const
    {
      return pvSourceFormat;
    }

    /*! \brief Set destination table
     *
     * Will also reset field mapping.
     *
     * \sa resetFieldMapping()
     */
    bool setDestinationTable(const QString & tableName, const QSqlDatabase & db);

    /*! \brief Get destination database
     */
    QSqlDatabase destinationDatabase() const override
    {
      return pvDestinationDatabase;
    }

    /*! \brief Get destination table name
     */
    QString destinationTableName() const
    {
      return pvDestinationTable.tableName();
    }

    /*! \brief Get field count in source table
     */
    int sourceTableFieldCount() const override
    {
      return pvSourceHeader.count();
    }

    /*! \brief Get field count in destination table
     */
    int destinationTableFieldCount() const override
    {
      return pvDestinationTable.fieldCount();
    }

  protected:

    /*! \brief Check if source CSV is set
     */
    virtual bool sourceCsvIsSet() const = 0;

    /*! \brief Parse source CSV header
     *
     * \pre sourceCsvSettings must be valid
     */
    virtual bool parseSourceHeader() = 0;

    /*! \brief Set source header
     *
     * This will also reset source CSV format
     *  (all fields are also of type String)
     */
    void setSourceHeader(const mdtCsvRecord & header)
    {
      pvSourceHeader = header;
      pvSourceFormat.setFieldCount(header.count(), QMetaType::QString);
    }

  private:

    /*! \brief Get a list of available field names in source table
     */
    QStringList fetchSourceTableFieldNameList() const override;

    /*! \brief Get a list of available field names in destination table
     */
    QStringList fetchDestinationTableFieldNameList() const override
    {
      return pvDestinationTable.getFieldNameList();
    }

    /*! \brief Get field index of given field name in source table
     */
    int fetchSourceTableFieldIndexOf(const QString & fieldName) const override
    {
      return pvSourceHeader.columnDataList.indexOf(fieldName);
    }

    /*! \brief Get field name for given fieldIndex in source table
     */
    QString fetchSourceTableFieldNameAt(int fieldIndex) const override
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < pvSourceHeader.count());
      return pvSourceHeader.columnDataList.at(fieldIndex).toString();
    }

    /*! \brief Get field type name for given fieldIndex in source table
     */
    QString fetchSourceTableFieldTypeNameAt(int fieldIndex) const override;

    /*! \brief Get field index of given field name in destination table
     */
    int fetchDestinationTableFieldIndexOf(const QString & fieldName) const override
    {
      return pvDestinationTable.fieldIndex(fieldName);
    }

    /*! \brief Get field name for given fieldIndex in destination table
     */
    QString fetchDestinationTableFieldNameAt(int fieldIndex) const override
    {
      return pvDestinationTable.fieldName(fieldIndex);
    }

    /*! \brief Get field type name for given fieldIndex in destination table
     */
    QString fetchDestinationTableFieldTypeNameAt(int fieldIndex) const override;

    /*! \brief Check if field is part of a key for given field index in destination table
     */
    FieldKeyType fetchDestinationTableFieldKeyType(int fieldIndex) const override;

    /*! \brief Check if source field is compatible with destination field
    */
    bool areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const override;

    /*! \brief Update CSV source format regarding destination table
    *
    * Will set CSV source format for given field map.
    * \note No check is done on CSV source data. Errors will be detected during import.
    */
  //   void updateCsvSourceFormat(mdtSqlCopierFieldMapping & fm);

    mdtCsvParserSettings pvSourceCsvSettings;
    mdtCsvRecord pvSourceHeader;
    mdt::csv::RecordFormat pvSourceFormat;
    mdtSqlTableSchema pvDestinationTable;
    QSqlDatabase pvDestinationDatabase;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_IMPORT_TABLE_MAPPING_H
