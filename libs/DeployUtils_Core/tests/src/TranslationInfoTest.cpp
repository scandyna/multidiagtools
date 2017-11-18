/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "TranslationInfoTest.h"
#include "Mdt/DeployUtils/TranslationInfo.h"
#include "Mdt/DeployUtils/TranslationInfoList.h"
#include <QStringList>

using namespace Mdt::DeployUtils;

void TranslationInfoTest::initTestCase()
{
}

void TranslationInfoTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void TranslationInfoTest::infoFromQmFilePathTest()
{
  const auto ti = TranslationInfo::fromQmFilePath("/tmp/a_fr.qm");
  QCOMPARE(ti.absoluteFilePath(), QString("/tmp/a_fr.qm"));
  QCOMPARE(ti.fullFileName(), QString("a_fr.qm"));

  QFAIL("Not complete");
}

void TranslationInfoTest::compareInfoTest()
{
  QFETCH(QString, qmFilePathA);
  QFETCH(QString, qmFilePathB);
  QFETCH(bool, expectedEqual);

  const auto a = TranslationInfo::fromQmFilePath(qmFilePathA);
  const auto b = TranslationInfo::fromQmFilePath(qmFilePathB);
  if(expectedEqual){
    QVERIFY(a == b);
  }else{
    QVERIFY(!(a == b));
  }
}

void TranslationInfoTest::compareInfoTest_data()
{
  QTest::addColumn<QString>("qmFilePathA");
  QTest::addColumn<QString>("qmFilePathB");
  QTest::addColumn<bool>("expectedEqual");

  const bool equal = true;
  const bool notEqual = false;

  QTest::newRow("1") << "/tmp/a.qm" << "/tmp/a.qm" << equal;
  QTest::newRow("2") << "/tmp/a.qm" << "/opt/a.qm" << notEqual;
  QTest::newRow("3") << "/tmp/a.qm" << "/tmp/A.qm" << equal;
}

void TranslationInfoTest::addTranslationTest()
{
  TranslationInfoList til;
  QCOMPARE(til.count(), 0);
  QVERIFY(til.isEmpty());
  // Add a translation
  til.addTranslation( TranslationInfo::fromQmFilePath("/tmp/a_fr.qm") );
  QCOMPARE(til.count(), 1);
  QVERIFY(!til.isEmpty());
  QCOMPARE(til.at(0).absoluteFilePath(), QString("/tmp/a_fr.qm"));
  QCOMPARE(til.at(0).fullFileName(), QString("a_fr.qm"));
  // Add a other translation
  til.addTranslation( TranslationInfo::fromQmFilePath("/tmp/b_fr.qm") );
  QCOMPARE(til.count(), 2);
  QCOMPARE(til.at(0).absoluteFilePath(), QString("/tmp/a_fr.qm"));
  QCOMPARE(til.at(0).fullFileName(), QString("a_fr.qm"));
  QCOMPARE(til.at(1).absoluteFilePath(), QString("/tmp/b_fr.qm"));
  QCOMPARE(til.at(1).fullFileName(), QString("b_fr.qm"));
  // Try to add the first translation again
  til.addTranslation( TranslationInfo::fromQmFilePath("/tmp/a_fr.qm") );
  QCOMPARE(til.count(), 2);
  QCOMPARE(til.at(0).absoluteFilePath(), QString("/tmp/a_fr.qm"));
  QCOMPARE(til.at(0).fullFileName(), QString("a_fr.qm"));
  QCOMPARE(til.at(1).absoluteFilePath(), QString("/tmp/b_fr.qm"));
  QCOMPARE(til.at(1).fullFileName(), QString("b_fr.qm"));
}

void TranslationInfoTest::infoListFromQmFilePathListTest()
{
  const auto til = TranslationInfoList::fromQmFilePathList({"/tmp/a_fr.qm"});
  QCOMPARE(til.count(), 1);
  QVERIFY(!til.isEmpty());
  QCOMPARE(til.at(0).absoluteFilePath(), QString("/tmp/a_fr.qm"));
  QCOMPARE(til.at(0).fullFileName(), QString("a_fr.qm"));

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  TranslationInfoTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
