/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_TEST_H
#define MDT_TEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <iostream>
#include <string>
#include "config.h"

// QVERIFY variant macro
#define VERIFY(statement, description) mdtTest::verify((statement), #statement, description, __FILE__, __LINE__);

/*! \brief Compare 2 floats with considering n bits of resolution for a range from min to max
 */
#define MDT_COMPARE(x, y, n, min, max) mdtTest::compare(x, y, n, min, max, __FILE__, __LINE__);

using namespace std;

class mdtTest : public QObject
{
 Q_OBJECT

 public:

  mdtTest();
  ~mdtTest();

  /*! \brief Verify a statement
   *
   */
  static bool verify(bool statement, const char *statementStr, const char *description, const char *file, int line);

  /*!  \brief get a random value between min and max
   * 
   */
  void randomValueInit();
  int randomValue(int min, int max);

  /*! \brief Compare 2 floats with considering n bits of resolution
   */
  static void compare(double x, double y, int n, double min, double max, const char *file, int line);
};

#endif  // ifdef MDT_TEST_H
