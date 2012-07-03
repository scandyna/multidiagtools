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
#include "mdtApplication.h"
#include "mdtErrorOut.h"
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <iostream>

#include <QDebug>

mdtApplication *mdtApplication::pvInstance = 0;

mdtApplication::mdtApplication(int &argc, char **argv, bool GUIenabled)
 : QtSingleApplication(argc, argv, GUIenabled)
{
  pvInstance = this;
}

mdtApplication::~mdtApplication()
{
  // Free the error system
  mdtErrorOut::destroy();
  qDeleteAll(pvTranslators);
}

bool mdtApplication::init(bool allowMultipleInstances, int dialogErrorLevelsMask)
{
  QFileInfo fi;
  QString logFileName;

  // Check if another instance of program is running
  if(!allowMultipleInstances){
    if(sendMessage("")){
      std::cerr << "mdtApplication::init(): another instance is running , abort" << std::endl;
      return false;
    }
  }

#ifdef Q_OS_UNIX
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

  // System data directory path
  if(!searchSystemDataDir()){
    return false;
  }
  // Init home directory structure
  if(!initHomeDir()){
    return false;
  }
  qDebug() << "mdtApplication , data dir: " << pvSystemDataDirPath;
  qDebug() << "mdtApplication , log dir: " << pvLogDirPath;
  qDebug() << "mdtApplication , lib version: " << mdtLibVersion();

  // Build log file name
  fi.setFile(arguments().at(0));
  logFileName = fi.baseName();
  if(logFileName.isEmpty()){
    logFileName = "mdtapplication";
  }
  logFileName += "-";
  logFileName += QString::number(applicationPid());
  logFileName += ".log";
  qDebug() << "mdtApplication::init(): log file: " << logFileName;
  // Init error system
  if(!mdtErrorOut::init(pvLogDirPath + "/" + logFileName)){
    std::cerr << "mdtApplication::init(): unable to init the error system" << std::endl;
    return false;
  }
  mdtErrorOut::setDialogLevelsMask(dialogErrorLevelsMask);
  // Set the default language
  if(!setLanguage()){
    return false;
  }

  return true;
}

void mdtApplication::setApplicationVersion(int versionMajor, int versionMinor, int versionMicro)
{
  QString num, version;
  
  num.setNum(versionMajor);
  version = num + ".";
  num.setNum(versionMinor);
  version += num + ".";
  num.setNum(versionMicro);
  version += num;
  
  QApplication::setApplicationVersion(version);
}

mdtApplication *mdtApplication::instance()
{
  Q_ASSERT(pvInstance != 0);

  return pvInstance;
}

QString mdtApplication::systemDataDirPath()
{
  return instance()->pvSystemDataDirPath;
}

QString mdtApplication::mdtLibVersion()
{
  return QString(QString::number(MDTLIB_VERSION_MAJOR) + "." + QString::number(MDTLIB_VERSION_MINOR) + "." + QString::number(MDTLIB_VERSION_MICRO));
}

bool mdtApplication::setLanguage(const QLocale &locale, const QStringList &otherQmDirectories)
{
  QString languageSuffix;
  int i;

  languageSuffix = locale.name().left(2);
  // Search in given other qm directories
  for(i=0; i<otherQmDirectories.size(); i++){
    if(!loadTranslationFiles(languageSuffix, otherQmDirectories.at(i))){
      return false;
    }
  }
  // Load that is avaliable in system's data directory
  return loadTranslationFiles(languageSuffix);
}

void mdtApplication::installTranslator(QTranslator *translator)
{
  Q_ASSERT(translator != 0);

  pvTranslators << translator;
  QApplication::installTranslator(translator);
}

void mdtApplication::removeCurrentTranslators()
{
  int i;

  for(i=0; i<pvTranslators.size(); i++){
    removeTranslator(pvTranslators.at(i));
  }
  qDeleteAll(pvTranslators);
  pvTranslators.clear();
}

