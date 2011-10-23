#include "mdtAlgorithmsTest.h"
#include "mdtAlgorithms.h"

#include <QTest>
#include <QString>
#include <QStringList>

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
  QTest::newRow("Numbers") << "1" << "2" << true;
  QTest::newRow("Numbers") << "1" << "1" << false;
  QTest::newRow("Numbers") << "2" << "1" << false;
  // Char are not supported, result must be false
  QTest::newRow("Numbers") << "a" << "1" << false;
  QTest::newRow("Numbers") << "1" << "a" << false;
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
}
