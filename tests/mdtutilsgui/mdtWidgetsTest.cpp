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
  // Check with V suffix
  v.setSuffix("V");
  str = "1.5e3 mV";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  v.setSuffix("ohm");
  str = "1";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1400";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1.4 k";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1 ohm";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1400 ohm";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  // Check with m (as meter) suffix
  v.setSuffix("m");
  str = "1.5";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1.5 m";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);
  str = "1.5 mm";
  QCOMPARE(v.validate(str, pos), QValidator::Acceptable);



}

void mdtWidgetsTest::mdtDoubleEditTest()
{
  mdtDoubleEdit e;

  e.show();

  // Check some basic conversions
  e.setValue("1.0");
  QVERIFY(e.valueIsValid());
  QVERIFY(e.value().isValid());
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.valueDouble(), 1.0);
  QCOMPARE(e.text().trimmed(), QString("1"));
  e.setValue("-inf");
  QVERIFY(e.valueIsValid());
  QVERIFY(e.value().isValid());
  QCOMPARE(e.value(), QVariant(-std::numeric_limits<double>::infinity()));
  QCOMPARE(QVariant(e.valueDouble()), QVariant(-std::numeric_limits<double>::infinity()));  // Direct double compare fails here
  QCOMPARE(e.text().trimmed(), QString("-" + e.infinityString()));
  e.setValue("inf");
  QVERIFY(e.valueIsValid());
  QVERIFY(e.value().isValid());
  QCOMPARE(e.value(), QVariant(std::numeric_limits<double>::infinity()));
  QCOMPARE(QVariant(e.valueDouble()), QVariant(std::numeric_limits<double>::infinity()));  // Direct double compare fails here
  QCOMPARE(e.text().trimmed(), e.infinityString());
  // Check invalid value setting
  e.setValue("x");
  QVERIFY(!e.valueIsValid());
  QVERIFY(!e.value().isValid());
  QCOMPARE(e.value(), QVariant());
  QCOMPARE(e.valueDouble(), 0.0);
  QCOMPARE(e.text().trimmed(), QString(""));
  // Check scientific suffixes - simple part
  e.setValue("1 a");
  QCOMPARE(e.value(), QVariant(1e-18));
  QCOMPARE(e.text().trimmed(), QString("1 a"));
  e.setValue("1 f");
  QCOMPARE(e.value(), QVariant(1e-15));
  QCOMPARE(e.text().trimmed(), QString("1 f"));
  e.setValue("1 p");
  QCOMPARE(e.value(), QVariant(1e-12));
  QCOMPARE(e.text().trimmed(), QString("1 p"));
  e.setValue("1 n");
  QCOMPARE(e.value(), QVariant(1e-9));
  QCOMPARE(e.text().trimmed(), QString("1 n"));
  e.setValue("1 u");
  QCOMPARE(e.value(), QVariant(1e-6));
  QCOMPARE(e.text().trimmed(), QString("1 u"));
  e.setValue("1 m");
  QCOMPARE(e.value(), QVariant(1e-3));
  QCOMPARE(e.text().trimmed(), QString("1 m"));
  e.setValue("1");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1"));
  e.setValue("1 k");
  QCOMPARE(e.value(), QVariant(1e3));
  QCOMPARE(e.text().trimmed(), QString("1 k"));
  e.setValue("1 M");
  QCOMPARE(e.value(), QVariant(1e6));
  QCOMPARE(e.text().trimmed(), QString("1 M"));
  e.setValue("1 G");
  QCOMPARE(e.value(), QVariant(1e9));
  QCOMPARE(e.text().trimmed(), QString("1 G"));
  e.setValue("1 T");
  QCOMPARE(e.value(), QVariant(1e12));
  QCOMPARE(e.text().trimmed(), QString("1 T"));
  e.setValue("1 P");
  QCOMPARE(e.value(), QVariant(1e15));
  QCOMPARE(e.text().trimmed(), QString("1 P"));
  e.setValue("1 E");
  QCOMPARE(e.value(), QVariant(1e18));
  QCOMPARE(e.text().trimmed(), QString("1 E"));
  // Check with some units
  e.setUnit("V");
  e.setValue("1");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 V"));
  e.setValue("1200");
  QCOMPARE(e.value(), QVariant(1.2e3));
  QCOMPARE(e.text().trimmed(), QString("1.2 kV"));
  e.setUnit("eV");
  e.setValue("1");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 eV"));
  e.setValue("1300");
  QCOMPARE(e.value(), QVariant(1.3e3));
  QCOMPARE(e.text().trimmed(), QString("1.3 keV"));
  e.setUnit("ohm"); // We use the false ohm, not Ohm, so it will not be converted to Omega
  e.setValue("1");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 ohm"));
  e.setValue("1400");
  QCOMPARE(e.value(), QVariant(1.4e3));
  QCOMPARE(e.text().trimmed(), QString("1.4 kohm"));
  // 0 value
  e.setUnit("");
  e.setValue(0.0);
  QCOMPARE(e.value(), QVariant(0.0));
  QCOMPARE(e.text().trimmed(), QString("0"));
  // Range
  e.setRange(0.0, 1e6);
  e.setValue(0.0);
  QCOMPARE(e.value(), QVariant(0.0));
  QCOMPARE(e.text().trimmed(), QString("0"));
  e.setValue(-1.0);
  QCOMPARE(e.value(), QVariant(0.0));
  QCOMPARE(e.text().trimmed(), QString("0"));
  e.setValue(1e6);
  QCOMPARE(e.value(), QVariant(1e6));
  QCOMPARE(e.text().trimmed(), QString("1 M"));
  e.setValue(1.1e6);
  QCOMPARE(e.value(), QVariant(1e6));
  QCOMPARE(e.text().trimmed(), QString("1 M"));

  /*
   * Check with meter unit [m]
   */
  // Setup
  e.setUnit("m");
  e.setRange(0.0, 1e6);
  // Set numeric values
  e.setValue(0.0);
  QCOMPARE(e.value(), QVariant(0.0));
  QCOMPARE(e.text().trimmed(), QString("0 m"));
  e.setValue(1.0);
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m"));
  e.setValue(1e-3);
  QCOMPARE(e.value(), QVariant(1e-3));
  QCOMPARE(e.text().trimmed(), QString("1 mm"));
  // Set string values - will be close to user input
  e.setValue("1");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m"));
  e.setValue("1 m");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m"));
  e.setValue("1m");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m"));
  e.setValue("1 mm");
  QCOMPARE(e.value(), QVariant(1e-3));
  QCOMPARE(e.text().trimmed(), QString("1 mm"));
  e.setValue("1 Mm");
  QCOMPARE(e.value(), QVariant(1e6));
  QCOMPARE(e.text().trimmed(), QString("1 Mm"));
  e.setValue("1 um");
  QCOMPARE(e.value(), QVariant(1e-6));
  QCOMPARE(e.text().trimmed(), QString("1 um"));

  /*
   * Check surface unit [m2]
   */
  // Setup
  e.setUnit("m2");
  e.setRange(0.0, 1e6);
  // Set numeric values
  e.setValue(0.0);
  QCOMPARE(e.value(), QVariant(0.0));
  QCOMPARE(e.text().trimmed(), QString("0 m2"));
  e.setValue(1.0);
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m2"));
  e.setValue(1e-3);
  QCOMPARE(e.value(), QVariant(1e-3));
  QCOMPARE(e.text().trimmed(), QString("1000 mm2"));
  e.setValue(1e-6);
  QCOMPARE(e.value(), QVariant(1e-6));
  QCOMPARE(e.text().trimmed(), QString("1 mm2"));

  
  
  // Set string values - will be close to user input
  e.setValue("1 m2");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m2"));
  e.setValue("1m2");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m2"));
  e.setValue("1m2 ");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m2"));
  e.setValue(" 1m2");
  QCOMPARE(e.value(), QVariant(1.0));
  QCOMPARE(e.text().trimmed(), QString("1 m2"));
  e.setValue("1 mm2");
  QCOMPARE(e.value(), QVariant(1e-6));
  QCOMPARE(e.text().trimmed(), QString("1 mm2"));
  e.setValue("1 km2");
  QCOMPARE(e.value(), QVariant(1e6));
  QCOMPARE(e.text().trimmed(), QString("1 km2"));

  
  ///e.setUnit(mdtDoubleEdit::OmegaCapital);

  
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
