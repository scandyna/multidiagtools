/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef _MDTDEVICESTATUSWIDGET_H
#define _MDTDEVICESTATUSWIDGET_H

#include <QWidget>
#include <QString>
#include "mdtDevice.h"

class mdtBlinkLed;
class QLabel;
class QPushButton;

/*! \brief
 */
class mdtDeviceStatusWidget : public QWidget
{
  Q_OBJECT

 public:

  mdtDeviceStatusWidget(QWidget *parent = 0);
  ~mdtDeviceStatusWidget();

  /*! \brief Set device
   *
   * Will make some connections
   */
  void setDevice(mdtDevice *device);

 public slots:

  /*! \brief
   */
  void setState(int state);

  /*! \brief
   */
  void setState(int state, const QString &message, const QString &details);

 private:

  Q_DISABLE_COPY(mdtDeviceStatusWidget);

  mdtBlinkLed *ldState;
  QPushButton *pbDetails;
  QLabel *lbMessage;
  QLabel *lbTx;
  QLabel *lbRx;
  mdtBlinkLed *ldTx;
  mdtBlinkLed *ldRx;
};
#endif
