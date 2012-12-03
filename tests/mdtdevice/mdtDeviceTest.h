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
#ifndef MDT_DEVICE_TEST_H
#define MDT_DEVICE_TEST_H

#include "mdtTest.h"

class mdtDeviceTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Some basic tests (states, ...)
  void deviceBaseTest();

  // I/O's container test
  void deviceIosTest();

  // I/O's widget container test
  void deviceIosWidgetTest();

  // Some simple coherence tests (no real test possible without MODBUS server)
  void modbusWagoTest();
  void modbusBeckhoffTest();

  // Agilent U3606A test (will be skipped if devic is not connected)
  void usbtmcU3606ATest();

/**
#ifdef Q_OS_UNIX
  // Check U3606A device
  void deviceU3606Atest();
#endif
*/
};

#endif  // #ifndef MDT_DEVICE_TEST_H
