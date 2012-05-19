/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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

#include "mdtSqlTableModel.h"

/*! \brief DataWidget mapper
 */
class mdtDataWidgetMapper : public QDataWidgetMapper
{
 Q_OBJECT

 public:

  mdtDataWidgetMapper(QObject *parent = 0);
  ~mdtDataWidgetMapper();

  /*! \brief Set the model
   * 
   * Note that a mdtSqlTableModel instance is requierd, because database fields
   * attributes are used internally.
   * \pre model must be a pointer to a valid instance.
   */
  void setModel(mdtSqlTableModel *model);

  /*! \brief Get the model instance
   */
  mdtSqlTableModel *model();

  /*! \brief Adds a mapping between a widget and a section from the model.
   * 
   * This method uses the widget name (objectName), and tries to find
   * the field (in database) that has the same name.<br>
   * If field is found, some attributes are set (f.ex: read only, mandatory field, ...).
   * At last step, QDataWidgetMapper::addMapping() is called.
   * \pre A valid model must be set with setModel()
   * \pre widget must be a valid pointer
   */
  void addMapping(QWidget *widget, int section);

  /*! \brief Adds a mapping between a widget and a section from the model.
   * 
   * \sa addMapping()
   */
  void addMapping(QWidget *widget, int section, const QByteArray &propertyName);

 private:

  // Diseable setModel() with QAbstractItemModel
  void setModel(QAbstractItemModel *model);

  // Set widget's attributes - Used by addMapping()
  void setWidgetAttributes(QWidget *widget, int section);

  mdtSqlTableModel *pvSqlTableModel;
};

#endif  // #ifndef MDT_DATA_WIDGET_MAPPER_H
