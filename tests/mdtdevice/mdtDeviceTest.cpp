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
#include "mdtDeviceTest.h"
#include "mdtApplication.h"
#include "mdtDeviceModbus.h"

/**
#ifdef Q_OS_UNIX
 #include "linux/mdtDeviceU3606A.h"
#endif
*/

#include <QString>
#include <QVariant>

#include <QDebug>

void mdtDeviceTest::deviceModbusTest()
{
  mdtDeviceModbus d;

  // First setup - Analog inputs
  d.setupAnalogInputs(11);
  QVERIFY(!d.getAnalogInputValue(-1, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false) == 0);
  QVERIFY(d.getAnalogInputValue(10, false) == 0);
  QVERIFY(!d.getAnalogInputValue(11, false).isValid());

  // Second setup - Analog inputs
  d.setupAnalogInputs(5);
  QVERIFY(!d.getAnalogInputValue(-1, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false).isValid());
  QVERIFY(d.getAnalogInputValue(0, false) == 0);
  QVERIFY(d.getAnalogInputValue(4, false) == 0);
  QVERIFY(!d.getAnalogInputValue(5, false).isValid());

  // First setup - Analog outputs
  d.setupAnalogOutputs(11);
  QVERIFY(!d.getAnalogOutputValue(-1, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false) == 0);
  QVERIFY(d.getAnalogOutputValue(10, false) == 0);
  QVERIFY(!d.getAnalogOutputValue(11, false).isValid());
  // Update some values
  QVERIFY(!d.setAnalogOutput(-1, 10, false));
  QVERIFY(d.setAnalogOutput(0, 10, false));
  QVERIFY(d.getAnalogOutputValue(0, false) == 10);
  QVERIFY(d.setAnalogOutput(9, 8, false));
  QVERIFY(d.getAnalogOutputValue(9, false) == 8);

  // Second setup - Analog outputs
  d.setupAnalogOutputs(5);
  QVERIFY(!d.getAnalogOutputValue(-1, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false).isValid());
  QVERIFY(d.getAnalogOutputValue(0, false) == 0);
  QVERIFY(d.getAnalogOutputValue(4, false) == 0);
  QVERIFY(!d.getAnalogOutputValue(5, false).isValid());

  // First setup - Digital inputs
  d.setupDigitalInputs(11);
  QVERIFY(!d.getDigitalInputState(-1, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false) == false);
  QVERIFY(d.getDigitalInputState(10, false) == false);
  QVERIFY(!d.getDigitalInputState(11, false).isValid());

  // Second setup - Digital inputs
  d.setupDigitalInputs(5);
  QVERIFY(!d.getDigitalInputState(-1, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false).isValid());
  QVERIFY(d.getDigitalInputState(0, false) == false);
  QVERIFY(d.getDigitalInputState(4, false) == false);
  QVERIFY(!d.getDigitalInputState(5, false).isValid());

  // First setup - Digital outputs
  d.setupDigitalOutputs(11);
  QVERIFY(!d.getDigitalOutputState(-1, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false) == false);
  QVERIFY(d.getDigitalOutputState(10, false) == false);
  QVERIFY(!d.getDigitalOutputState(11, false).isValid());
  // Update some values
  QVERIFY(!d.setDigitalOutput(-1, true, false));
  QVERIFY(d.setDigitalOutput(0, true, false));
  QVERIFY(d.getDigitalOutputState(0, false) == true);
  QVERIFY(d.setDigitalOutput(9, true, false));
  QVERIFY(d.getDigitalOutputState(9, false) == true);

  // Second setup - Digital outputs
  d.setupDigitalOutputs(5);
  QVERIFY(!d.getDigitalOutputState(-1, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false).isValid());
  QVERIFY(d.getDigitalOutputState(0, false) == false);
  QVERIFY(d.getDigitalOutputState(4, false) == false);
  QVERIFY(!d.getDigitalOutputState(5, false).isValid());

  /// Essais
  QVERIFY(d.setAnalogOutput(0, 0x00FF, false));
  QVERIFY(d.setAnalogOutput(1, 0x00FF, false));
  QVERIFY(d.writeAnalogOutputs());
  QVERIFY(d.setDigitalOutput(0, true, false));
  QVERIFY(d.setDigitalOutput(1, true, false));
  QVERIFY(d.setDigitalOutput(3, true, false));
  QVERIFY(d.writeDigitalOutputs());
  QVERIFY(d.readDigitalInputs());
  QVERIFY(d.readAnalogOutputs());
  QVERIFY(d.readAnalogInputs());
  QVERIFY(d.readDigitalOutputs());
  QTest::qWait(1000);
}

/**
void mdtDeviceTest::deviceU3606Atest()
{
  mdtDeviceU3606A d;
  QStringList ports;
  
  qDebug() << "* A Agilent U3606A must be attached at USB port, else test will fail *";

  // Check that scan and open methods works
  ports = d.scan();
  QVERIFY(ports.size() > 0);
  QVERIFY(d.selectPort(ports, 0));

  d.essais();
}
*/

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDeviceTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

