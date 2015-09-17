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
#ifndef MDT_SQL_TABLE_COPIER_MAPPING_H
#define MDT_SQL_TABLE_COPIER_MAPPING_H

#include "mdtSqlSchemaTable.h"
#include "mdtSqlCopierFieldMapping.h"
#include "mdtError.h"
#include <QString>
#include <QStringList>
#include <QVector>
#include <QSqlDatabase>

/*! \brief Mapping used to copy 2 SQL tables
 */
class mdtSqlTableCopierMapping
{
 public:

  /*! \brief Set source database
   *
   * Will also clear field mapping and source table.
   *
   * \sa clearFieldMapping()
   */
  bool setSourceDatabase(const QSqlDatabase & db);

  /*! \brief Set destination database
   *
   * Will also clear field mapping and destination table.
   *
   * \sa clearFieldMapping()
   */
  bool setDestinationDatabase(const QSqlDatabase & db);

  /*! \brief Set source table
   *
   * Will also reset field mapping.
   *
   * \sa resetFieldMapping()
   */
  bool setSourceTable(const QString & tableName);

  /*! \brief Set destination table
   *
   * Will also reset field mapping.
   *
   * \sa resetFieldMapping()
   */
  bool setDestinationTable(const QString & tableName);

  /*! \brief Add a field mapping
   *
   * \pre fm's source and destination field indexes must be in valid range
   */
//   void addFieldMapping(const mdtSqlCopierFieldMapping & fm);

  /*! \brief Add a field mapping
   *
   * \pre sourceFieldIndex must be in a valid range and destination field name must exist in destination table.
   */
//   void addFieldMapping(int sourceFieldIndex, const QString & destinationFieldName);

  /*! \brief Reset field mapping
   *
   * Will clear field mapping,
   *  then, generate it for each available field in source table.
   *  After this, no valid field mapping is set to destination table.
   */
  void resetFieldMapping();

  /*! \brief Clear field mapping
   */
  void clearFieldMapping();

  /*! \brief Generate field mapping by name
   *
   * Will first reset field mapping.
   *  Then, for each field in source table,
   *  destination field is defined by source field name.
   *
   * \todo Type compatiblity checking is to add..
   */
  void generateFieldMappingByName();

  /*! \brief Set destination field for given field mapping index
   *
   * If destination field name is empty,
   *  the destination field will be removed for given index.
   *
   * \pre index must be in a valid range
   */
  void setDestinationField(int index, const QString & fieldName);

  /*! \brief Get field count
   *
   * Field count depends on number of field mapping set.
   */
  int fieldCount() const
  {
    return pvFieldMappingList.size();
  }

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
  QString sourceFieldName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFieldMappingList.size());

    int sourceFieldIndex = pvFieldMappingList.at(index).sourceFieldIndex;
    Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
    if(sourceFieldIndex < 0){
      return QString();
    }
    return pvSourceTable.fieldName(sourceFieldIndex);
  }

  /*! \brief Get source field type name for given field mapping index
   *
   * \pre index must be in valid range.
   */
  QString sourceFieldTypeName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFieldMappingList.size());

    int sourceFieldIndex = pvFieldMappingList.at(index).sourceFieldIndex;
    Q_ASSERT(sourceFieldIndex < pvSourceTable.fieldCount());
    if(sourceFieldIndex < 0){
      return QString();
    }
    return pvSourceTable.fieldTypeName(sourceFieldIndex);
  }

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
  QString destinationFieldName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFieldMappingList.size());

    int destinationFieldIndex = pvFieldMappingList.at(index).destinationFieldIndex;
    Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
    if(destinationFieldIndex < 0){
      return QString();
    }
    return pvDestinationTable.fieldName(destinationFieldIndex);
  }

  /*! \brief Get destination field type name for given field mapping index
   *
   * \pre index must be in valid range.
   */
  QString destinationFieldTypeName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFieldMappingList.size());

    int destinationFieldIndex = pvFieldMappingList.at(index).destinationFieldIndex;
    Q_ASSERT(destinationFieldIndex < pvDestinationTable.fieldCount());
    if(destinationFieldIndex < 0){
      return QString();
    }
    return pvDestinationTable.fieldTypeName(destinationFieldIndex);
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Get field mapping of given source field index
   *
   * If no mapping was defined with given source field index,
   *  a null field mapping is returned.
   */
//   mdtSqlCopierFieldMapping fieldMapping(int sourceFieldIndex) const
//   {
//     for(const auto & fm : pvFieldMappingList){
//       if(fm.sourceFieldIndex == sourceFieldIndex){
//         return fm;
//       }
//     }
//     return mdtSqlCopierFieldMapping();
//   }

  QSqlDatabase pvSourceDatabase;
  mdtSqlSchemaTable pvSourceTable;
  QSqlDatabase pvDestinationDatabase;
  mdtSqlSchemaTable pvDestinationTable;
  QVector<mdtSqlCopierFieldMapping> pvFieldMappingList;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_TABLE_COPIER_MAPPING_H
