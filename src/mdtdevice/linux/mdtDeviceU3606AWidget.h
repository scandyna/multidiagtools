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
#ifndef MDT_DEVICE_U3606A_WIDGET_H
#define MDT_DEVICE_U3606A_WIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include "ui_mdtDeviceU3606AWidget.h"
#include "mdtDeviceU3606A.h"

class mdtDeviceU3606AWidget : public QWidget , private Ui::mdtDeviceU3606AWidget
{
 Q_OBJECT

 public:

  mdtDeviceU3606AWidget(QWidget *parent = 0);

 public slots:

  void essais();
  
  void scan();
  
  void selectPort(const QString &port);

  void displayIdn(const QStringList &data);
  
 private:

  // Set IDN to unknow
  void setDefaulIdn();
  
  mdtDeviceU3606A pvDevice;
};

#endif  // #ifndef MDT_DEVICE_U3606A_WIDGET_H
