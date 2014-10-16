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
#ifndef MDT_TT_LOGICAL_TEST_CABLE_DUT_WIDGET_H
#define MDT_TT_LOGICAL_TEST_CABLE_DUT_WIDGET_H

#include "ui_mdtTtLogicalTestCableDutWidget.h"
#include <QGroupBox>
#include <QVariant>
#include <QSqlDatabase>

class mdtError;

/*! \brief Helper class for logical test cable generation
 */
class mdtTtLogicalTestCableDutWidget : public QGroupBox, Ui::mdtTtLogicalTestCableDutWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtLogicalTestCableDutWidget(QWidget * parent, QSqlDatabase db);

  /*! \brief Set test cable connector
   */
  void setTestCableConnector(const QVariant & connectorId, const QString & name);

  /*! \brief Set test cable connection
   */
  void setTestCableConnection(const QVariant & connectionId, const QString & name);

 private slots:

  /*! \brief Select DUT
   */
  void selectDut();

  /*! \brief Select connector or connection
   */
  void selectCn();

 private:

  /*! \brief Select DUT connector
   */
  void selectDutConnector();

  /*! \brief Select DUT connection
   */
  void selectDutConnection();

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  // CN type enum
  enum cnType_t{
    Connector,
    Connection
  };

  Q_DISABLE_COPY(mdtTtLogicalTestCableDutWidget);

  QVariant pvTestCableCnId;
  QVariant pvUnitId;
  cnType_t pvCnType;
  QVariant pvCnId;
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_TT_LOGICAL_TEST_CABLE_DUT_WIDGET_H
