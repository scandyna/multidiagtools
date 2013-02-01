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
#ifndef MDT_SERIAL_PORT_CTL_WIDGET_H
#define MDT_SERIAL_PORT_CTL_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "mdtLed.h"
#include "mdtSerialPortManager.h"

class mdtSerialPortCtlWidget : public QWidget
{
 Q_OBJECT

 public:

  mdtSerialPortCtlWidget(QWidget *parent = 0);

  /*! \brief Make the signals/slots connections
   * 
   * \pre manager must be a valid pointer
   */
  void makeConnections(mdtSerialPortManager *manager);

 private slots:

  // Update buttons text
  void setDtrText(bool on);
  void setRtsText(bool on);

 private:

  QLabel *lbCar;
  QLabel *lbRng;
  QLabel *lbDsr;
  QLabel *lbCts;
  QLabel *lbDtr;
  QLabel *lbRts;
  mdtLed *ldCar;
  mdtLed *ldRng;
  mdtLed *ldDsr;
  mdtLed *ldCts;
  QPushButton *pbDtr;
  QPushButton *pbRts;
};

#endif  // #ifndef MDT_SERIAL_PORT_CTL_WIDGET_H
