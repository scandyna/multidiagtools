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
#include "RPathTestLinux.h"
#include "Mdt/DeployUtils/PatchelfWrapper.h"
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/RPathInfo.h"
#include "Mdt/DeployUtils/RPathInfoList.h"
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

using namespace Mdt::DeployUtils;

void RPathTestLinux::initTestCase()
{
}

void RPathTestLinux::cleanupTestCase()
{
}

/*
 * Tests
 */

void RPathTestLinux::patchelfWrapperTest()
{
  PatchelfWrapper patchelf;
  const auto binaryFilePath = copyCurrentExecutable();
  QVERIFY(!binaryFilePath.isEmpty());

  QVERIFY(patchelf.execWriteRPath("/usr/lib", binaryFilePath));
  QVERIFY(patchelf.execReadRPath(binaryFilePath));
  QCOMPARE(patchelf.readAllStandardOutputString().trimmed(), QString("/usr/lib"));

  QVERIFY(patchelf.execWriteRPath("$ORIGIN", binaryFilePath));
  QVERIFY(patchelf.execReadRPath(binaryFilePath));
  QCOMPARE(patchelf.readAllStandardOutputString().trimmed(), QString("$ORIGIN"));
}

void RPathTestLinux::rpathTest()
{
  RPath rpath;
  RPathInfoList rpathInfo;
  const auto binaryFilePath = copyCurrentExecutable();
  QVERIFY(!binaryFilePath.isEmpty());

  rpathInfo.setRelativePath("lib");
  QVERIFY(rpath.setRPath(rpathInfo, binaryFilePath));
  QVERIFY(rpath.readRPath(binaryFilePath));
  rpathInfo = rpath.rpath();
  QCOMPARE(rpathInfo.count(), 1);
  QVERIFY(rpathInfo.at(0).isRelative());
  QCOMPARE(rpathInfo.at(0).path(), QString("lib"));

  QVERIFY(rpath.setRPathToOrigin(binaryFilePath));
  QVERIFY(rpath.readRPath(binaryFilePath));
  rpathInfo = rpath.rpath();
  QCOMPARE(rpathInfo.count(), 1);
  QVERIFY(rpathInfo.at(0).isRelative());
  QVERIFY(rpathInfo.at(0).isOrigin());
}

void RPathTestLinux::prependPathForBinInDirTest()
{
  RPath rpath;
  RPathInfoList rpathInfo;
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const auto binaryFilePath = copyCurrentExecutableToDirectory(dir);
  QVERIFY(!binaryFilePath.isEmpty());

  QVERIFY(rpath.prependPathForBinaries("lib", dir.path()));
  QVERIFY(rpath.readRPath(binaryFilePath));
  rpathInfo = rpath.rpath();
  QVERIFY(rpathInfo.count() >= 1);
  QVERIFY(rpathInfo.at(0).isRelative());
  QCOMPARE(rpathInfo.at(0).path(), QString("lib"));
}

/*
 * Helpers
 */

QString RPathTestLinux::copyCurrentExecutable()
{
  const auto sourceExePath = QCoreApplication::applicationFilePath();
  const auto destExePath = sourceExePath + "_cpy";

  if(QFile::exists(destExePath)){
    if(!QFile::remove(destExePath)){
      qWarning() << "Removeing previously created file " << destExePath << " failed";
    }
  }
  if(!QFile::copy(sourceExePath, destExePath)){
    qWarning() << "Copy of " << sourceExePath << " -> " << destExePath << " failed";
    return QString();
  }

  return destExePath;
}

QString RPathTestLinux::copyCurrentExecutableToDirectory(const QTemporaryDir & dir)
{
  Q_ASSERT(dir.isValid());

  const auto sourceExePath = QCoreApplication::applicationFilePath();
  const auto destExePath =  QDir::cleanPath(dir.path() + "/" + QFileInfo(sourceExePath).fileName());

  if(!QFile::copy(sourceExePath, destExePath)){
    qWarning() << "Copy of " << sourceExePath << " -> " << destExePath << " failed";
    return QString();
  }

  return destExePath;
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RPathTestLinux test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
