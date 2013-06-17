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
#ifndef MDT_DATA_WIDGET_MAPPER_H
#define MDT_DATA_WIDGET_MAPPER_H

#include <QDataWidgetMapper>
#include <QObject>

class QSqlTableModel;

/// \note Obselete ?
/*! \brief DataWidget mapper
 */
class mdtSqlDataWidgetMapper : public QDataWidgetMapper
{
 Q_OBJECT

 public:

  mdtSqlDataWidgetMapper(QObject *parent = 0);
  ~mdtSqlDataWidgetMapper();

  /*! \brief Set the model
   * 
   * Note that a QSqlTableModel instance is requierd, because database fields
   *  attributes are used internally.
   *
   * \pre model must be a pointer to a valid instance.
   */
  void setModel(QSqlTableModel *model);

  /*! \brief Get the model instance
   */
  QSqlTableModel *model();

 private:

  // Diseable setModel() with QAbstractItemModel
  void setModel(QAbstractItemModel *model);

  Q_DISABLE_COPY(mdtSqlDataWidgetMapper);
};

#endif  // #ifndef MDT_DATA_WIDGET_MAPPER_H
