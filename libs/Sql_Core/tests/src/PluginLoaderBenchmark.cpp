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
#include "PluginLoaderBenchmark.h"
#include "Mdt/Sql/PluginLoader.h"
#include "Mdt/Sql/PluginInfo.h"
#include "Mdt/Sql/PluginInstance.h"
#include "Mdt/Sql/AbstractErrorDriver.h"
#include "Mdt/Sql/AbstractErrorDriverPlugin.h"
#include <memory>

using Mdt::Sql::PluginLoader;
using Mdt::Sql::PluginInfo;
using Mdt::Sql::PluginInstance;
using Mdt::Sql::AbstractErrorDriver;
using Mdt::Sql::AbstractErrorDriverPlugin;

/*
 * Benchmarks
 */

void PluginLoaderBenchmark::findPluginBenchmark()
{
  PluginInfo pluginInfo;
  PluginLoader loader;

  QBENCHMARK{
    pluginInfo = loader.findPlugin("Mdt.Sql.AbstractErrorDriver", "TEST");
    QVERIFY(!pluginInfo.isNull());
  }
  QObject *plugin = loader.loadPlugin(pluginInfo);
  QVERIFY(plugin != nullptr);
  QVERIFY( qobject_cast<Mdt::Sql::AbstractErrorDriverPlugin*>(plugin) != nullptr );
}

void PluginLoaderBenchmark::loadPluginBenchmark()
{
  PluginInfo pluginInfo;
  PluginLoader loader;
  QObject *plugin = nullptr;

  pluginInfo = loader.findPlugin("Mdt.Sql.AbstractErrorDriver", "TEST");
  QVERIFY(!pluginInfo.isNull());
  QBENCHMARK{
    plugin = loader.loadPlugin(pluginInfo);
    QVERIFY(plugin != nullptr);
  }
  QVERIFY( qobject_cast<Mdt::Sql::AbstractErrorDriverPlugin*>(plugin) != nullptr );
}

void PluginLoaderBenchmark::pluginInstanceCreateBenchmark()
{
  PluginInstance plugin;
  QObject *driver;
  std::unique_ptr<AbstractErrorDriver> errorDriver;

  QBENCHMARK{
    driver = plugin.create<AbstractErrorDriverPlugin>("Mdt.Sql.AbstractErrorDriver", "TEST");
    QVERIFY(driver != nullptr);
    errorDriver.reset( qobject_cast<AbstractErrorDriver*>(driver) );
    QVERIFY(errorDriver != nullptr);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
//   Mdt::CoreApplication app(argc, argv);
  QCoreApplication app(argc, argv);
  PluginLoaderBenchmark test;

  return QTest::qExec(&test, argc, argv);
}
