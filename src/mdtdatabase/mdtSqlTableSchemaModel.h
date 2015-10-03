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
#ifndef MDT_SQL_TABLE_SCHEMA_MODEL_H
#define MDT_SQL_TABLE_SCHEMA_MODEL_H

#include "mdtSqlSchemaTable.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>

class QComboBox;

/*! \brief Model to access attributes of a SQL table schema
 *
 * Each row represents a field and each column the attribute of the field.
 *
 * Typical usage with a QComboBox:
 * \code
 *   QComboBox *cb = new QComboBox;
 *   cb->setModel(new mdtSqlTableSchemaModel(this));
 *   cb->setModelColumn(mdtSqlTableSchemaModel::NameIndex);  // Will display field name
 *   // Recommended way to enable field name edition
 *   cb->setInsertPolicy(QComboBox::InsertAtCurrent);
 *   cb->setEditable(true);
 * \endcode
 */
class mdtSqlTableSchemaModel : public QAbstractTableModel
{
 Q_OBJECT

 public:

  /*! \brief Column index
   */
  enum ColumnIndex_t
  {
    NameIndex = 0,    /*!< Column index of field name */
    TypeIndex = 1,    /*!< Column index of field type */
    LengthIndex = 2,  /*!< Column index of field length */
    IsPkIndex = 3     /*!< Column index that tells if field is part of primary key */
  };

  /*! \brief Constructor
   */
  mdtSqlTableSchemaModel(QObject *parent = 0);

  /*! \brief Set table schema
   */
  void setTableSchema(const mdtSqlSchemaTable & st);

  /*! \brief Get field at given row
   *
   * If given row does not exist, a invalid QSqlField is returned
   */
//   QSqlField field(int row) const;

  /*! \brief Get field for given combo box current index
   *
   * \sa field()
   */
//   QSqlField currentField(QComboBox *cb) const;

  /*! \brief Check if field at given row is part of primary key
   */
  bool isPartOfPrimaryKey(int row) const;

  /*! \brief Get row count
   *
   * Returns the number of fields
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
   *
   * \todo Currently, edition is not supported
   */
//   Qt::ItemFlags flags(const QModelIndex & index) const;

  /*! \brief Set data
   *
   * \todo Currently, edition is not supported
   */
//   bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

 private:

  Q_DISABLE_COPY(mdtSqlTableSchemaModel)

  mdtSqlSchemaTable pvSchema;
};

#endif // #ifndef MDT_SQL_TABLE_SCHEMA_MODEL_H
