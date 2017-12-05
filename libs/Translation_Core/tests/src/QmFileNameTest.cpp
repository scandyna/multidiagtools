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
#include "QmFileNameTest.h"
#include "Mdt/Translation/QmFileName.h"

using namespace Mdt::Translation;

void QmFileNameTest::initTestCase()
{
}

void QmFileNameTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void QmFileNameTest::constructNullFileNameTest()
{
  QmFileName nullQfn;
  QVERIFY(nullQfn.fullName().isEmpty());
  QVERIFY(nullQfn.baseName().isEmpty());
  QVERIFY(nullQfn.suffix().isEmpty());
  QVERIFY(nullQfn.languageSuffix().isEmpty());
  QVERIFY(nullQfn.countrySuffix().isEmpty());
  QVERIFY(nullQfn.isNull());
}

void QmFileNameTest::contructFromFullNameTest()
{
  QFETCH(QString, inputName);
  QFETCH(QString, expectedBaseName);
  QFETCH(QString, expectedSuffix);
  QFETCH(QString, expectedLanguageSuffix);
  QFETCH(QString, expectedCountrySuffix);
  QFETCH(bool, expectedIsNull);

  QmFileName qfn(inputName);
  QCOMPARE(qfn.fullName(), inputName);
  QCOMPARE(qfn.baseName(), expectedBaseName);
  QCOMPARE(qfn.suffix(), expectedSuffix);
  QCOMPARE(qfn.languageSuffix(), expectedLanguageSuffix);
  QCOMPARE(qfn.countrySuffix(), expectedCountrySuffix);
  QCOMPARE(qfn.isNull(), expectedIsNull);
}

void QmFileNameTest::contructFromFullNameTest_data()
{
  QTest::addColumn<QString>("inputName");
  QTest::addColumn<QString>("expectedBaseName");
  QTest::addColumn<QString>("expectedSuffix");
  QTest::addColumn<QString>("expectedLanguageSuffix");
  QTest::addColumn<QString>("expectedCountrySuffix");
  QTest::addColumn<bool>("expectedIsNull");

  const bool Null = true;
  const bool NotNull = false;

  QTest::newRow("") << "" << "" << "" << "" << "" << Null;
  QTest::newRow("foo.qm") << "foo.qm" << "foo" << "" << "" << "" << NotNull;
  QTest::newRow("foo_fr.qm") << "foo_fr.qm" << "foo" << "fr" << "fr" << "" << NotNull;
  QTest::newRow("foo_fr_ca.qm") << "foo_fr_ca.qm" << "foo" << "fr_ca" << "fr" << "ca" << NotNull;
  QTest::newRow("foo_fr_CA.qm") << "foo_fr_CA.qm" << "foo" << "fr_CA" << "fr" << "CA" << NotNull;
  QTest::newRow("qt_fr.qm") << "qt_fr.qm" << "qt" << "fr" << "fr" << "" << NotNull;
  QTest::newRow("qt_fr_ca.qm") << "qt_fr_ca.qm" << "qt" << "fr_ca" << "fr" << "ca" << NotNull;
  QTest::newRow("Application_Core.qm") << "Application_Core.qm" << "Application_Core" << "" << "" << "" << NotNull;
  QTest::newRow("Application_Core_fr.qm") << "Application_Core_fr.qm" << "Application_Core" << "fr" << "fr" << "" << NotNull;
  QTest::newRow("Application_Core_fr_ca.qm") << "Application_Core_fr_ca.qm" << "Application_Core" << "fr_ca" << "fr" << "ca" << NotNull;
  QTest::newRow("Application_Core_b2_fr_ca.qm") << "Application_Core_b2_fr_ca.qm" << "Application_Core_b2" << "fr_ca" << "fr" << "ca" << NotNull;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QmFileNameTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
