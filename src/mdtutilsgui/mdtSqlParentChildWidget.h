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
#ifndef MDT_SQL_PARENT_CHILD_WIDGET_H
#define MDT_SQL_PARENT_CHILD_WIDGET_H

#include <QWidget>
#include <QList>
#include <QString>

class mdtAbstractSqlWidget;
class mdtSqlRelation;
class QTabWidget;

/*! \brief Helper widget to create a master/detail (parent/child) representation
 *
 * To create a master/detail database form, this class will help
 *  to dispose the widgets and for signal/slot connection
 *  between relations and widgets.
 *
 * Note: because all widgets given by setParentWidget() and addChildWidget()
 *  are reparented to internall layout, they will be deleted automatically
 *  (as usual in Qt).
 *  The rule is the same for relations added with addChildWidget(),
 *   wich will be reparented.
 */
class mdtSqlParentChildWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlParentChildWidget(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlParentChildWidget();

  /*! \brief Set the parent (master) widget
   *
   * \pre widget must be a valid pointer
   */
  void setParentWidget(mdtAbstractSqlWidget *widget);

  /*! \brief Get parent widget
   *
   * Will return 0 if parent was not set.
   */
  mdtAbstractSqlWidget *parentWidget();

  /*! \brief Add a child (details) widget and relation
   *
   * Needed signal/slot connection between widget
   *  and relation is done internally.
   *
   * \param widget Will be added to the childs tab
   * \param relation Relation between parent and child
   * \param label Will be displayed in tab of tab widget
   *
   * \pre Parent widget must be set with setParentWidget() before using this method
   * \pre widget must be a valid pointer
   * \pre relation must be a valid pointer
   */
  void addChildWidget(mdtAbstractSqlWidget *widget, mdtSqlRelation *relation, const QString &label);


 public slots:

  /*! \brief Save data (if required) in child widgets and disable it
   *
   * When editing data in parent table,
   *  it can happen that data becomes currupted,
   *  or simply that we let related child tables in a unsaved
   *  state.
   *
   * This method will check child widgets state.
   *  If a child is not in Visualizing state,
   *  a warning is displayed to the user,
   *  and fromChildWidgetErrorOccured() signal is emitted.
   */
  ///void checkAndDisableChildWidgets();

  /*! \brief Set child widget enabled
   *
   * Typically used after parent widget insertion
   */
  ///void enableChildWidgets();

 private:

  Q_DISABLE_COPY(mdtSqlParentChildWidget);

  mdtAbstractSqlWidget *pvParentWidget;
  QList<mdtSqlRelation*> pvRelations;
  QList<mdtAbstractSqlWidget*> pvChildWidgets;
  QTabWidget *pvChildsTabWidget;
};

#endif  // #ifndef MDT_SQL_PARENT_CHILD_WIDGET_H
