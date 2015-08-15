/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
//   QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

  // System data directory path
  if(!searchSystemDataDir()){
    return false;
  }
  // Init home directory structure
  if(!initHomeDir()){
    return false;
  }
  ///qDebug() << "mdtApplication , data dir: " << pvSystemDataDirPath;
  qDebug() << "mdtApplication , data dir: " << pvSystemDataDir.absolutePath();
  ///qDebug() << "mdtApplication , log dir: " << pvLogDirPath;
  qDebug() << "mdtApplication , log dir: " << pvLogDir.absolutePath();
  qDebug() << "mdtApplication , cache dir: " << pvCacheDir.absolutePath();
  qDebug() << "mdtApplication , lib version: " << mdtLibVersion();

  // Build log file name
  fi.setFile(arguments().at(0));
  logFileName = fi.baseName();
  if(logFileName.isEmpty()){
    logFileName = "mdtapplication";
  }
  /*
   * If multiple instances of application is allowed, 
   * the simple way to avoid conflict during logfile write is
   * to suffix process id to the logfilename
   */
  if(allowMultipleInstances){
    logFileName += "-";
    logFileName += QString::number(applicationPid());
  }
  logFileName += ".log";
  qDebug() << "mdtApplication::init(): log file: " << logFileName;
  // Init error system
  ///if(!mdtErrorOut::init(pvLogDirPath + "/" + logFileName)){
  if(!mdtErrorOut::init(pvLogDir.absolutePath() + "/" + logFileName)){
    std::cerr << "mdtApplication::init(): unable to init the error system" << std::endl;
    return false;
  }
  mdtErrorOut::setDialogLevelsMask(dialogErrorLevelsMask);
  // Load translations and set the default language
  ///addTranslationsDirectory(pvSystemDataDirPath + "/i18n");
  addTranslationsDirectory(pvSystemDataDir.absolutePath() + "/i18n");
#ifdef Q_OS_UNIX
  addTranslationsDirectory("/usr/share/qt4/translations");
#endif
  buildAvailableTranslationsList();
  changeLanguage();

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
  ///return instance()->pvSystemDataDirPath;
  return instance()->pvSystemDataDir.absolutePath();
}

const QDir mdtApplication::cacheDir()
{
  return instance()->pvCacheDir;
}

QString mdtApplication::mdtLibVersion()
{
  return QString(QString::number(MDTLIB_VERSION_MAJOR) + "." + QString::number(MDTLIB_VERSION_MINOR) + "." + QString::number(MDTLIB_VERSION_MICRO));
}

