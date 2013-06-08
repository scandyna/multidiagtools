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
#ifndef MDT_SQL_WINDOW_H
#define MDT_SQL_WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include "ui_mdtSqlWindow.h"

class mdtAbstractSqlWidget;
class QAction;

/*! \brief Main window for a database table view/editor
 *
 * This class offers some functionnality for the GUI part.
 *  Based on QMainWindow, it displays a menu, toolbar and a status bar.
 *
 * It was designed to work with a subclass of mdtAbstractSqlWidget.
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

  /*! \brief Set table view/editor
   *
   * Will make some signal/slot connection,
   *  then place sqlWidget as QMainWindow's central widget.
   *
   * \pre sqlWidget must be a valid pointer.
   */
  void setSqlWidget(mdtAbstractSqlWidget *sqlWidget);

  /*! \brief Enable navigation
   *
   * Will build a navigation bar
   *  with |<< < > >>| buttons.
   *
   * \pre sqlWidget must be set with setSqlWidget() before call this method.
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
   * \pre sqlWidget must be set with setSqlWidget() before call this method.
   */
  void enableEdition();

  /*! \brief Disable edition
   *
   * \sa enableEdition()
   */
  void disableEdition();

 private:

  QAction *actSubmit;
  QAction *actRevert;
  QAction *actInsert;
  QAction *actRemove;
  QAction *actNavToFirst;
  QAction *actNavToLast;
  QAction *actNavToPrevious;
  QAction *actNavToNext;

/**
  private:
    mdtAbstractSqlWidget * pvSqlWidget;
*/

  Q_DISABLE_COPY(mdtSqlWindow);
};

#endif  // #ifndef MDT_SQL_WINDOW_H
