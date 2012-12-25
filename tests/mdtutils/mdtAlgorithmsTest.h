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
#ifndef MDT_ALGORITHMS_TEST_H
#define MDT_ALGORITHMS_TEST_H

#include "mdtTest.h"

class mdtAlgorithmsTest : public mdtTest
{
 Q_OBJECT
 
 private slots:

  void extractNumericPartAtEndTest();
  void extractNumericPartAtEndTest_data();

  void stringWithNumericEndLessThanTest();
  void stringWithNumericEndLessThanTest_data();

  void sortStringListWithNumericEndTest();
  void sortStringListWithNumericEndTest_data();

  // Test hex string <-> byte array conversions
  void hexStringByteArrayTest();

  // Split string test
  void splitStringTest();
};

#endif  // #ifndef MDT_ALGORITHMS_TEST_H
