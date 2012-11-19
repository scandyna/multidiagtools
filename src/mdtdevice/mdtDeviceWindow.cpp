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
#include "mdtDeviceWindow.h"
#include "mdtDeviceIosWidget.h"
#include "mdtDeviceStatusWidget.h"

#include <QPushButton>

mdtDeviceWindow::mdtDeviceWindow(QWidget *parent)
 : QMainWindow(parent)
{
  setupUi(this);

  pvStatusWidget = new mdtDeviceStatusWidget;
  statusBar()->addWidget(pvStatusWidget);
}

mdtDeviceWindow::~mdtDeviceWindow()
{
}

void mdtDeviceWindow::setDevice(mdtDevice *device)
{
  Q_ASSERT(device != 0);

  // Status widget
  pvStatusWidget->setDevice(device);
}

void mdtDeviceWindow::setIosWidget(QWidget *widget)
{
  saIos->setWidget(widget);
}
