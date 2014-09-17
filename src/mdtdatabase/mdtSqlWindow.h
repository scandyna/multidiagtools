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
#ifndef MDT_SQL_WINDOW_H
#define MDT_SQL_WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include "ui_mdtSqlWindow.h"
#include <memory>

class QAction;
class QCloseEvent;
class mdtSqlForm;
///class mdtSqlDataWidgetController;

/*! \brief Main window for a database table view/editor
 *
 * This class offers some functionnality for the GUI part.
 *  Based on QMainWindow, it displays a menu, toolbar and a status bar.
 *
 * It was designed to work with a subclass of mdtSqlForm .
 */
class mdtSqlWindow : public QMainWindow, public Ui::mdtSqlWindow
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Will construct a empty window.
   *  Use setSqlWidget() to set central widget,
   *  enableNavigation() to build navigaton bar
   *  and enableEdition() to add edition buttons.
   */
  mdtSqlWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

  /*! \brief Destructor
   */
  ~mdtSqlWindow();

  /*! \brief Set SQL table controller
   */
  ///void setTableController(std::shared_ptr<mdtSqlDataWidgetController> controller);

  /*! \brief Set the SQL form
   *
   * \pre form must be a valid pointer .
   */
  void setSqlForm(mdtSqlForm *form);

  /*! \brief Get SQL for
   *
   * Returns the SQL form that was set with setSqlForm(),
   *  or a null pointer if it was not set.
   */
  mdtSqlForm *sqlForm() { return pvForm; }

  /*! \brief Enable navigation
   *
   * Will build a navigation bar
   *  with |<< < > >>| buttons.
   *
   * \pre Form must be set with setSqlForm() before call this method.
   */
  void enableNavigation();

  /*! \brief Disable navigation
   *
   * \sa enableNavigation()
   */
  void disableNavigation();

  /*! \brief Enable edition
   *
   * Will add insert/revert/save/delete buttons.
   *
   * \pre Form must be set with setSqlForm() before call this method.
   */
  void enableEdition();

  /*! \brief Disable edition
   *
   * \sa enableEdition()
   */
  void disableEdition();

 private:

  /*! \brief Create edition elements
   */
  void createEditionElements();

  /*! \brief Connect edition elements
   *
   * \pre pvForm must be valid
   */
  void connectEditionElements();

  /*! \brief Create navigation elements
   */
  void createNavigationElements();

  /*! \brief Connect navigation elements
   *
   * \pre pvForm must be valid
   */
  void connectNavigationElements();

  /*! \brief Give a chance to the user to save data before close
   */
  void closeEvent(QCloseEvent *event);

  QAction *actSubmit;
  QAction *actRevert;
  QAction *actInsert;
  QAction *actRemove;
  QAction *actNavToFirst;
  QAction *actNavToLast;
  QAction *actNavToPrevious;
  QAction *actNavToNext;
  mdtSqlForm *pvForm;

  Q_DISABLE_COPY(mdtSqlWindow);
};

#endif  // #ifndef MDT_SQL_WINDOW_H