void mdtApplication::addTranslationsDirectory(const QString &directory)
{
  QDir dir;

  if(dir.cd(directory)){
    pvTranslationsDirectories << directory;
  }else{
    mdtError e(MDT_QM_FILE_LOAD_ERROR, "Cannot access directory '" + directory +"'", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtApplication");
    e.commit();
  }
}

QMap<QString, QString> &mdtApplication::availableTranslations()
{
  return pvAvailableTranslations;
}

QString &mdtApplication::currentTranslationKey()
{
  return pvCurrentTranslationKey;
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

void mdtApplication::changeLanguage(const QLocale &locale)
{
  QString languageSuffix;

  languageSuffix = locale.name().left(2);

  removeCurrentTranslators();
  pvCurrentTranslationKey = "en";
  if(languageSuffix != "en"){
    if(!loadTranslationFiles(languageSuffix)){
      mdtError e(MDT_QM_FILE_LOAD_ERROR, "Loading a translation file failed for " + languageSuffix, mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtApplication");
      e.commit();
    }
  }

  emit(languageChanged());
}

void mdtApplication::changeLanguage(QAction *action)
{
  Q_ASSERT(action != 0);

  changeLanguage(QLocale(action->data().toString()));
}

bool mdtApplication::searchSystemDataDir()
{
  ///QDir dir;

  // At first, we look in current directory
  pvSystemDataDir = QDir::current();
  if(pvSystemDataDir.cd("data")){
    if(isSystemDataDir(pvSystemDataDir)){
      // Ok, found.
      ///pvSystemDataDirPath = dir.absolutePath();
      return true;
    }
  }
  // Check in application directory
  pvSystemDataDir.setPath(applicationDirPath());
  if(!pvSystemDataDir.exists()){
    std::cerr << "mdtApplication::searchSystemDataDir(): cannot find application directory" << std::endl;
    return false;
  }
  if(pvSystemDataDir.cd("data")){
    if(isSystemDataDir(pvSystemDataDir)){
      // Ok, found.
      ///pvSystemDataDirPath = dir.absolutePath();
      return true;
    }
  }
  // Check in application's parent directory
  if(pvSystemDataDir.cd("../data")){
    if(isSystemDataDir(pvSystemDataDir)){
      // Ok, found.
      ///pvSystemDataDirPath = dir.absolutePath();
      return true;
    }
  }
  // On Linux/Unix, data is probably installed in /usr/share/mdt or /usr/local/share
#ifdef Q_OS_UNIX
  if(pvSystemDataDir.cd("/usr/share/mdt/data")){
    if(isSystemDataDir(pvSystemDataDir)){
      // Ok, found.
      ///pvSystemDataDirPath = dir.absolutePath();
      return true;
    }
  }
  if(pvSystemDataDir.cd("/usr/local/share/mdt/data")){
    if(isSystemDataDir(pvSystemDataDir)){
      // Ok, found.
      ///pvSystemDataDirPath = dir.absolutePath();
      return true;
    }
  }
#endif  // #ifdef Q_OS_UNIX

  // Data directory not found :-(
  std::cerr << "mdtApplication::searchSystemDataDir(): cannot find system data directory" << std::endl;

  return false;
}

bool mdtApplication::isSystemDataDir(QDir &dir)
{
  // Some checks on directories that must be in data
  if(!dir.cd("i18n")){
    std::cerr << "mdtApplication::isSystemDataDir(): subdirectory i18n not found in " << dir.path().toStdString() << std::endl;
    return false;
  }
  dir.cdUp();
  if(!dir.cd("uic")){
    std::cerr << "mdtApplication::isSystemDataDir(): subdirectory uic not found in " << dir.path().toStdString() << std::endl;
    return false;
  }
  dir.cdUp();

  return true;
}

bool mdtApplication::initHomeDir()
{
  ///QDir dir(QDir::home());

  pvLogDir = QDir::home();

  // Home directory must exists first..
  if(!pvLogDir.exists()){
    std::cerr << "mdtApplication::initHomeDir(): cannot find home directory" << std::endl;
    return false;
  }
  // Look about the .mdt directory
  if(!pvLogDir.cd(".mdt")){
    // Not found, try to create it
    if(!pvLogDir.mkdir(".mdt")){
      std::cerr << "mdtApplication::initHomeDir(): cannot create .mdt in home directory" << std::endl;
      return false;
    }
    if(!pvLogDir.cd(".mdt")){
      std::cerr << "mdtApplication::initHomeDir(): cannot enter $home/.mdt directory" << std::endl;
      return false;
    }
  }
  // Log directory
  if(!pvLogDir.cd("log")){
    // Not found, try to create it
    if(!pvLogDir.mkdir("log")){
      std::cerr << "mdtApplication::initHomeDir(): cannot create log in home directory" << std::endl;
      return false;
    }
    if(!pvLogDir.cd("log")){
      std::cerr << "mdtApplication::initHomeDir(): cannot enter $home/.mdt/log directory" << std::endl;
      return false;
    }
  }
  // Cache directory
  pvCacheDir = QDir::home();
  if(!pvCacheDir.cd(".mdt")){
    std::cerr << "mdtApplication::initHomeDir(): cannot enter $home/.mdt/ directory (should allready exists)" << std::endl;
    return false;
  }
  if(!pvCacheDir.cd("cache")){
    // Not found, try to create it
    if(!pvCacheDir.mkdir("cache")){
      std::cerr << "mdtApplication::initHomeDir(): cannot create cache directory in home directory" << std::endl;
      return false;
    }
    if(!pvCacheDir.cd("cache")){
      std::cerr << "mdtApplication::initHomeDir(): cannot enter $home/.mdt/cache directory" << std::endl;
      return false;
    }
  }
  ///pvLogDirPath = dir.absolutePath();

  return true;
}

void mdtApplication::buildAvailableTranslationsList()
{
  QDir dir;
  QFileInfoList filesInfoList;
  QTranslator *translator;
  int i;

  pvAvailableTranslations.clear();

  // Search translations files in known directories
  for(i=0; i<pvTranslationsDirectories.size(); i++){
    if(dir.cd(pvTranslationsDirectories.at(i))){
      filesInfoList << dir.entryInfoList(QStringList("mdtutils_*.qm"));
    }
  }
  // Install found translation files, add language and uninstall translator
  pvAvailableTranslations.insert("en", "English");
  for(i=0; i<filesInfoList.size(); i++){
    translator = new QTranslator;
    if(translator->load(filesInfoList.at(i).absoluteFilePath())){
      QApplication::installTranslator(translator);
      pvAvailableTranslations.insert(filesInfoList.at(i).baseName().right(2), tr("English"));
      QApplication::removeTranslator(translator);
    }
    delete translator;
  }
}

bool mdtApplication::loadTranslationFiles(const QString &languageSuffix)
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
  // Search translations files in known directories
  for(i=0; i<pvTranslationsDirectories.size(); i++){
    if(dir.cd(pvTranslationsDirectories.at(i))){
      filesInfoList << dir.entryInfoList(QStringList("*_" + languageSuffix + ".qm"));
    }
  }
  // Install found translation files
  for(i=0; i<filesInfoList.size(); i++){
    translator = new QTranslator;
    if(!translator->load(filesInfoList.at(i).absoluteFilePath())){
      std::cerr << "mdtApplication::loadTranslationFiles(): cannot load translation file " << filesInfoList.at(i).fileName().toStdString() << std::endl;
      delete translator;
      return false;
    }
    installTranslator(translator);
    pvCurrentTranslationKey = languageSuffix;
  }

  return true;
}
