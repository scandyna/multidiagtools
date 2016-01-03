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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_MODEL_H
#define MDT_SQL_COPIER_TABLE_MAPPING_MODEL_H

#include "mdt/sql/copier/TableMapping.h"
#include "mdtError.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>
#include <QVariant>

class mdtComboBoxItemDelegate;

/*! \brief Base table model to access SQL table copier mapping
 *
 * \sa mdtSqlCopierTableMapping
 */
class mdtSqlCopierTableMappingModel : public QAbstractTableModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlCopierTableMappingModel(QObject *parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlCopierTableMappingModel(const mdtSqlCopierTableMappingModel & other) = delete;
  mdtSqlCopierTableMappingModel & operator=(const mdtSqlCopierTableMappingModel &) = delete;

  /*! \brief Setup source type delegate
   */
  void setupSourceTypeDelegate(mdtComboBoxItemDelegate * const delegate);

  /*! \brief Reset field mapping
   *
   * \sa mdtSqlCopierTableMapping::resetFieldMapping()
   */
  void resetFieldMapping();

  /*! \brief Generate field mapping by name
   *
   * \sa mdtSqlCopierTableMapping::generateFieldMappingByName()
   */
  void generateFieldMappingByName();

  /*! \brief Get source table name
   */
  QString sourceTableName() const
  {
    return mappingBase().sourceTableName();
  }

  /*! \brief Get destination table name
   */
  QString destinationTableName() const
  {
    return mappingBase().destinationTableName();
  }

  /*! \brief Get row count
   *
   * Returns the number of source table fields
   */
  int rowCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get column count
   */
  int columnCount(const QModelIndex & parent = QModelIndex()) const;

  /*! \brief Get header data
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  /*! \brief Get data
   */
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

  /*! \brief Set data
   */
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

  /*! \brief Get item flags
   */
  Qt::ItemFlags flags(const QModelIndex & index) const;

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 protected:

  /*! \brief Reference internal table mapping (read only version)
   */
  virtual const mdt::sql::copier::TableMapping & mappingBase() const = 0;

  /*! \brief Reference internal table mapping
   */
  virtual mdt::sql::copier::TableMapping & mappingBase() = 0;

  /*! \brief Column index
   */
  enum ColumnIndex_t
  {
    SourceTypeIndex = 0,        /*!< Column of source type (field or fixed value) */
    SourceKeyTypeIndex,         /*!< Column index of source field key type */
    SourceFieldNameIndex,       /*!< Column index of source field name */
    SourceFieldTypeIndex,       /*!< Column index of source field type */
    SourceFixedValueIndex,      /*!< Column index of source fixed value */
    DestinationKeyTypeIndex,    /*!< Column index of destination field key type */
    DestinationFieldNameIndex,  /*!< Column index of destination field name */
    DestinationFieldTypeIndex,  /*!< Column index of destination field type */
    FieldMappinStateIndex       /*!< Column index of destination field type */
  };

  mdtError pvLastError;

 private:

  /*! \brief Get source type data
   */
  QVariant sourceTypeData(int row, int role) const;

  /*! \brief Get source field key type text
   */
  QVariant sourceFieldKeyTypeText(int row) const;

  /*! \brief Get source field name text
   */
  QVariant sourceFieldNameText(int row) const;

  /*! \brief Get source field type name text
   */
  QVariant sourceFieldTypeNameText(int row) const;

  /*! \brief Get source fixed value
   */
  QVariant sourceFixedValue(int row) const;

  /*! \brief Get destination field key type text
   */
  QVariant destinationFieldKeyTypeText(int row) const;

  /*! \brief Get field mapping state data
   */
  QVariant fieldMappingStateData(int row, int role) const;

  /*! \brief Get field mapping state text
   */
  QVariant fieldMappingStateText(mdtSqlCopierFieldMapping::MappingState state) const;

  /*! \brief Get field mapping state decoration
   */
  QVariant fieldMappingStateDecoration(mdtSqlCopierFieldMapping::MappingState state) const;
};

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_MODEL_H
