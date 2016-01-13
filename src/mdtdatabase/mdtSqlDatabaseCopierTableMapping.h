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
#ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_H
#define MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_H

#include "mdt/sql/copier/TableMapping.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlDriverType.h"
#include <QString>
#include <QStringList>
#include <QSqlDatabase>

class QSqlRecord;

/*! \brief Mapping used to copy 2 SQL tables
 */
class mdtSqlDatabaseCopierTableMapping : public mdt::sql::copier::TableMapping
{
 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseCopierTableMapping() = default;

  /*! \brief Copy constructor
   */
//    mdtSqlDatabaseCopierTableMapping(const mdtSqlDatabaseCopierTableMapping & other) = default;

  /*! \brief Copy assignation
   */
//    mdtSqlDatabaseCopierTableMapping & operator=(const mdtSqlDatabaseCopierTableMapping & other) = default;

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

  /*! \brief Get field count in source table
    */
  int sourceTableFieldCount() const override
  {
    return pvSourceTable.fieldCount();
  }

  /*! \brief Get field count in destination table
    */
  int destinationTableFieldCount() const override
  {
    return pvDestinationTable.fieldCount();
  }

  /*! \brief Generate field mapping by name
   *
   * Will first reset field mapping.
   *  Then, for each field in destination table,
   *  source field is defined by destination field name.
   */
//   void generateFieldMappingByName();

 private:

  /*! \brief Get a list of available field names in source table
   */
  QStringList fetchSourceTableFieldNameList() const override
  {
    return pvSourceTable.getFieldNameList();
  }

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
    return pvSourceTable.fieldIndex(fieldName);
  }

  /*! \brief Get field name for given fieldIndex in source table
   */
  QString fetchSourceTableFieldNameAt(int fieldIndex) const override
  {
    return pvSourceTable.fieldName(fieldIndex);
  }

  /*! \brief Get field type name for given fieldIndex in source table
   */
  QString fetchSourceTableFieldTypeNameAt(int fieldIndex) const override;

  /*! \brief Check if field is part of a key for given field index in source table
   */
  FieldKeyType fetchSourceTableFieldKeyType(int fieldIndex) const override;

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
  bool areFieldsCompatible(int sourceFieldIndex, int destinationFieldIndex) const;

  QSqlDatabase pvSourceDatabase;
  mdtSqlSchemaTable pvSourceTable;
  QSqlDatabase pvDestinationDatabase;
  mdtSqlSchemaTable pvDestinationTable;
};

#endif // #ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_H
