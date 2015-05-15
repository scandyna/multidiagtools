/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_MATH_TEST_H
#define MDT_MATH_TEST_H

#include "mdtTest.h"

class mdtMathTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void sandbox();

  void advanceIteratorTest();

  void productTest();
  void productTest_data();
  void productBenchamrk();
  void productBenchamrk_data();

  void arithmeticMeanTest();
  void arithmeticMeanTest_data();
  void arithmeticMeanBenchmark();
  void arithmeticMeanBenchmark_data();

  void geometricMeanTest();
  void geometricMeanTest_data();
  void geometricMeanBenchmark();
  void geometricMeanBenchmark_data();

  void sumOfSquaredDifferencesTest();
  void sumOfSquaredDifferencesTest_data();
  void sumOfSquaredDifferencesBenchmark();
  void sumOfSquaredDifferencesBenchmark_data();

  void varianceTest();
  void varianceTest_data();
  void varianceBenchmark();
  void varianceBenchmark_data();

  void uncorrectedStandardDeviationTest();
  void uncorrectedStandardDeviationTest_data();
  void uncorrectedStandardDeviationBenchmark();
  void uncorrectedStandardDeviationBenchmark_data();

  void correctedStandardDeviationTest();
  void correctedStandardDeviationTest_data();
  void correctedStandardDeviationBenchmark();
  void correctedStandardDeviationBenchmark_data();
};

#endif // #ifndef MDT_MATH_TEST_H
