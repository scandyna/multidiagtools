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
#include "ui_mdtTtBasicTestNodeCalibrationWindow.h"
#include <QMainWindow>

#include <QSqlDatabase>

class mdtTtBasicTestNodeCalibrationTool;
class mdtSqlTableWidget;
class QCloseEvent;

/*! \biref Test node calibration example
 */
class mdtTtBasicTestNodeCalibrationWindow : public QMainWindow, Ui::mdtTtBasicTestNodeCalibrationWindow
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtBasicTestNodeCalibrationWindow(QSqlDatabase db, QWidget *parent = 0);

  /*! \brief Init
   */
  bool init();

 private:

  /*! \brief Connect actions
   */
  void connectActions();

  Q_DISABLE_COPY(mdtTtBasicTestNodeCalibrationWindow);

  mdtSqlTableWidget *pvTestNodeUnitWidget;
  mdtTtBasicTestNodeCalibrationTool *pvCalibrationTool;
};