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
#ifndef MDT_FRAME_CODEC_TEST_H
#define MDT_FRAME_CODEC_TEST_H


#include "mdtTest.h"

class mdtFrameCodecTest : public mdtTest
{
 Q_OBJECT
 
 private slots:

  // Check some ASCII decoding
  void asciiDecodeTest();

  // Check SCPI
  void scpiDecodeTest();

  // Check Agilent U3606A SCPI
  void scpiU3606ATest();

  // Check mdtFrameCodecModbus encode/decode
  void mdtFrameCodecModbusTest();

  // Check Velleman k8055 codec
  void mdtFrameCodecK8055Test();
};

#endif  // #ifndef MDT_FRAME_CODEC_TEST_H
