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
#include "QmFileNameBenchmark.h"
#include "Mdt/Translation/QmFileName.h"

using namespace Mdt::Translation;

void QmFileNameBenchmark::initTestCase()
{
}

void QmFileNameBenchmark::cleanupTestCase()
{
}

/*
 * Benchmarks
 */

void QmFileNameBenchmark::contructFromFullNameBenchmark()
{
  QFETCH(QString, inputName);
  QFETCH(QString, expectedSuffix);

  QString suffix;
  QBENCHMARK{
    QmFileName qfn(inputName);
    suffix = qfn.suffix();
  }
  QCOMPARE(suffix, expectedSuffix);
}

void QmFileNameBenchmark::contructFromFullNameBenchmark_data()
{
  QTest::addColumn<QString>("inputName");
  QTest::addColumn<QString>("expectedSuffix");

  QTest::newRow("") << "" << "";
  QTest::newRow("foo.qm") << "foo.qm" << "";
  QTest::newRow("Application_Core_fr_ca.qm") << "Application_Core_fr_ca.qm" << "fr_ca";
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QmFileNameBenchmark test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
