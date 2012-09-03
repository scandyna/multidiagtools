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
#ifndef MDT_FRAME_TEST_H
#define MDT_FRAME_TEST_H

#include "mdtTest.h"

class mdtFrameTest : public mdtTest
{
 Q_OBJECT
 
 private slots:

  // Verify that initial parameters are correct,
  // such as capacity, ...
  void initTest();

  // Verify putData() function
  void putDataTest();

  // Verify state and values by ASCII frames
  void asciiReceptionTest();
  void asciiReceptionIgnoreNullValuesTest();
  // Same. but the EOF is a string
  void asciiReceptionEofStrTest();
  void asciiReceptionEofStrIgnoreNullValuesTest();

  // Verify states and values by taking data
  void takeDataTest();

  // Check mdtFrameModbusTcp encoding
  void modbusTcpEncodeTest();

  // Check mdtFrameModbusTcp decoding
  void modbusTcpDecodeTest();

  // Check mdtFrameUsbTmc encoding
  void usbTmcEncodeTest();

  // Check mdtFrameUsbTmc decoding
  void usbTmcDecodeTest();

  // Some special cases seen in real world
  void usbTmcDecodeRealCasesTest();
  
  /// \todo Add size caluclation test
};
#endif // idnfef MDT_FRAME_TEST_H