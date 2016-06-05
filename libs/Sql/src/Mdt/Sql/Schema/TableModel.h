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
#ifndef MDT_SQL_SCHEMA_TABLE_MODEL_H
#define MDT_SQL_SCHEMA_TABLE_MODEL_H

#include "Table.h"
#include <QAbstractTableModel>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Qt item/view based model that represents a SQL schema Table
   *
   * Each row in the model represents a Field and each column a attribute of a field or a key.
   *
   * Typical usage with a QComboBox:
   * \code
   * using Mdt::Sql::Schema::TableModel;
   *
   * auto *cb = new QComboBox;
   * cb->setModel(new TableModel(cb));  // Give model a parent for memory management
   * cb->setModelColumn(TableModel::FieldNameColumn); // Will display field name
   * \endcode
   */
  class TableModel : public QAbstractTableModel
  {
    Q_OBJECT

   public:

    /*! \brief Column index
     */
    enum ColumnIndex
    {
      FieldNameColumn = 0,  /*!< Column index of field name */
      FieldTypeColumn,      /*!< Column index of field type name and its length, if applicable */
      PkFlagColumn,         /*!< Column index of primary key flag */
      AiFlagColumn,         /*!< Column index of auto increment flag */
      NotNullFlagColumn,    /*!< Column index of not null flag */
      UniqueFlagColumn      /*!< Column index of unique flag */
    };

    /*! \brief Constructor
     */
    TableModel(QObject* parent = nullptr);

    // Copy disabled
    TableModel(const TableModel &) = delete;
    TableModel & operator=(const TableModel &) = delete;
    // Move disabled
    TableModel(TableModel &&) = delete;
    TableModel & operator=(TableModel &&) = delete;

    /*! \brief Set table
     */
    void setTable(const Table & table);

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get header data
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*! \brief Get data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

   private:

    /*! \brief Get field type name
     */
    QString fieldTypeName(int row) const;

    /*! \brief Get PK flag
     */
    QVariant pkFlag(int row) const;

    /*! \brief Get auto increment flag
     */
    QVariant autoIncrementFlag(int row) const;

    /*! \brief Get not null flag
     */
    QVariant notNullFlag(int row) const;

    /*! \brief Get unique flag
     */
    QVariant uniqueFlag(int row) const;

    /*! \brief Get header tool tip text
     */
    QString headerToolTipText(int column) const;

    Table pvTable;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_MODEL_H
