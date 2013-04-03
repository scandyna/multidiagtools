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

  /*! \brief Add a row of data in model
   *
   * This is a helper method wich uses QSqlTableModel's insert/setData methods.
   * Note: submit() is done internally.
   *
   * \param data A map with field name as key and associate data to add.
   * \param role See QSqlTableModel.
   * \return True on success. See QSqlTableModel for known errors.
   */
  bool addRow(const QMap<QString,QVariant> &data, int role = Qt::EditRole);

  /*! \brief Set data for a given index in model
   *
   * This is a helper method wich uses QSqlTableModel's setData methods.
   * Note: submit() is done internally.
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
   * Note: submit() is done internally.
   *
   * \param row Row to update.
   * \param field Name of field to update.
   * \param value Value to set at given index.
   * \param role See QSqlTableModel.
   * \return True on success. If row is out of bound or field not exists, false is returned.
   *          See QSqlTableModel for details.
   */
  bool setData(int row, const QString &field, const QVariant &value, int role = Qt::EditRole);
};

#endif  // #ifndef MDT_DATA_TABLE_MODEL_H
