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
#ifndef MDT_TT_CABLE_CHECKER_WINDOW_H
#define MDT_TT_CABLE_CHECKER_WINDOW_H

#include "ui_mdtTtCableCheckerWindow.h"
#include <QMainWindow>

class mdtTtCableChecker;
class QWidget;

/*! \brief Cable check window
 */
class mdtTtCableCheckerWindow : public QMainWindow, Ui::mdtTtCableCheckerWindow
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtCableCheckerWindow(QWidget *parent = 0);

  /*! \brief Set cable checker object
   */
  void setCableChecker(mdtTtCableChecker *cc);

 private:

  Q_DISABLE_COPY(mdtTtCableCheckerWindow);

  mdtTtCableChecker *pvCableChecker;
};

#endif // #ifndef MDT_TT_CABLE_CHECKER_WINDOW_H
