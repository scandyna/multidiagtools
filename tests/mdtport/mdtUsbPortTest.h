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
#ifndef MDT_USB_PORT_TEST_H
#define MDT_USB_PORT_TEST_H

#include "mdtTest.h"

// Pour les essais ...
#include <libusb-1.0/libusb.h>

/// \todo Add start/stop test
class mdtUsbPortTest : public mdtTest
{
 Q_OBJECT

 public:

  static void transfertCallback(libusb_transfer *transfer);

 private slots:

  // play
  void essais();
};

#endif // MDT_USB_PORT_TEST_H
