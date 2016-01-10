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
#ifndef MDT_SQL_CSV_IMPORT_TABLE_MAPPING_H
#define MDT_SQL_CSV_IMPORT_TABLE_MAPPING_H

#include "mdt/sql/copier/TableMapping.h"
#include "mdtCsvSourceInfo.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlDriverType.h"
#include <QSqlDatabase>
#include <QString>

/*! \brief Table mapping for SQL CSV import
 */
class mdtSqlCsvImportTableMapping : public mdt::sql::copier::TableMapping
{
 public:

  /*! \brief Constructor
   */
  mdtSqlCsvImportTableMapping() = default;

  /*! \brief Set destination table
   *
   * Will also reset field mapping.
   *
   * \sa resetFieldMapping()
   */
  bool setDestinationTable(const QString & tableName, const QSqlDatabase & db);

  /*! \brief Get source table name
   */
  QString sourceTableName() const
  {
    return sourceTable().sourceName();
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
    return sourceTable().fieldCount();
  }

  /*! \brief Get field count in destination table
    */
  int destinationTableFieldCount() const override
  {
    return pvDestinationTable.fieldCount();
  }

  /*! \brief Get list of field names of source table
   */
  QStringList getSourceFieldNameList() const
  {
    return sourceTable().getFieldNameList();
  }

  /*! \brief Get destination field name for given field mapping index
   *
   * \pre index must be in valid range.
   */
  QString destinationFieldName(int index) const;

  /*! \brief Get destination field type name for given field mapping index
   *
   * \pre index must be in valid range.
   */
  QString destinationFieldTypeName(int index) const;

  /*! \brief Generate field mapping by name
   *
   * Will first reset field mapping.
   *  Then, for each field in destination table,
   *  source field is defined by destination field name.
   */
  void generateFieldMappingByName();

 protected:

  // Permit only concrete classes to be copied
  mdtSqlCsvImportTableMapping(const mdtSqlCsvImportTableMapping &) = default;
  mdtSqlCsvImportTableMapping & operator=(const mdtSqlCsvImportTableMapping &) = default;

  /*! \brief Get field count of destination table
   *
   * Mainly used by resetFieldMapping()
   */
//   int destinationTableFieldCount() const
//   {
//     return pvDestinationTable.fieldCount();
//   }

  /*! \brief Reference CSV source info
   */
  virtual mdtCsvSourceInfo & sourceTable() = 0;

  /*! \brief Reference CSV source info (read only)
   */
  virtual const mdtCsvSourceInfo & sourceTable() const = 0;

 private:

  /*! \brief Set source field index for given field mapping
   */
  void setSourceFieldIndex(mdtSqlCopierFieldMapping & fm, const QString & sourceFieldName) override;

  /*! \brief Get field index of given field name in source table
   */
  int fetchSourceTableFieldIndexOf(const QString & fieldName) const override
  {
    return sourceTable().fieldIndex(fieldName);
  }

  /*! \brief Get field name for given fieldIndex in source table
   */
  QString fetchSourceTableFieldNameAt(int fieldIndex) const override
  {
    return sourceTable().fieldName(fieldIndex);
  }

  /*! \brief Get field type name for given fieldIndex in source table
   */
  QString fetchSourceTableFieldTypeNameAt(int fieldIndex) const override
  {
    return sourceTable().fieldTypeName(fieldIndex);
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
  bool areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const;

  /*! \brief Update CSV source format regarding destination table
   *
   * Will set CSV source format for given field map.
   * \note No check is done on CSV source data. Errors will be detected during import.
   */
  void updateCsvSourceFormat(mdtSqlCopierFieldMapping & fm);

  mdtSqlSchemaTable pvDestinationTable;
  QSqlDatabase pvDestinationDatabase;
};

#endif // #ifndef MDT_SQL_CSV_IMPORT_TABLE_MAPPING_H
