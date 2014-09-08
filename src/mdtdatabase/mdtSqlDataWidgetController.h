/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_SQL_DATA_WIDGET_CONTROLLER_H
#define MDT_SQL_DATA_WIDGET_CONTROLLER_H

#include "mdtAbstractSqlTableController.h"
#include <QDataWidgetMapper>
#include <QWidget>
#include <QList>
#include <QString>

class mdtSqlFieldHandler;
class QSqlTableModel;
class QLayoutItem;

/*! \brief
 */
class mdtSqlDataWidgetController : public mdtAbstractSqlTableController
{
  Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDataWidgetController(QObject* parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlDataWidgetController();

  /*! \brief Map widgets to matching database fields
   *
   * Will parse all child widgets found in layout (and contained widgets layouts),
   *  extract widgets that have a fld_ prefix as object name.
   *  For each of them, corresponding field will be searched in model (database).
   *
   * \param widget Widget on witch child widgets must be searched.
   * \param firstWidgetInTabOrder It's possible to specify the object name of the first widget in tab order.
   *                               This is then used by insertion to give this widget the focus.
   *
   * \pre The custom form's setupUi() must be called on this object before using this method.
   *       Once setupUi() is done, mdtSqlFormWidget object (this) will contain a layout
   *       with all widgets (QLineEdit, QSpinBox, ...).
   * \pre Model must be set with setModel() before using this method.
   */
  bool mapFormWidgets(QWidget *widget, const QString &firstWidgetInTabOrder = QString());

 private:

  /*! \brief Build the list of widgets contained in w
   *
   * Result will be stored to pvFoundWidgets .
   *
   * \pre w must be valid and contain a valid layout
   */
  void buildWidgetsList(QWidget *w, const QString &prefix);

  /*! \brief Search all widgets contained in a layout item
   *
   * If objectName beginns with prefix, it will be added to pvFoundWidgets .
   */
  void searchWidgets(QLayoutItem *item, const QString &prefix);

  Q_DISABLE_COPY(mdtSqlDataWidgetController);

  QDataWidgetMapper pvWidgetMapper;
  QWidgetList pvFoundWidgets;       // Used temporary during mapping - see searchWidgets() and buildWidgetsList()
  QList<mdtSqlFieldHandler*> pvFieldHandlers;
  QWidget *pvFirstDataWidget;       // Keep trace of first data edit/view widget in focus chain
};

#endif  // #ifndef MDT_SQL_DATA_WIDGET_CONTROLLER_H
