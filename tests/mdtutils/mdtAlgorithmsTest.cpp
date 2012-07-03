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
#include "mdtAlgorithmsTest.h"
#include "mdtAlgorithms.h"

#include <QTest>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include "mdtApplication.h"

using namespace mdtAlgorithms;

void mdtAlgorithmsTest::extractNumericPartAtEndTest()
{
  QFETCH(QString, input);
  QFETCH(QString, refOutput);
  
  QCOMPARE(extractNumericPartAtEnd(input), refOutput);
}

void mdtAlgorithmsTest::extractNumericPartAtEndTest_data()
{
  QTest::addColumn<QString>("input");
  QTest::addColumn<QString>("refOutput");
  
  QTest::newRow("Empty string") << "" << "";
  QTest::newRow("Space") << " " << "";
  QTest::newRow("One char") << "a" << "";
  QTest::newRow("One char and num") << "a1" << "1";
  QTest::newRow("Space at end") << "a1 " << "1";
  QTest::newRow("Spaces at end") << "a1  " << "1";
  QTest::newRow("Combinaisons") << "a 1" << "1";
  QTest::newRow("Combinaisons") << "adkkjsj1" << "1";
  QTest::newRow("Combinaisons") << "adkkjsj12" << "12";
  QTest::newRow("Combinaisons") << "abc5def52" << "52";
  QTest::newRow("Paths") << "/dev/ttyS0" << "0";
}

void mdtAlgorithmsTest::stringWithNumericEndLessThanTest()
{
  QFETCH(QString, input1);
  QFETCH(QString, input2);
  QFETCH(bool, refOutput);

  QCOMPARE(stringWithNumericEndLessThan(input1, input2), refOutput);
}

void mdtAlgorithmsTest::stringWithNumericEndLessThanTest_data()
{
  QTest::addColumn<QString>("input1");
  QTest::addColumn<QString>("input2");
  QTest::addColumn<bool>("refOutput");

  QTest::newRow("Empty strings") << "" << "" << false;
  QTest::newRow("Empty strings") << "" << "1" << true;
  QTest::newRow("Empty strings") << "1" << "" << false;
  QTest::newRow("Numbers") << "1" << "2" << true;
  QTest::newRow("Numbers") << "1" << "1" << false;
  QTest::newRow("Numbers") << "2" << "1" << false;
  //QTest::newRow("Numbers") << "a" << "1" << false;
  //QTest::newRow("Numbers") << "1" << "a" << true;
  // Some other values
  QTest::newRow("Numbers") << "1" << "10254" << true;
}

void mdtAlgorithmsTest::sortStringListWithNumericEndTest()
{
  QFETCH(QStringList, input);
  QFETCH(QStringList, refOutput);
  
  QCOMPARE(sortStringListWithNumericEnd(input), refOutput);
}

void mdtAlgorithmsTest::sortStringListWithNumericEndTest_data()
{
  QTest::addColumn<QStringList>("input");
  QTest::addColumn<QStringList>("refOutput");

  QTest::newRow("Empty strings") << QStringList("") << QStringList("");
  QTest::newRow("Alpha strings") << QStringList("a") << QStringList("a");
  QTest::newRow("Alpha strings") << QString("a;b").split(";") << QString("a;b").split(";");
  QTest::newRow("Alpha strings") << QString("b;a").split(";") << QString("a;b").split(";");
  QTest::newRow("AlphaNum strings") << QString("a1;b1").split(";") << QString("a1;b1").split(";");
  QTest::newRow("AlphaNum strings") << QString("b1;a1").split(";") << QString("a1;b1").split(";");
  QTest::newRow("AlphaNum strings") << QString("a2;a10;a1").split(";") << QString("a1;a2;a10").split(";");
  QTest::newRow("AlphaNum strings") << QString("a5s2;a5s10;a5s1").split(";") << QString("a5s1;a5s2;a5s10").split(";");
  QTest::newRow("Paths") << QString("/dev/ttyS0;/dev/ttyS15;/dev/ttyS10;/dev/ttyS5").split(";") << QString("/dev/ttyS0;/dev/ttyS5;/dev/ttyS10;/dev/ttyS15").split(";");
  // Found bug whenn, f.ex. ttyS0, ttyMXUSB0, ... connected
  QTest::newRow("Paths") << QString("/dev/ttyS0;/dev/ttyS15;/dev/ttyMXUSB1;/dev/ttyS10;/dev/ttyMXUSB10;/dev/ttyMXUSB0;/dev/ttyMXUSB2").split(";") \
                         << QString("/dev/ttyMXUSB0;/dev/ttyMXUSB1;/dev/ttyMXUSB2;/dev/ttyMXUSB10;/dev/ttyS0;/dev/ttyS10;/dev/ttyS15").split(";");
}

void mdtAlgorithmsTest::hexStringByteArrayTest()
{
  QString hexString;
  QByteArray byteArray;

  // Check HEX string to byteArray conversion
  hexString = " 00 12 F2  FF 09 ";
  // Reference byteArray
  byteArray.clear();
  byteArray.append((char)0);
  byteArray.append((char)18);
  byteArray.append((char)242);
  byteArray.append((char)255);
  byteArray.append((char)9);
  QVERIFY(hexStringToByteArray(hexString) == byteArray);
  // Check lower case
  hexString = " 00 12 f2  ff 09 ";
  QVERIFY(hexStringToByteArray(hexString) == byteArray);
  // Check when end of line char exists at end
  hexString = " 00 12 F2  FF 09 ";
  hexString.append((char)0x0D);
  hexString.append((char)0x0A);
  QVERIFY(hexStringToByteArray(hexString) == byteArray);
  // Check when end of line char exists in the middle
  hexString = " 00 12 F2";
  hexString.append((char)0x0D);
  hexString.append((char)0x0A);
  hexString.append("FF 09");
  QVERIFY(hexStringToByteArray(hexString) == byteArray);

  // Check byte array to HEX string conversion
  hexString = "";
  QVERIFY(byteArrayToHexString(byteArray) == "00 12 f2 ff 09");
  // Check upper case
  QVERIFY(byteArrayToHexString(byteArray).toUpper() == "00 12 F2 FF 09");
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtAlgorithmsTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
