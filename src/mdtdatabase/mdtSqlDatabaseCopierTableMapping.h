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
#ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_H
#define MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_H

#include "mdtSqlCopierTableMapping.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlCopierFieldMapping.h"
#include "mdtSqlDriverType.h"
#include <QString>
#include <QStringList>
#include <QSqlDatabase>

/*! \brief Mapping used to copy 2 SQL tables
 */
class mdtSqlDatabaseCopierTableMapping : public mdtSqlCopierTableMapping
{
 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseCopierTableMapping() = default;

  /*! \brief Copy constructor
   */
//   mdtSqlDatabaseCopierTableMapping(const mdtSqlDatabaseCopierTableMapping & other);

  /*! \brief Copy assignation
   */
//   mdtSqlDatabaseCopierTableMapping & operator=(const mdtSqlDatabaseCopierTableMapping & other);

  /*! \brief Set source table
   *
   * Will also reset field mapping.
   *
   * \sa resetFieldMapping()
   */
  bool setSourceTable(const QString & tableName, const QSqlDatabase & db);

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
    return pvSourceTable.tableName();
  }

  /*! \brief Get destination table name
   */
  QString destinationTableName() const
  {
    return pvDestinationTable.tableName();
  }

  /*! \brief Generate field mapping by name
   *
   * Will first reset field mapping.
   *  Then, for each field in destination table,
   *  source field is defined by destination field name.
   */
  void generateFieldMappingByName();

  /*! \brief Set source field for given field mapping index
   *
   * If source field name is empty,
   *  the source field will be removed for given index.
   *
   * \pre index must be in a valid range
   */
  void setSourceField(int index, const QString & fieldName);

  /*! \brief Get list of field names of source table
   */
  QStringList getSourceFieldNameList() const
  {
    return pvSourceTable.getFieldNameList();
  }

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

  /*! \brief Get list of field names of destination table
   */
  QStringList getDestinationFieldNameList() const
  {
    return pvDestinationTable.getFieldNameList();
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

 private:

  /*! \brief Get field count of destination table
   *
   * Mainly used by resetFieldMapping()
   */
  int destinationTableFieldCount() const
  {
    return pvDestinationTable.fieldCount();
  }

  /*! \brief Update given field maping state
   */
  void updateFieldMappingState(mdtSqlCopierFieldMapping & fm, mdtSqlDriverType::Type sourceDriverType, mdtSqlDriverType::Type destinationDriverType);

  QSqlDatabase pvSourceDatabase;
  mdtSqlSchemaTable pvSourceTable;
  QSqlDatabase pvDestinationDatabase;
  mdtSqlSchemaTable pvDestinationTable;
};

#endif // #ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_H
