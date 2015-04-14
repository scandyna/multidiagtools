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
#include "mdtApplication.h"
#include <QString>
#include <QVariant>

#include <QDebug>

void mdtTtValueLimitsTest::voltageLimitsTest()
{
  mdtTtValueLimits vl;

  /*
   * Initial state
   */
  QVERIFY(vl.result() == mdtTtValueLimits::Result_t::Undefined);

  /*
   * Check with limits:
   *  ]-inf;0[ : fail
   *  [0;10]: Ok
   *  ]10;12]: limit
   *  ]12;inf[: fail
   */
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
}

void mdtTtValueLimitsTest::limitsWidgetTest()
{
  mdtTtValueLimitsWidget vlw;

  /*
   * Initial state
   */
  vlw.show();
  QVERIFY(vlw.lbLeftFailZone->isVisible());
  ///QVERIFY(vlw.lbLeftLimitRange->isVisible());
  QVERIFY(vlw.lbOkZone->isVisible());
  ///QVERIFY(vlw.lbRightLimitRange->isVisible());
  QVERIFY(vlw.lbRightFailZone->isVisible());

  /*
   * Play
   */
  while(vlw.isVisible()){
    QTest::qWait(500);
  }
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
