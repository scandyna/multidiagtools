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
#include "mdtTtValueLimitsTest.h"
#include "mdtTtValueLimits.h"
#include "mdtTtValueLimitsWidget.h"
#include "mdtTtValueEditionDialog.h"
#include "mdtApplication.h"
#include <QString>
#include <QVariant>
#include <limits>

#include <QDebug>

void mdtTtValueLimitsTest::simpleSetGetTest()
{
  mdtTtValueLimits vl;

  /*
   * Initial state
   */
  QVERIFY(vl.leftBottomLimit().isNull());
  QVERIFY(vl.leftBottomLimitVar().isNull());
  QVERIFY(vl.leftTopLimit().isNull());
  QVERIFY(vl.leftTopLimitVar().isNull());
  QVERIFY(vl.rightBottomLimit().isNull());
  QVERIFY(vl.rightBottomLimitVar().isNull());
  QVERIFY(vl.rightTopLimit().isNull());
  QVERIFY(vl.rightTopLimitVar().isNull());
  QVERIFY(!vl.isSet());
  QVERIFY(!vl.isValid(0.0));
  QVERIFY(vl.getResult(0.0) == mdtTtValueLimits::Result_t::Undefined);
  /*
   * Set limits and check
   */
  // Set limits
  vl.setLeftBottomLimit(-10.0);
  QVERIFY(!vl.isSet());
  vl.setLeftTopLimit(-5.0);
  QVERIFY(!vl.isSet());
  vl.setRightBottomLimit(1.0);
  QVERIFY(!vl.isSet());
  vl.setRightTopLimit(2.0);
  QVERIFY(vl.isSet());
  // Check
  QCOMPARE(vl.leftBottomLimit().value(), -10.0);
  QCOMPARE(vl.leftTopLimit().value(), -5.0);
  QCOMPARE(vl.rightBottomLimit().value(), 1.0);
  QCOMPARE(vl.rightTopLimit().value(), 2.0);
  // Check - QVariant getters
  QCOMPARE(vl.leftBottomLimitVar(), QVariant(-10.0));
  QCOMPARE(vl.leftTopLimitVar(), QVariant(-5.0));
  QCOMPARE(vl.rightBottomLimitVar(), QVariant(1.0));
  QCOMPARE(vl.rightTopLimitVar(), QVariant(2.0));
  /*
   * Set limits - with infinity
   */
  // Set left bottom limit
  vl.setLeftBottomLimitToMinusInfinity();
  // Check
  QCOMPARE(vl.leftBottomLimitVar(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.leftTopLimitVar(), QVariant(-5.0));
  QCOMPARE(vl.rightBottomLimitVar(), QVariant(1.0));
  QCOMPARE(vl.rightTopLimitVar(), QVariant(2.0));
  // Set left top limit
  vl.setLeftTopLimitToMinusInfinity();
  QCOMPARE(vl.leftBottomLimitVar(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.leftTopLimitVar(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.rightBottomLimitVar(), QVariant(1.0));
  QCOMPARE(vl.rightTopLimitVar(), QVariant(2.0));
  // Set right top limit - must be set before bottom limit
  vl.setRightTopLimitToPlusInfinity();
  QCOMPARE(vl.leftBottomLimitVar(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.leftTopLimitVar(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.rightBottomLimitVar(), QVariant(1.0));
  QCOMPARE(vl.rightTopLimitVar(), QVariant(std::numeric_limits<double>::infinity()));
  // Set right bottom limit
  vl.setRightBottomLimitToPlusInfinity();
  QCOMPARE(vl.leftBottomLimitVar(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.leftTopLimitVar(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.rightBottomLimitVar(), QVariant(std::numeric_limits<double>::infinity()));
  QCOMPARE(vl.rightTopLimitVar(), QVariant(std::numeric_limits<double>::infinity()));
  /*
   * Check clear
   */
  vl.clear();
  QVERIFY(!vl.isSet());
  /*
   * Check set limits at once
   */
  vl.setLimits(8.0, 9.0, 11.0, 12.0);
  QVERIFY(vl.isSet());
  // Check
  QCOMPARE(vl.leftBottomLimit().value(), 8.0);
  QCOMPARE(vl.leftTopLimit().value(), 9.0);
  QCOMPARE(vl.rightBottomLimit().value(), 11.0);
  QCOMPARE(vl.rightTopLimit().value(), 12.0);
}

void mdtTtValueLimitsTest::simpleSetGetBenchmark()
{
  mdtTtValueLimits vl;
  bool isValid;
  mdtTtValueLimits::Result_t result;

  vl.setLimits(8.0, 9.0, 11.0, 12.0);
  QVERIFY(vl.isSet());
  QBENCHMARK{
    isValid = vl.isValid(10.0);
    result = vl.getResult(10.76);
  }
  QVERIFY(isValid);
  QVERIFY(result == mdtTtValueLimits::Result_t::Ok);
}

void mdtTtValueLimitsTest::voltageLimitsTest()
{
  mdtTtValueLimits vl;

  /*
   * Initial state
   */
  QVERIFY(!vl.isSet());
  QVERIFY(!vl.isValid(0.0));
  /*
   * Check with limits:
   *  ]-inf;0[ : fail
   *  [0;10]: Ok
   *  ]10;12]: limit
   *  ]12;inf[: fail
   */
  // Setup limits
  vl.setLeftBottomLimit(0.0);
  vl.setLeftTopLimit(0.0);
  vl.setRightBottomLimit(10.0);
  vl.setRightTopLimit(12.0);
  QVERIFY(vl.isSet());
  // Check that limits are valid for values in [0;10]
  QVERIFY(vl.isValid(5.0));
  QVERIFY(vl.isValid(0.0));
  QVERIFY(vl.isValid(10.0));
  // Check that limits are not valid for values out of Ok range
  QVERIFY(!vl.isValid(-1.0));
  QVERIFY(!vl.isValid(11.0));
  // Check some values
  QVERIFY(vl.getResult(-std::numeric_limits<double>::infinity()) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(-50.0) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(-0.1) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(0.0) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(10.0) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(10.1) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(12.0) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(12.1) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(40.0) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(std::numeric_limits<double>::infinity()) == mdtTtValueLimits::Result_t::Fail);
  /*
   * Check with limits:
   *  ]-inf;2.3[ : fail
   *  [2.3;2.4[: limit
   *  [2.4;2.6]: Ok
   *  ]2.6;2.7]: limit
   *  ]2.7;inf[: fail
   */
  // Setup limits
  vl.setLimits(2.3, 2.4, 2.6, 2.7);
  QVERIFY(vl.isSet());
  // Check that limits are valid for values in [2.4;2.6]
  QVERIFY(vl.isValid(2.4));
  QVERIFY(vl.isValid(2.5));
  QVERIFY(vl.isValid(2.6));
  // Check that limits are not valid for values out of Ok range
  QVERIFY(!vl.isValid(2.3));
  QVERIFY(!vl.isValid(2.7));
  // Check some values
  QVERIFY(vl.getResult(-std::numeric_limits<double>::infinity()) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(2.29) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(2.3) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(2.39) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(2.4) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(2.5) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(2.6) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(2.61) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(2.7) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(2.71) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(30.0) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(std::numeric_limits<double>::infinity()) == mdtTtValueLimits::Result_t::Fail);
}

void mdtTtValueLimitsTest::continuityLimitsTest()
{
  mdtTtValueLimits vl;

  /*
   * Check with limits:
   *  ]-inf;0[: fail
   *  [0;1]: Ok
   *  ]1;5]: Limit
   *  ]5;inf[: fail
   */
  // Setup limits
  vl.setLimits(0.0, 0.0, 1.0, 5.0);
  QVERIFY(vl.isSet());
  // Check that limits are valid for values in [0.0;1.0]
  QVERIFY(vl.isValid(0.0));
  QVERIFY(vl.isValid(0.5));
  QVERIFY(vl.isValid(1.0));
  // Check that limits are not valid for values out of Ok range
  QVERIFY(!vl.isValid(-0.1));
  QVERIFY(!vl.isValid(1.1));
  // Check some values
  QVERIFY(vl.getResult(-0.1) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(0.0) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(0.5) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(1.0) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(1.1) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(4.9) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(5.0) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(40.0) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(1e3) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(std::numeric_limits<double>::infinity()) == mdtTtValueLimits::Result_t::Fail);
}

void mdtTtValueLimitsTest::isolationLimitsTest()
{
  mdtTtValueLimits vl;

  /*
   * Check with limits:
   *  [-inf;10k[: fail
   *  [10k;1M]: Limit
   *  ]1M;inf[: Ok
   */
  // Setup limits
  vl.setLeftBottomLimit(10e3);
  vl.setLeftTopLimit(1e6);
  vl.setRightBottomLimitToPlusInfinity();
  vl.setRightTopLimitToPlusInfinity();
  QVERIFY(vl.isSet());
  // Check that limits are valid for values in [1M;+infinity]
  QVERIFY(vl.isValid(1e6));
  QVERIFY(vl.isValid(1e9));
  QVERIFY(vl.isValid(std::numeric_limits<double>::infinity()));
  // Check that limits are not valid for values out of Ok range
  QVERIFY(!vl.isValid(1.0));
  QVERIFY(!vl.isValid(10e3));
  // Check some values
  QVERIFY(vl.getResult(-std::numeric_limits<double>::infinity()) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(-1.0) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(0.0) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(1e2) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(9.99e3) == mdtTtValueLimits::Result_t::Fail);
  QVERIFY(vl.getResult(10e3) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(10e4) == mdtTtValueLimits::Result_t::Limit);
  QVERIFY(vl.getResult(1e6) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(1e9) == mdtTtValueLimits::Result_t::Ok);
  QVERIFY(vl.getResult(std::numeric_limits<double>::infinity()) == mdtTtValueLimits::Result_t::Ok);
}

void mdtTtValueLimitsTest::limitsWidgetTest()
{
  mdtTtValueLimitsWidget vlw;
  mdtTtValueLimits limits;

  /*
   * Initial state
   */
  vlw.show();
  QVERIFY(!vlw.limits().isSet());
  QVERIFY(vlw.lbLeftFailZone->isVisible());
  QVERIFY(vlw.lbLeftLimitRange->isVisible());
  QVERIFY(vlw.lbOkZone->isVisible());
  QVERIFY(vlw.lbRightLimitRange->isVisible());
  QVERIFY(vlw.lbRightFailZone->isVisible());
  /*
   * Set limits for a voltage check
   */
  limits.setLimits(2.3, 2.4, 2.6, 2.7);
  vlw.setUnit("V");
  vlw.setLimits(limits);
  QVERIFY(vlw.limits().isSet());
  QVERIFY(vlw.lbLeftFailZone->isVisible());
  QVERIFY(vlw.lbLeftLimitRange->isVisible());
  QVERIFY(vlw.lbOkZone->isVisible());
  QVERIFY(vlw.lbRightLimitRange->isVisible());
  QVERIFY(vlw.lbRightFailZone->isVisible());
  /*
   * Set limits for a continuity check
   */
  limits.setLimits(0.0, 0.0, 1.0, 2.0);
  vlw.setUnit("Ohm");
  vlw.setLimits(limits);
  QVERIFY(vlw.limits().isSet());
  QVERIFY(vlw.lbLeftFailZone->isVisible());
  QVERIFY(!vlw.lbLeftLimitRange->isVisible());
  QVERIFY(vlw.lbOkZone->isVisible());
  QVERIFY(vlw.lbRightLimitRange->isVisible());
  QVERIFY(vlw.lbRightFailZone->isVisible());
  /*
   * Set limits for a isolation check
   */
  limits.clear();
  limits.setLeftBottomLimit(10e3);
  limits.setLeftTopLimit(1e6);
  limits.setRightBottomLimitToPlusInfinity();
  limits.setRightTopLimitToPlusInfinity();
  vlw.setUnit("Ohm");
  vlw.setLimits(limits);
  QVERIFY(vlw.limits().isSet());
  QVERIFY(vlw.lbLeftFailZone->isVisible());
  QVERIFY(vlw.lbLeftLimitRange->isVisible());
  QVERIFY(vlw.lbOkZone->isVisible());
  QVERIFY(!vlw.lbRightLimitRange->isVisible());
  QVERIFY(!vlw.lbRightFailZone->isVisible());

  /*
   * Play
   */
  /*
  while(vlw.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtTtValueLimitsTest::valueEditionDialogTest()
{
  mdtTtValueEditionDialog ved;
  mdtTtValueLimits limits;

  limits.setLimits(2.3, 2.4, 2.6, 2.7);
  ved.setLimits(limits);
  ved.setValue(2.5);
  ved.exec();
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtTtValueLimitsTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
