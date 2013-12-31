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
#ifndef MDT_SQL_FORM_WINDOW_H
#define MDT_SQL_FORM_WINDOW_H

#include "mdtSqlFormOld.h"
#include <QString>
#include <QSqlDatabase>

class mdtSqlWindowOld;

/*! \brief SQL form window
 *
 * This class is part of the high level SQL GUI API.
 *
 * The main table's GUI is based on a mdtSqlFormWidget.
 *  This widget is created by constructor, and needs a widget
 *  that contains the data widgets (see mdtSqlFormWidget for details).
 *  To attach your Ui (typically created with Qt Designer), call
 *   your setupUi() on widget returned by sqlFormWidget() .
 *
 * Setting up the Ui is the first thing to do.
 *
 * Once done, use mdtSqlForm methods to continue setup.
 * 
 * \todo Obselete this class
 */
class mdtSqlFormWindow : public mdtSqlFormOld
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlFormWindow(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlFormWindow();

  /*! \brief Access SQL window instance
   *
   * \post Returns a valid object.
   */
  mdtSqlWindowOld *sqlWindow();

  /*! \brief Show the form
   */
  void show();

 private:

  /*! \brief Add a widget to the window's childs tab
   */
  void addChildWidget(mdtAbstractSqlWidget *widget);

  Q_DISABLE_COPY(mdtSqlFormWindow);

  mdtSqlWindowOld *pvSqlWindow;
};

#endif  // #ifndef MDT_SQL_FORM_WINDOW_H
