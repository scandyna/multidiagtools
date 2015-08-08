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
#ifndef MDT_SQL_FIELD_TABLE_MODEL_H
#define MDT_SQL_FIELD_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QList>
#include <QVariant>

class QComboBox;

/*! \brief Model to access attributes of a list of QSqlFields
 *
 * \todo Should be renamed to mdtSqlFieldSetupTableModel
 *
 * Each row represents a field and each column the attribute of the field.
 *
 * Typical usage with a QComboBox:
 * \code
 *   QComboBox *cb = new QComboBox;
 *   cb->setModel(new mdtSqlFieldSetupTableModel(this));
 *   cb->setModelColumn(mdtSqlFieldSetupTableModel::NameIndex);  // Will display field name
 *   // Recommended way to enable field name edition
 *   cb->setInsertPolicy(QComboBox::InsertAtCurrent);
 *   cb->setEditable(true);
 * \endcode
 */
class mdtSqlFieldSetupTableModel : public QAbstractTableModel
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
  };

  /*! \brief Constructor
   */
  mdtSqlFieldSetupTableModel(QObject *parent = 0);

  /*! \brief Set field list
   */
  void setFieldList(const QList<QSqlField> & fields);

  /*! \brief Get field at given row
   *
   * If given row does not exist, a invalid QSqlField is returned
   */
  QSqlField field(int row) const;

  /*! \brief Get field for given combo box current index
   *
   * \sa field()
   */
  QSqlField currentField(QComboBox *cb) const;

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
   */
  Qt::ItemFlags flags(const QModelIndex & index) const;

  /*! \brief Set data
   */
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

 private:

  Q_DISABLE_COPY(mdtSqlFieldSetupTableModel)

  QList<QSqlField> pvFields;
};

#endif // #ifndef MDT_SQL_FIELD_TABLE_MODEL_H
