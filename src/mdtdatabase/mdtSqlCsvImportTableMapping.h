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
#ifndef MDT_SQL_CSV_IMPORT_TABLE_MAPPING_H
#define MDT_SQL_CSV_IMPORT_TABLE_MAPPING_H

#include "mdtSqlCopierTableMapping.h"
#include "mdtCsvSourceInfo.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlDriverType.h"
#include <QSqlDatabase>
#include <QString>

/*! \brief Table mapping for SQL CSV import
 */
class mdtSqlCsvImportTableMapping : public mdtSqlCopierTableMapping
{
 public:

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

  /*! \brief Set source field for given field mapping index
   *
   * If source field name is empty,
   *  the source field will be removed for given index.
   *
   * \pre index must be in a valid range
   */
  void setSourceField(int index, const QString & fieldName);

  /*! \brief Get source field name for given field mapping index
   *
   * \pre index must be in valid range.
   */
  QString sourceFieldName(int index) const;

  /*! \brief Get source field type name for given field mapping index
   *
   * \pre index must be in valid range.
   */
  QString sourceFieldTypeName(int index) const;

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

  /*! \brief Get field count of destination table
   *
   * Mainly used by resetFieldMapping()
   */
  int destinationTableFieldCount() const
  {
    return pvDestinationTable.fieldCount();
  }

  /*! \brief Reference CSV source info
   */
  virtual mdtCsvSourceInfo & sourceTable() = 0;

  /*! \brief Reference CSV source info (read only)
   */
  virtual const mdtCsvSourceInfo & sourceTable() const = 0;

 private:

  /*! \brief Update given field maping state
   */
  void updateFieldMappingState(mdtSqlCopierFieldMapping & fm/*, mdtSqlDriverType::Type destinationDriverType*/);

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
