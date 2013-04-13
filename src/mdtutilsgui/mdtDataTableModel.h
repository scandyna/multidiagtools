/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_DATA_TABLE_MODEL_H
#define MDT_DATA_TABLE_MODEL_H

#include "mdtCsvFile.h"
#include "mdtFieldMap.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QString>
#include <QChar>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QSqlField>
#include <QSqlIndex>

/*! \brief
 */
class mdtDataTableModel : public QSqlTableModel
{
 Q_OBJECT

 public:

  /*! \brief
   */
  mdtDataTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief
   */
  ~mdtDataTableModel();

  /*! \brief Set table name
   *
   * Will fetch some informations on fields and call
   *  QSqlTableModel::setTable().
   */
  void setTable(const QString & tableName);

  /*! \brief Add a row of data in model
   *
   * This is a helper method wich uses QSqlTableModel's insert/setData methods.
   * Note: submit() or submitAll() (for OnManualSubmit strategy) is done internally.
   *
   * \param data A map with field name as key and associate data to add.
   * \param role See QSqlTableModel.
   * \return True on success. See QSqlTableModel for known errors.
   */
  bool addRow(const QMap<QString,QVariant> &data, int role = Qt::EditRole);

  /*! \brief Add a row of data in model
   *
   * This is a helper method wich uses QSqlTableModel's insert/setData methods.
   * Note: submit() or submitAll() (for OnManualSubmit strategy) is done internally.
   *
   * \param data A list of data items for the row.
   *              Note: if field order is unknow, use addRow(const QMap\<QString,QVariant>, int) .
   * \param pkNotInData Set true if primary key is not contained in data (must also be auto generated).
   * \param role See QSqlTableModel.
   * \return True on success. See QSqlTableModel for known errors.
   */
  bool addRow(const QList<QVariant> &data, bool pkNotInData, int role = Qt::EditRole);

  /*! \brief Add a row of data in model
   *
   * This is a helper method wich uses QSqlTableModel's insert/setData methods.
   * Note: submit() or submitAll() (for OnManualSubmit strategy) is done internally.
   *
   * \param data A list of data items for the row. Data will be converted to field's format.
   *              Note: if field order is unknow, use addRow(const QMap\<QString,QVariant>, int) .
   * \param pkNotInData Set true if primary key is not contained in data (must also be auto generated).
   * \param role See QSqlTableModel.
   * \return True on success. False if a data conversion failed, or other error. See QSqlTableModel for known errors.
   */
  bool addRow(const QStringList &data, bool pkNotInData, int role = Qt::EditRole);

  /*! \brief Add some rows of data in model
   *
   * This is a helper method wich uses QSqlTableModel's insert/setData methods.
   * Notes:
   *  - submitAll() is done internally when all rows are stored in model.
   *  - This method will fail if edit strategy is not OnManualSubmit (See QSqlTableModel documentations for details).
   *
   * \param rows A list of rows containing data items (columns). Data will be converted regarding fieldMap.
   * \param fieldMap Correspondence between rows field indexes and model indexes. See mdtFieldMap for details.
   * \param role See QSqlTableModel.
   * \return True on success, false else. See QSqlTableModel for known errors.
   */
  bool addRows(const QList<QStringList> &rows, const mdtFieldMap &fieldMap, int role = Qt::EditRole);

  /*! \brief Set data for a given index in model
   *
   * This is a helper method wich uses QSqlTableModel's setData methods.
   * Note: submit() or submitAll() (for OnManualSubmit strategy) is done internally.
   *
   * \param row Row to update.
   * \param column Column to update.
   * \param value Value to set at given index.
   * \param role See QSqlTableModel.
   * \return True on success. If row or column are out of bound, false is returned.
   *          See QSqlTableModel for details.
   */
  bool setData(int row, int column, const QVariant &value, int role = Qt::EditRole);

  /*! \brief Set data for a given index in model
   *
   * This is a helper method wich uses QSqlTableModel's setData methods.
   * Note: submit() or submitAll() (for OnManualSubmit strategy) is done internally.
   *
   * \param row Row to update.
   * \param field Name of field to update.
   * \param value Value to set at given index.
   * \param role See QSqlTableModel.
   * \return True on success. If row is out of bound or field not exists, false is returned.
   *          See QSqlTableModel for details.
   */
  bool setData(int row, const QString &field, const QVariant &value, int role = Qt::EditRole);

 private:

  /*! \brief Call submit() or submitAll() depending on current edit strategy
   */
  bool doSubmit();

  Q_DISABLE_COPY(mdtDataTableModel);

  QList<int> pvPkIndexes; // Hold the (column) indexes of primary keys
};

#endif  // #ifndef MDT_DATA_TABLE_MODEL_H
