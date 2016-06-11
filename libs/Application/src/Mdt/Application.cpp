/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "Application.h"
#include "config.h"
#include "Mdt/Error.h"
#include "Mdt/ErrorLogger/Logger.h"
#include "Mdt/ErrorLogger/ConsoleBackend.h"
#include "Mdt/ErrorLogger/FileBackend.h"
#include <QStringList>
#include <QDebug>
#include <QDebugStateSaver>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QFont>
#include <QStyle>
#include <QtGlobal>
#include <QMetaType>
#include <memory>

namespace Mdt{

Application::Application(int & argc, char** argv)
 : QtSingleApplication(argc, argv, true)
{
  pvMultipleInstancePermission = SingleInstanceOnly;
}

Application::~Application()
{
  Mdt::ErrorLogger::Logger::cleanup();
}

bool Application::init(Application::MultipleInstancePermission multipleInstancePermission)
{
  pvMultipleInstancePermission = multipleInstancePermission;

  // Check if another instance of program is running
  if(multipleInstancePermission == SingleInstanceOnly){
    if(sendMessage("")){
      // We not use qFatal() because this could pop-up debuggers, or crash dialogs, witch is not desirable here
      qWarning() << "Mdt::Application::init(): a other instance of program is allready running";
      return false;
    }
  }
  // Search system data directory
  if(!findSystemDataDirectory()){
    qFatal("Mdt::Application::init(): system data directory was not found");
    return false;
  }
  // Init log directory
  if(!initLogDirectory()){
    qWarning() << "Mdt::Application: log directory could not be created. No log will be available";
  }
  // Init error system
  if(!initErrorSystem()){
    qFatal("Mdt::Application::init(): could not init error system");
    return false;
  }
  // Init cache directory
  if(!initCacheDirectory()){
    qFatal("Mdt::Application::init(): could not init cache directory");
    return false;
  }

  return true;
}

QString Application::libraryVersion()
{
  return QStringLiteral(MDT_VERSION_STR);
}

void Application::debugEnvironnement()
{
  Q_ASSERT(MdtApp != nullptr);
  Q_ASSERT(style() != nullptr);

  QDebug dbg(QtDebugMsg);
  QDebugStateSaver stateSaver(dbg);

  dbg.noquote();
  dbg.nospace();

  dbg << "********* MDT Application environment *********\n";
  dbg << "Qt library version: " << QT_VERSION_STR << "\n";
  dbg << "MDT library version: " << libraryVersion() << "\n";
  dbg << "Application PID: " << applicationPid() << "\n";
  dbg << "Application name: " << applicationName() << "\n";
  dbg << "Organization name: " << organizationName() << "\n";
  dbg << "Application directory: " << applicationDirPath() << "\n";
  dbg << "System data directory: " << systemDataDirectoryPath() << "\n";
  dbg << "Log directory: " << logDirectoryPath() << "\n";
  dbg << "Log file: " << logFilePath() << "\n";
  dbg << "Cache directory: " << cacheDirectoryPath() << "\n";
  dbg << "Multiple instances allowed: " << (multipleInstancePermission() == MultipleInstanceAllowed) << "\n";
  dbg << "GUI platform: " << platformName() << "\n";
  dbg << "Style: " << style()->objectName() << "\n";
  dbg << "Font: " << font().toString() << "\n";
  dbg << "Is desktop settings aware: " << desktopSettingsAware() << "\n";
  dbg << "***********************************************";
}

bool Application::findSystemDataDirectory()
{
  QStringList pathList;

  // Fisrt, look in current directory
  pathList << QDir::currentPath() + QStringLiteral("/data");
  // Check in application directory
  pathList << applicationDirPath() + QStringLiteral("/data");
  // On Linux/Unix, data is probably installed in /usr/share/mdt or /usr/local/share
#ifdef Q_OS_UNIX
  pathList << "/usr/share/mdt/data" << "/usr/local/share/mdt/data";
#endif  // #ifdef Q_OS_UNIX
  // Check also in source tree
  pathList << QStringLiteral(MDT_SOURCE_DIRECTORY_PATH) + QStringLiteral("/data");
  // Search..
  for(const auto & path : pathList){
    if(isSystemDataDirectory(path)){
      pvSystemDataDirectory = path;
      return true;
    }
  }
  // Data directory not found :-(

  return false;
}

bool Application::isSystemDataDirectory(const QString & path)
{
  QDir dir(path);

  // Check if needed directories exists
  if(!dir.cd("i18n")){
    return false;
  }
  dir.cdUp();
  if(!dir.cd("uic")){
    return false;
  }
  dir.cdUp();

  return true;
}

bool Application::initLogDirectory()
{
  QDir dir = QDir::home();

  // Look about the .mdt directory
  if(!dir.cd(".mdt")){
    // Not found, try to create it
    if(!dir.mkdir(".mdt")){
      qWarning() << "Mdt::Application: cannot create " << dir.absolutePath() + "/.mdt";
      return false;
    }
    if(!dir.cd(".mdt")){
      qWarning() << "Mdt::Application: cannot enter " << dir.absolutePath() + "/.mdt";
      return false;
    }
  }
  // Log directory
  if(!dir.cd("log")){
    // Not found, try to create it
    if(!dir.mkdir("log")){
      qWarning() << "Mdt::Application: cannot create " << dir.absolutePath() + "/log";
      return false;
    }
    if(!dir.cd("log")){
      qWarning() << "Mdt::Application: cannot enter " << dir.absolutePath() + "log";
      return false;
    }
  }
  // Store log directory path
  pvLogDirectory = dir.absolutePath();

  return true;
}

bool Application::initErrorSystem()
{
  QString logFileName;

  qRegisterMetaType<Mdt::Error>();
  auto consoleOut = std::make_shared<Mdt::ErrorLogger::ConsoleBackend>();
  Mdt::ErrorLogger::Logger::addBackend(consoleOut);
  // Add a log file backend if possible
  if(!pvLogDirectory.isEmpty()){
    logFileName = applicationName();
    /*
     * If multiple instances of application is allowed, 
     * the simple way to avoid conflict for log file name is
     * to suffix process id to the logfilename
     */
    if(pvMultipleInstancePermission == MultipleInstanceAllowed){
      logFileName += QStringLiteral("-") + QString::number(applicationPid());
    }
    logFileName += QStringLiteral(".log");
    // Define path
    pvLogFile = QFileInfo(pvLogDirectory, logFileName).absoluteFilePath();
    // Add backend
    auto fileOut = std::make_shared<Mdt::ErrorLogger::FileBackend>();
    if(fileOut->setLogFilePath(pvLogFile)){
      Mdt::ErrorLogger::Logger::addBackend(fileOut);
    }else{
      qWarning() << "Mdt::Application: log file '" << pvLogFile << "' could not be open, no log will come to it";
    }
  }

  return true;
}

bool Application::initCacheDirectory()
{
  QString path;
  const auto pathList = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);

  // Define cache path: if a standard path exists, use it
  if(pathList.isEmpty()){
    path = QDir::homePath() + "/.mdt/cache";
  }else{
    path = pathList.at(0);
  }
  // Create cahce directory if required
  QDir dir(path);
  if(!dir.exists()){
    qDebug() << "Creating " << path << " ...";
    if(!dir.mkpath(path)){
      qWarning() << "Mdt::Application: cannot create " << path;
      return false;
    }
  }
  if(!dir.cd(path)){
    qWarning() << "Mdt::Application: cannot enter " << path;
    return false;
  }
  // Store pvCacheDirectory
  pvCacheDirectory = path;

  return true;
}

} // namespace Mdt{
