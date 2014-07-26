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
#ifndef MDT_TT_BASIC_TESTER_WINDOW_H
#define MDT_TT_BASIC_TESTER_WINDOW_H

#include "ui_mdtTtBasicTesterWindow.h"
#include "mdtTtTestData.h"
#include "mdtError.h"
#include <QWidget>
#include <QMainWindow>
#include <QSqlDatabase>

class mdtTtBasicTester;
class mdtDeviceContainerWidget;
class mdtSqlTableWidget;
class QCloseEvent;

class mdtTtBasicTesterWindow : public QMainWindow, Ui::mdtTtBasicTesterWindow
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtBasicTesterWindow(QSqlDatabase db, QWidget *parent);

  /*! \brief Init
   */
  bool init();

  /*! \brief Get last error
   */
  mdtError lastError() const;

 public slots:

  /*! \brief Update widget with given test data
   */
  void displayTestData(const QSqlRecord & data);

 private:

  /*! \brief Setup test item table widget
   */
  void setupTestItemTableWidget();

  /*! \brief Connect actions to slots
   */
  void connectActions();

  /*! \brief Do some check before accept close event
   */
  void closeEvent(QCloseEvent *event);

  Q_DISABLE_COPY(mdtTtBasicTesterWindow);

  mdtTtBasicTester *pvTesterWidget;
  mdtDeviceContainerWidget *pvDeviceContainerWidget;
  mdtSqlTableWidget *pvTestItemWidget;  // Display data from TestItem_view (R only access)
};

#endif
