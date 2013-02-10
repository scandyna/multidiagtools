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

void mdtAlgorithmsTest::unprotectedStringTest()
{
  int end;

  // Empty data test
  QCOMPARE(unprotectedString("", "", '\0'), QString(""));
  QCOMPARE(unprotectedString("", "'", '\0'), QString(""));
  QCOMPARE(unprotectedString("", "<>", '\0'), QString(""));
  QCOMPARE(unprotectedString("", "", '\\'), QString(""));
  QCOMPARE(unprotectedString("", "'", '\\'), QString(""));
  QCOMPARE(unprotectedString("", "'", '\\', &end), QString(""));
  QCOMPARE(end, -1);

  // Data without escape char
  QCOMPARE(unprotectedString("A", "'", '\0'), QString(""));
  QCOMPARE(unprotectedString("A", "'", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("AB", "'", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("'A'", "'", '\0', &end), QString("A"));
  QCOMPARE(end, 2);
  QCOMPARE(unprotectedString("'AB'", "'", '\0', &end), QString("AB"));
  QCOMPARE(end, 3);
  QCOMPARE(unprotectedString("'ABC'", "'", '\0', &end), QString("ABC"));
  QCOMPARE(end, 4);
  QCOMPARE(unprotectedString("A", "<>", '\0'), QString(""));
  QCOMPARE(unprotectedString("A", "<>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("AB", "<>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("<>A<>", "<>", '\0', &end), QString("A"));
  QCOMPARE(end, 3);
  QCOMPARE(unprotectedString("<>AB<>", "<>", '\0', &end), QString("AB"));
  QCOMPARE(end, 4);
  QCOMPARE(unprotectedString("<>ABC<>", "<>", '\0', &end), QString("ABC"));
  QCOMPARE(end, 5);
  QCOMPARE(unprotectedString("A", "<!>", '\0'), QString(""));
  QCOMPARE(unprotectedString("A", "<!>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("AB", "<!>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("<!>A<!>", "<!>", '\0', &end), QString("A"));
  QCOMPARE(end, 4);
  QCOMPARE(unprotectedString("<!>AB<!>", "<!>", '\0', &end), QString("AB"));
  QCOMPARE(end, 5);
  QCOMPARE(unprotectedString("<!>ABC<!>", "<!>", '\0', &end), QString("ABC"));
  QCOMPARE(end, 6);
  QCOMPARE(unprotectedString("A", "<!>", '\0'), QString(""));
  QCOMPARE(unprotectedString("A", "<!>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("AB", "<!>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("<>A<>", "<!>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("<>AB<>", "<!>", '\0', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("<>ABC<>", "<!>", '\0', &end), QString(""));
  QCOMPARE(end, -1);

   // Data with escape char
  QCOMPARE(unprotectedString("A", "'", '\\'), QString(""));
  QCOMPARE(unprotectedString("A", "'", '\\', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("\\", "'", '\\', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("\\'", "'", '\\'), QString("'"));
  QCOMPARE(unprotectedString("AB", "'", '\\', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("'A'", "'", '\\', &end), QString("A"));
  QCOMPARE(end, 2);
  QCOMPARE(unprotectedString("'A\\'B'", "'", '\\', &end), QString("A'B"));
  QCOMPARE(end, 5);
  QCOMPARE(unprotectedString("'A\\'B\\''", "'", '\\', &end), QString("A'B'"));
  QCOMPARE(end, 7);
  QCOMPARE(unprotectedString("'A\\'B\\'C'", "'", '\\', &end), QString("A'B'C"));
  QCOMPARE(end, 8);
  QCOMPARE(unprotectedString("A", "<>", '\\'), QString(""));
  QCOMPARE(unprotectedString("A", "<>", '\\', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("\\", "<>", '\\', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("\\<>", "<>", '\\'), QString("<>"));
  QCOMPARE(unprotectedString("AB", "<>", '\\', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("<>A<>", "<>", '\\', &end), QString("A"));
  QCOMPARE(end, 3);
  QCOMPARE(unprotectedString("<>A\\<>B<>", "<>", '\\', &end), QString("A<>B"));
  QCOMPARE(end, 7);
  QCOMPARE(unprotectedString("<>A\\<>B\\<><>", "<>", '\\', &end), QString("A<>B<>"));
  QCOMPARE(end, 10);
  QCOMPARE(unprotectedString("<>A\\<>B\\<>C<>", "<>", '\\', &end), QString("A<>B<>C"));
  QCOMPARE(end, 11);

  // Data and identical protection and escape char (like CSV)
  QCOMPARE(unprotectedString("A", "'", '\'', &end), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("'A'", "'", '\'', &end), QString("A"));
  QCOMPARE(end, 2);
  QCOMPARE(unprotectedString("'ABCD'", "'", '\'', &end), QString("ABCD"));
  QCOMPARE(end, 5);
  QCOMPARE(unprotectedString("'A''BC''D'", "'", '\'', &end), QString("A'BC'D"));
  QCOMPARE(end, 9);

  // Start a given position
  QCOMPARE(unprotectedString("", "'", '\\', &end, 0), QString(""));
  QCOMPARE(end, -1);
  QCOMPARE(unprotectedString("A'BC'", "'", '\\', &end, 1), QString("BC"));
  QCOMPARE(end, 4);
  QCOMPARE(unprotectedString("A'BC'D", "'", '\\', &end, 1), QString("BC"));
  QCOMPARE(end, 4);
  QCOMPARE(unprotectedString("A'BC'D'EF'G", "'", '\\', &end, 1), QString("BC"));
  QCOMPARE(end, 4);
  QCOMPARE(unprotectedString("A'BC\\'D\\'EF'G", "'", '\\', &end, 1), QString("BC'D'EF"));
  QCOMPARE(end, 11);
  QCOMPARE(unprotectedString("A<>BC<>D<>EF<>G", "<>", '\\', &end, 1), QString("BC"));
  QCOMPARE(end, 5);
  QCOMPARE(unprotectedString("A<>BC\\<>D\\<>EF<>G", "<>", '\\', &end, 1), QString("BC<>D<>EF"));
  QCOMPARE(end, 14);

  // Invalid data: must not crash ...
  unprotectedString("A'", "'", '\\');
  unprotectedString("A\\'", "'", '\\');
  unprotectedString("'A", "'", '\\', &end);
  unprotectedString("\\'A", "'", '\\');
  unprotectedString("\\", "'", '\\');
  unprotectedString("A\\", "'", '\\');
  unprotectedString("A\\'", "'", '\\');
}

void mdtAlgorithmsTest::splitStringTest()
{
  QStringList items;

  // Empty data test (mus not crash)
  items = splitString("", "", "");
  QVERIFY(items.isEmpty());
  items = splitString("", " ", "");
  QVERIFY(items.isEmpty());
  items = splitString("", ";", "");
  QVERIFY(items.isEmpty());
  items = splitString("", "", "\"");
  QVERIFY(items.isEmpty());
  items = splitString("", ";", "\"");
  QVERIFY(items.isEmpty());
  items = splitString("", "", "", '\\');
  QVERIFY(items.isEmpty());
  items = splitString("", " ", "", '\\');
  QVERIFY(items.isEmpty());
  items = splitString("", ";", "", '\\');
  QVERIFY(items.isEmpty());
  items = splitString("", "", "\"", '\\');
  QVERIFY(items.isEmpty());
  items = splitString("", ";", "\"", '\\');
  QVERIFY(items.isEmpty());

  // Simple data (1 item)
  items = splitString("A", "", "");
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A"));
  items = splitString("A", ";", "");
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A"));
  items = splitString("A", ";", "\"");
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A"));
  items = splitString("ABCD", ";", "\"");
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("ABCD"));
  items = splitString("A", "", "", '\\');
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A"));
  items = splitString("A", ";", "", '\\');
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A"));
  items = splitString("A", ";", "\"", '\\');
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A"));
  items = splitString("ABCD", ";", "\"", '\\');
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("ABCD"));

  // Many items, without protection
  items = splitString("A;B", ";", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("B"));
  items = splitString("AB;C", ";", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("AB"));
  QCOMPARE(items.at(1), QString("C"));
  items = splitString("A;BC", ";", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("BC"));
  items = splitString("AB;CD", ";", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("AB"));
  QCOMPARE(items.at(1), QString("CD"));
  items = splitString(";", ";", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString(""));
  QCOMPARE(items.at(1), QString(""));
  items = splitString(";;", ";", "");
  QCOMPARE(items.size(), 3);
  QCOMPARE(items.at(0), QString(""));
  QCOMPARE(items.at(1), QString(""));
  QCOMPARE(items.at(2), QString(""));
  items = splitString("A;", ";", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString(""));
  items = splitString(";A", ";", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString(""));
  QCOMPARE(items.at(1), QString("A"));
  items = splitString(";A;;B", ";", "");
  QCOMPARE(items.size(), 4);
  QCOMPARE(items.at(0), QString(""));
  QCOMPARE(items.at(1), QString("A"));
  QCOMPARE(items.at(2), QString(""));
  QCOMPARE(items.at(3), QString("B"));
  items = splitString(";A;;B", ";", "", '\\');
  QCOMPARE(items.size(), 4);
  QCOMPARE(items.at(0), QString(""));
  QCOMPARE(items.at(1), QString("A"));
  QCOMPARE(items.at(2), QString(""));
  QCOMPARE(items.at(3), QString("B"));

  items = splitString("B<sep>C", "<sep>", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("B"));
  QCOMPARE(items.at(1), QString("C"));
  items = splitString("AB<sep>CD", "<sep>", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("AB"));
  QCOMPARE(items.at(1), QString("CD"));
  items = splitString("<sep>CD", "<sep>", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString(""));
  QCOMPARE(items.at(1), QString("CD"));
  items = splitString("AB<sep>", "<sep>", "");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("AB"));
  QCOMPARE(items.at(1), QString(""));

  // Items with protection
  items = splitString("'A'", ";", "'");
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A"));
  items = splitString("'A;B'", ";", "'");
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A;B"));
  items = splitString("A;B", ";", "'");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("B"));
  items = splitString("A;'B;C'", ";", "'");
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("B;C"));
  items = splitString("A;'B;C';D", ";", "'");
  QCOMPARE(items.size(), 3);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("B;C"));
  QCOMPARE(items.at(2), QString("D"));
  items = splitString("A;<rem>B;C<rem>;D", ";", "<rem>");
  QCOMPARE(items.size(), 3);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("B;C"));
  QCOMPARE(items.at(2), QString("D"));
  items = splitString("A;'B;C';D", ";", "'", '\\');
  QCOMPARE(items.size(), 3);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("B;C"));
  QCOMPARE(items.at(2), QString("D"));
  items = splitString("A;<rem>B;C<rem>;D", ";", "<rem>", '\\');
  QCOMPARE(items.size(), 3);
  QCOMPARE(items.at(0), QString("A"));
  QCOMPARE(items.at(1), QString("B;C"));
  QCOMPARE(items.at(2), QString("D"));

  // 1 item with data protection + escaped strings
  items = splitString("'A\\'BC\\'D'", ";", "'", '\\');
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A'BC'D"));
  items = splitString("'A\\BCD'", ";", "'", '\\');
  QCOMPARE(items.size(), 1);
  QCOMPARE(items.at(0), QString("A\\BCD"));

  // CSV format
  items = splitString("abc,\"de,f\"", ",", "\"", '"');
  QCOMPARE(items.size(), 2);
  QCOMPARE(items.at(0), QString("abc"));
  QCOMPARE(items.at(1), QString("de,f"));
  items = splitString("abc,\"de,f\",\"123 \"\"test\",567", ",", "\"", '"');
  QCOMPARE(items.size(), 4);
  QCOMPARE(items.at(0), QString("abc"));
  QCOMPARE(items.at(1), QString("de,f"));
  QCOMPARE(items.at(2), QString("123 \"test"));
  QCOMPARE(items.at(3), QString("567"));
  items = splitString("abc,\"de,f\",\"123 \"\"test\"\"\",567", ",", "\"", '"');
  QCOMPARE(items.size(), 4);
  QCOMPARE(items.at(0), QString("abc"));
  QCOMPARE(items.at(1), QString("de,f"));
  QCOMPARE(items.at(2), QString("123 \"test\""));
  QCOMPARE(items.at(3), QString("567"));
  
  // Some wrong formated strings
  items = splitString("A;'B;C;D", ";", "'");
  ///qDebug() << items;
  ///QCOMPARE(items.size(), 2);
  ///QCOMPARE(items.at(0), QString("A"));
  ///QCOMPARE(items.at(1), QString("B;C;D"));

  items = splitString("''A''", ";", "'");
  ///qDebug() << items;
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
