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
#ifndef MDT_PORT_TEST_H
#define MDT_PORT_TEST_H

#include "mdtTest.h"

class mdtPortTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Open/Close and check state
  void openCloseTest();

  // Test start and stop
  void startStopTest();

  // Check that write works
  void writeRawTest();
  void writeRawTest_data();
  void writeAsciiTest();
  void writeAsciiTest_data();

  // Check that read works
  void readRawTest();
  void readRawTest_data();
  void readAsciiTest();
  void readAsciiTest_data();

  // Check that read works with invalid frames
  void readInvalidDataAsciiTest();

  // Check that recovery works whenn a frame pool was empty for some time
  void emptyQueueRecoveryTest();
};

#endif  // #ifndef MDT_PORT_TEST_H
