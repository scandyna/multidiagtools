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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_MODEL_H
#define MDT_SQL_COPIER_TABLE_MAPPING_MODEL_H

#include "mdtSqlCopierTableMapping.h"
#include "mdtError.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>

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
  virtual const mdtSqlCopierTableMapping & mappingBase() const = 0;

  /*! \brief Reference internal table mapping
   */
  virtual mdtSqlCopierTableMapping & mappingBase() = 0;

  /*! \brief Column index
   */
  enum ColumnIndex_t
  {
    SourceNameIndex = 0,      /*!< Column index of source field name */
    SourceTypeIndex = 1,      /*!< Column index of source field type */
    DestinationNameIndex = 2, /*!< Column index of destination field name */
    DestinationTypeIndex = 3  /*!< Column index of destination field type */
  };

  mdtError pvLastError;

//private:

};

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_MODEL_H
