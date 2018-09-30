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
#include "CoreApplicationImpl.h"
#include "Version.h"
#include "Mdt/Error.h"
#include "Mdt/ErrorLogger/Logger.h"
#include "Mdt/ErrorLogger/ConsoleBackend.h"
#include "Mdt/ErrorLogger/FileBackend.h"
#include "Mdt/LibraryInfo.h"
#include <QDir>
#include <QStringBuilder>
#include <QLatin1String>
#include <QLatin1Char>
#include <QDebug>
#include <QMetaType>
#include <QtGlobal>
#include <QCoreApplication>
#include <memory>

namespace Mdt{

CoreApplicationImpl::CoreApplicationImpl()
{
  initErrorSystem();
  initCacheDirectory();
  addPluginPathToLibrariesPath();
}

CoreApplicationImpl::~CoreApplicationImpl()
{
  Mdt::ErrorLogger::Logger::cleanup();
}

void CoreApplicationImpl::enableFileLogging(CoreApplicationImpl::LogFileNameFormat format)
{
  using Mdt::ErrorLogger::Logger;
  using Mdt::ErrorLogger::FileBackend;

  if(isFileLoggingEnabled()){
    return;
  }
  // Create log directory
  QDir logDirectory( logDirectoryPath() );
  if(!logDirectory.exists()){
    if(!logDirectory.mkpath( logDirectory.absolutePath() )){
      qWarning() << tr("Mdt::CoreApplication: could not create log directory '%1'. File logging will be disabled.").arg(logDirectory.absolutePath());
      return;
    }
  }
  Q_ASSERT(logDirectory.exists());
  // Build log file path
  QString logFileName;
  switch(format){
    case ApplicationName:
      logFileName = QCoreApplication::applicationName();
      break;
    case ApplicationNameAndPid:
      logFileName = QCoreApplication::applicationName() % QLatin1Char('_') % QString::number( QCoreApplication::applicationPid() );
      break;
  }
  mLogFilePath = QDir::cleanPath( logDirectory.absolutePath() % QLatin1Char('/') % logFileName % QLatin1String(".log") );
  // Add file backend to the error logger
  auto *logFileBackend = Logger::addBackend<FileBackend>(Logger::ExecuteInSeparateThread);
  Q_ASSERT(logFileBackend != nullptr);
  if(!logFileBackend->setLogFilePath(mLogFilePath)){
    qWarning() << tr("Mdt::CoreApplication: log file '%1' could not be open, no log will come to it.").arg(mLogFilePath);
    return;
  }
}

QString CoreApplicationImpl::qtVersion()
{
  return QLatin1String(QT_VERSION_STR);
}

QString CoreApplicationImpl::mdtVersion()
{
  return QLatin1String(MDT_VERSION_STR);
}

void CoreApplicationImpl::debugEnvironment(const QStringList & entries)
{
  QDebug dbg(QtDebugMsg);
  dbg.noquote();
  dbg.nospace();

  dbg << "********* Mdt Application environment *********\n";
  dbg << entries.join(QLatin1Char('\n')) << '\n';
  dbg << "***********************************************";
}

QStringList CoreApplicationImpl::commonEnvironmentEntries()
{
  return QStringList{
    tr("Organization name: %1").arg(QCoreApplication::organizationName()),
    tr("Application name: %1").arg(QCoreApplication::applicationName()),
    tr("Application PID: %1").arg(QCoreApplication::applicationPid()),
    tr("Application directory: %1").arg(QCoreApplication::applicationDirPath()),
    tr("Log directory: %1").arg(logDirectoryPath()),
    tr("Log file: %1").arg(logFilePath()),
    tr("Cache directory: %1").arg(cacheDirectoryPath()),
    tr("Qt  library version: %1").arg(qtVersion()),
    tr("Mdt library version: %1").arg(mdtVersion())
  };
}

void CoreApplicationImpl::initErrorSystem()
{
  using Mdt::ErrorLogger::Logger;
  using Mdt::ErrorLogger::ConsoleBackend;

  qRegisterMetaType<Mdt::Error>();
  Logger::addBackend<ConsoleBackend>(Logger::ExecuteInMainThread);
}

void CoreApplicationImpl::initCacheDirectory()
{
  QDir cacheDirectory( cacheDirectoryPath() );
  if(!cacheDirectory.exists()){
    if(!cacheDirectory.mkpath( cacheDirectory.absolutePath() )){
      qWarning() << tr("Mdt::CoreApplication: could not create cache directory '%1'.").arg(cacheDirectory.absolutePath());
      return;
    }
  }
}

void CoreApplicationImpl::addPluginPathToLibrariesPath()
{
  const auto path = Mdt::LibraryInfo::getPluginsPath();
  if(!path){
    qWarning() << tr("Mdt::CoreApplication: no plugin path found, maybe some functionnalities will missing.\n Error: %1").arg(path.error().text());
    return;
  }
  QCoreApplication::addLibraryPath(*path);
}

} // namespace Mdt{
