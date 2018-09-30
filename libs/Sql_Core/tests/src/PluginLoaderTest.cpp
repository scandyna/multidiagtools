/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "PluginLoaderTest.h"
#include "Mdt/Sql/PluginInfo.h"
#include "Mdt/Sql/PluginLoader.h"
#include <QJsonDocument>
#include <QByteArray>
#include <QSqlError>

using Mdt::Sql::PluginInfo;
using Mdt::Sql::PluginLoader;
using Mdt::Sql::AbstractErrorDriver;

/*
 * Tests
 */

void PluginLoaderTest::pluginInfoTest()
{
  PluginInfo pi;
  QVERIFY(pi.isNull());

  QString path = "/path/to/plugin.so";
  pi.setFilePath(path);
  QCOMPARE(pi.filePath(), path);
  QVERIFY(pi.isNull());

  QString interface = "Mdt.Sql.AbstractErrorDriver";
  pi.setInterface(interface);
  QCOMPARE(pi.interface(), interface);
  QVERIFY(pi.isNull());

  QStringList drivers = {"QSQLITE","MDTQSQLITE"};
  pi.setCompatibleDrivers(drivers);
  QCOMPARE(pi.compatibleDrivers(), drivers);
  QVERIFY(!pi.isNull());
  QVERIFY(pi.supportsDriver("QSQLITE"));
  QVERIFY(pi.supportsDriver("MDTQSQLITE"));
  QVERIFY(!pi.supportsDriver("QMYSQL"));

  path = "/path/to/plugin.so";
  QByteArray metaData = "{"\
    "\"IID\":\"Mdt.Sql.AbstractErrorDriver\","\
    "\"MetaData\":{\"Keys\":[\"TEST\"]},"\
    "\"className\":\"ErrorDriverTestPlugin\","\
    "\"debug\":true,"\
    "\"version\":329985"\
  "}";
  QJsonObject jsonMetaData = QJsonDocument::fromJson(metaData).object();
  pi = PluginInfo::fromFileInfoAndMetaData(path, jsonMetaData);
  QCOMPARE(pi.filePath(), path);
  QCOMPARE(pi.interface(), QString("Mdt.Sql.AbstractErrorDriver"));
  QCOMPARE(pi.compatibleDrivers(), QStringList({"TEST"}));
  QVERIFY(!pi.isNull());

  path = "/path/to/plugin.so";
  metaData = "{"\
    "\"IID\":\"Mdt.Sql.AbstractErrorDriver\","\
    "\"MetaData\":{},"\
    "\"className\":\"ErrorDriverTestPlugin\","\
    "\"debug\":true,"\
    "\"version\":329985"\
  "}";
  jsonMetaData = QJsonDocument::fromJson(metaData).object();
  pi = PluginInfo::fromFileInfoAndMetaData(path, jsonMetaData);
  QVERIFY(pi.isNull());
}

void PluginLoaderTest::loadErrorDriverTestPluginTest()
{
  PluginLoader loader;

  AbstractErrorDriver *errorDriver = loader.loadErrorDriver("TEST");
  QVERIFY(errorDriver != nullptr);
  QCOMPARE(errorDriver->errorCode(QSqlError()), Mdt::ErrorCode::UnknownError);
  QCOMPARE(errorDriver->errorLevel(QSqlError()), Mdt::Error::Warning);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
//   Mdt::CoreApplication app(argc, argv);
  QCoreApplication app(argc, argv);
  PluginLoaderTest test;

  return QTest::qExec(&test, argc, argv);
}
