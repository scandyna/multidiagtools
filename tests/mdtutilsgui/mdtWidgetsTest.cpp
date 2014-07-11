/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtWidgetsTest.h"
#include "mdtApplication.h"
#include "mdtDoubleEdit.h"
#include "mdtDoubleValidator.h"
#include <QString>
#include <QVariant>
#include <QValidator>

#include <QDebug>

void mdtWidgetsTest::mdtDoubleValidatorTest()
{
  mdtDoubleValidator v;
  QString str;
  int pos = 0;

  // Check some basic validation
  str = "1.25";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1.2e5";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "-1.3";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "-1.7";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  // Check sequential input
  str = "";
  QCOMPARE(v.validate(str, pos), QValidator::Intermediate);
  str = "-";
  QCOMPARE(v.validate(str, pos), QValidator::Intermediate);
  str = "-1";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "-1 ";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "-1 m";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "-1e";
  QCOMPARE(v.validate(str, pos), QValidator::Intermediate);
  str = "-1e5";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "-1e5 m";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  // Check power of 10 chars
  str = "1 a";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 f";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 p";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 n";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 u";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 m";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 c";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 d";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 h";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 k";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 M";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 G";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 T";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 P";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 E";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  // Invalid input
  str = "1 R";
  QCOMPARE(v.validate(str, pos), QValidator::Invalid);
  str = "R";
  QCOMPARE(v.validate(str, pos), QValidator::Invalid);
  str = "m";
  QCOMPARE(v.validate(str, pos), QValidator::Invalid);
  str = "e";
  QCOMPARE(v.validate(str, pos), QValidator::Invalid);
  str = "R1";
  QCOMPARE(v.validate(str, pos), QValidator::Invalid);
  // Check with suffix
  v.setSuffix("V");
  str = "1.5e3 mV";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);

}

void mdtWidgetsTest::mdtDoubleEditTest()
{
  mdtDoubleEdit e;

  e.setUnit("V");
  e.show();

  while(e.isVisible()){
    QTest::qWait(1000);
  }
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtWidgetsTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
