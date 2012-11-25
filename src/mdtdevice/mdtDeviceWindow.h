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
#ifndef MDT_DEVICE_WINDOW_H
#define MDT_DEVICE_WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "mdtDevice.h"
#include "ui_mdtDeviceWindow.h"

class mdtDeviceStatusWidget;

/*! \brief
 */
class mdtDeviceWindow : public QMainWindow, public Ui::mdtDeviceWindow
{
 Q_OBJECT

 public:

  mdtDeviceWindow(QWidget *parent = 0);

  ~mdtDeviceWindow();

  /*! \brief Set the device
   *
   * \pre device must be a valid pointer.
   */
  void setDevice(mdtDevice *device);

  /*! \brief Set the I/O's widget
   */
  void setIosWidget(QWidget *widget);

 private:

  Q_DISABLE_COPY(mdtDeviceWindow);

  mdtDeviceStatusWidget *pvStatusWidget;
  mdtDevice *pvDevice;
};

#endif  // #ifndef MDT_DEVICE_WINDOW_H
