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
   * Will parse all child widgets found in given widget,
   *  extract widgets that have a fld_ prefix as object name.
   *  For each of them, corresponding field will be searched in model (database).
   *  Note: if you create widgets by code, don't forget to set objectName (QObject::setObjectName() ).
   *   When using Qt Designer, this is done automatically.
   *
   * \param widget Widget on witch child widgets must be searched.
   * \param firstWidgetInTabOrder It's possible to specify the object name of the first widget in tab order.
   *                               This is then used by insertion to give this widget the focus.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool mapFormWidgets(QWidget *widget, const QString &firstWidgetInTabOrder = QString());

  /*! \brief Get current row
   */
  int currentRow() const;

 public slots:

  /*! \brief Set first record as current record
   */
  void toFirst();

  /*! \brief Set last record as current record
   */
  void toLast();

  /*! \brief Set previous record as current record
   */
  void toPrevious();

  /*! \brief Set next record as current record
   */
  void toNext();

 signals:

  /*! \brief Emitted when toFirst() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toFirstEnabledStateChanged(bool enabled);

  /*! \brief Emitted when toLast() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toLastEnabledStateChanged(bool enabled);

  /*! \brief Emitted when toNext() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toNextEnabledStateChanged(bool enabled);

  /*! \brief Emitted when toPrevious() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void toPreviousEnabledStateChanged(bool enabled);

 private slots:

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateVisualizingEntered();

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateVisualizingExited();

 private:

  /*! \brief Table model set event
   */
  void modelSetEvent();

  /*! \brief Current row changed event
   */
  void currentRowChangedEvent(int row);

  /*! \brief Submit current row to model
   */
  bool doSubmit();

  /*! \brief Revert current row from model
   */
  bool doRevert();

  /*! \brief Insert a new row to model
   */
  bool doInsert();

  /*! \brief Submit new row to model
   */
  bool doSubmitNewRow();

  /*! \brief Revert new row
   */
  bool doRevertNewRow();

  /*! \brief Remove current row from model
   */
  bool doRemove();

  /*! \brief Update mapped data widgets
   *
   * Will update enabled/disabled state
   *  of mapped widgets regarding if a model was set,
   *  state machine is running and current row.
   */
  void updateMappedWidgets();

  /*! \brief Set the focus to first data widget in focus chain (if exists)
   */
  void setFocusOnFirstDataWidget();

  /*! \brief Clear mapped data widgets
   */
  void clearMappedWidgets();

  /*! \brief Update navigation controls states
   *
   * Will emit navigation control state signals (f.ex. toNextEnabledStateChanged() )
   *  regarding if model was set, state machine is running and current row.
   */
  void updateNavigationControls();

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