bool mdtApplication::searchSystemDataDir()
{
  QDir dir;

  // At first, we look in current directory
  if(dir.cd("data")){
    // Ok, found.
    pvSystemDataDirPath = dir.absolutePath();
    return true;
  }
  // Check in application directory
  dir.setPath(applicationDirPath());
  if(!dir.exists()){
    std::cerr << "mdtApplication::searchSystemDataDir(): cannot find application directory" << std::endl;
    return false;
  }
  if(dir.cd("data")){
    // Ok, found.
    pvSystemDataDirPath = dir.absolutePath();
    return true;
  }
  // Check in application's parent directory
  if(dir.cd("../data")){
    // Ok, found.
    pvSystemDataDirPath = dir.absolutePath();
    return true;
  }
  // On Linux/Unix, data is probably installed in /usr/share/mdt or /usr/local/share
#ifdef Q_OS_UNIX
  if(dir.cd("/usr/share/mdt/data")){
    // Ok, found.
    pvSystemDataDirPath = dir.absolutePath();
    return true;
  }
  if(dir.cd("/usr/local/share/mdt/data")){
    // Ok, found.
    pvSystemDataDirPath = dir.absolutePath();
    return true;
  }
#endif  // #ifdef Q_OS_UNIX

  // Data directory not found :-(
  std::cerr << "mdtApplication::searchSystemDataDir(): cannot find system data directory" << std::endl;

  return false;
}

bool mdtApplication::initHomeDir()
{
  QDir dir(QDir::home());

  // Home directory must exists first..
  if(!dir.exists()){
    std::cerr << "mdtApplication::initHomeDir(): cannot find home directory" << std::endl;
    return false;
  }
  // Look about the .mdt directory
  if(!dir.cd(".mdt")){
    // Not found, try to create it
    if(!dir.mkdir(".mdt")){
      std::cerr << "mdtApplication::initHomeDir(): cannot create .mdt in home directory" << std::endl;
      return false;
    }
    if(!dir.cd(".mdt")){
      std::cerr << "mdtApplication::initHomeDir(): cannot enter $home/.mdt directory" << std::endl;
      return false;
    }
  }
  // Log directory
  if(!dir.cd("log")){
    // Not found, try to create it
    if(!dir.mkdir("log")){
      std::cerr << "mdtApplication::initHomeDir(): cannot create log in home directory" << std::endl;
      return false;
    }
    if(!dir.cd("log")){
      std::cerr << "mdtApplication::initHomeDir(): cannot enter $home/.mdt/log directory" << std::endl;
      return false;
    }
  }
  pvLogDirPath = dir.absolutePath();

  return true;
}

bool mdtApplication::loadTranslationFiles(const QString &languageSuffix, const QString &otherQmDirectory)
{
  QDir dir;
  QFileInfoList filesInfoList;
  QTranslator *translator;
  int i;

  // If no language suffix is geven, just return
  if(languageSuffix.isEmpty()){
    return true;
  }
  // Case of non supported suffix size, error
  if(languageSuffix.size() != 2){
    std::cerr << "mdtApplication::loadTranslationFiles(): unknow language suffix: " << languageSuffix.toStdString() << std::endl;
    return false;
  }
  // Try to go to translations directory
  if(!otherQmDirectory.isEmpty()){
    if(!dir.cd(otherQmDirectory)){
      std::cerr << "mdtApplication::loadTranslationFiles(): cannot find directory" << otherQmDirectory.toStdString() << std::endl;
      return false;
    }
  }else{
    if(!dir.cd(pvSystemDataDirPath)){
      std::cerr << "mdtApplication::loadTranslationFiles(): cannot find data directory" << std::endl;
      return false;
    }
    // We are in data directory, try to go to i18n
    if(!dir.cd("i18n")){
      std::cerr << "mdtApplication::loadTranslationFiles(): cannot find i18n directory\n -> Searched in " << pvSystemDataDirPath.toStdString() << std::endl;
      return false;
    }
  }
  // Get avaliable files for given language
  filesInfoList = dir.entryInfoList(QStringList("*_" + languageSuffix + ".qm"));
  if(filesInfoList.size() < 1){
    // Not translation file found is not fatal, just put a message
    std::cerr << "mdtApplication::loadTranslationFiles(): no translation file was found in " << dir.path().toStdString() << std::endl;
  }
  for(i=0; i<filesInfoList.size(); i++){
    translator = new QTranslator;
    if(!translator->load(filesInfoList.at(i).absoluteFilePath())){
      std::cerr << "mdtApplication::loadTranslationFiles(): cannot load translation file " << filesInfoList.at(i).fileName().toStdString() << std::endl;
      delete translator;
      return false;
    }
    installTranslator(translator);
  }

  return true;
}
