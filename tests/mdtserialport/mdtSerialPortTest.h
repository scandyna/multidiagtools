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
#ifndef MDT_SERIAL_PORT_TEST_H
#define MDT_SERIAL_PORT_TEST_H

#include "mdtTest.h"

class mdtSerialPortTest : public mdtTest
{
 Q_OBJECT
 
 private slots:
  
  void essais();
  
  // Serial port manager tests
  void mdtSerialPortManagerTest();
  
  // Test the mdtSerialPortConfig class
  void mdtSerialPortConfigTest();
  void mdtSerialPortConfigTest_data();

  // Test start and stop
  void mdtSerialPortStartStopTest();

  // Test the control signals (modem lines)
  void mdtSerialPortCtlSignalsTest();

  // Test the data transmission: RAW
  void mdtSerialPortTxRxBinaryTest();
  void mdtSerialPortTxRxBinaryTest_data();

  // Test the data transmission: timeout protocol
  void mdtSerialPortTxRxBinaryTopTest();
  void mdtSerialPortTxRxBinaryTopTest_data();

  // Test the data transmission: ASCII
  void mdtSerialPortTxRxAsciiTest();
  void mdtSerialPortTxRxAsciiTest_data();

signals:
  void testSignal(bool on);
};

#endif  // #ifndef MDT_SERIAL_PORT_TEST_H
