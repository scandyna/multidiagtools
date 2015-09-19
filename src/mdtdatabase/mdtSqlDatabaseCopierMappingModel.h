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
#ifndef MDT_SQL_DATABASE_COPIER_MAPPING_MODEL_H
#define MDT_SQL_DATABASE_COPIER_MAPPING_MODEL_H

#include "mdtSqlDatabaseCopierMapping.h"
#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtError.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>

class mdtComboBoxItemDelegate;

/*! \brief Table model to access SQL database copier mapping
 *
 * \sa mdtSqlDatabaseCopierMapping
 */
class mdtSqlDatabaseCopierMappingModel : public QAbstractTableModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseCopierMappingModel(QObject *parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlDatabaseCopierMappingModel(const mdtSqlDatabaseCopierMappingModel & other) = delete;

  /*! \brief Set source database
   *
   * Will also reset table mapping.
   */
  bool setSourceDatabase(const QSqlDatabase & db);

  /*! \brief Set destination database
   *
   * Will also reset table mapping.
   */
  bool setDestinationDatabase(const QSqlDatabase & db);

  /*! \brief Generate table mapping by name
   *
   * \sa mdtSqlDatabaseCopierMapping::generateTableMappingByName()
   */
  bool generateTableMappingByName();

  /*! \brief Get table mapping at given row
   *
   * \pre row must be in a valid range
   */
  mdtSqlDatabaseCopierTableMapping tableMapping(int row) const;

  /*! \brief Set table mapping for given row
   *
   * \pre row must be in a valid range
   */
  void setTableMapping(int row, const mdtSqlDatabaseCopierTableMapping & tm);

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

  /*! \brief Get data about source table column
   */
  QVariant sourceTableData(int row, int role) const;

  /*! \brief Get data about destination table column
   */
  QVariant destinationTableData(int row, int role) const;

  /*! \brief Get mapping state data
   */
  QVariant tableMappingStateData(int row, int role) const;

  /*! \brief Get table mapping state text
   */
  QString tableMappingStateText(mdtSqlDatabaseCopierTableMapping::MappingState state) const;

  /*! \brief Get table mapping state decoration
   */
  QVariant tableMappingStateDecoration(mdtSqlDatabaseCopierTableMapping::MappingState state) const;

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
    SourceTableNameIndex = 0,       /*!< Column index of source table name */
    DestinationTableNameIndex = 1,  /*!< Column index of destination table name */
    TableMappingStateIndex = 2      /*!< Column index of table mapping state */
  };

  mdtSqlDatabaseCopierMapping pvMapping;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_DATABASE_COPIER_MAPPING_MODEL_H
