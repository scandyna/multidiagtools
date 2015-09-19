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
#ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_MODEL_H
#define MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_MODEL_H

#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtError.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>

class mdtComboBoxItemDelegate;

/*! \brief Table model to access SQL table copier mapping
 *
 * \sa mdtSqlDatabaseCopierTableMapping
 */
class mdtSqlDatabaseCopierTableMappingModel : public QAbstractTableModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseCopierTableMappingModel(QObject *parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlDatabaseCopierTableMappingModel(const mdtSqlDatabaseCopierTableMappingModel & other) = delete;

  /*! \brief Set source database
   *
   * Will also clear field mapping and source table.
   */
//   bool setSourceDatabase(const QSqlDatabase & db);

  /*! \brief Set destination database
   *
   * Will also clear field mapping and destination table.
   */
//   bool setDestinationDatabase(const QSqlDatabase & db);

  /*! \brief Set source table
   *
   * Will also reset field mapping.
   */
  bool setSourceTable(const QString & tableName, const QSqlDatabase & db);

  /*! \brief Set destination table
   *
   * Will also reset field mapping.
   *
   * If delegate is not null,
   *  its combobox will be populated with available fields.
   */
  bool setDestinationTable(const QString & tableName, const QSqlDatabase & db, mdtComboBoxItemDelegate *delegate = nullptr);

  /*! \brief Reset field mapping
   *
   * \sa mdtSqlDatabaseCopierTableMapping::resetFieldMapping()
   */
  void resetFieldMapping();

  /*! \brief Generate field mapping by name
   *
   * \sa mdtSqlDatabaseCopierTableMapping::generateFieldMappingByName()
   */
  void generateFieldMappingByName();

  /*! \brief Set mapping
   */
  void setMapping(const mdtSqlDatabaseCopierTableMapping & m);

  /*! \brief Get mapping
   */
  mdtSqlDatabaseCopierTableMapping mapping() const
  {
    return pvMapping;
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

  /*! \brief Get item flags
   */
  Qt::ItemFlags flags(const QModelIndex & index) const;

  /*! \brief Set data
   */
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Column index
   */
  enum ColumnIndex_t
  {
    SourceNameIndex = 0,      /*!< Column index of source field name */
    SourceTypeIndex = 1,      /*!< Column index of source field type */
    DestinationNameIndex = 2, /*!< Column index of destination field name */
    DestinationTypeIndex = 3  /*!< Column index of destination field type */
  };

  mdtSqlDatabaseCopierTableMapping pvMapping;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_MODEL_H
