/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "FindTranslation.h"
#include "SearchPathList.h"
#include "Console.h"
#include "Mdt/Error.h"
#include <QLatin1String>
#include <QStringBuilder>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QCoreApplication>
// #include <algorithm>
// #include <iterator>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

Mdt::Expected<TranslationInfoList> findQtTranslations(QtModule qtModule, const QStringList & languageSuffixes, const PathList & pathPrefixList)
{
  const auto qmFileBaseNames = getQmFileBaseNameListForQtModule(qtModule);
  const auto qmFileNames = getQmFileNameList(qmFileBaseNames, languageSuffixes);
  const auto translationsRoot = findQtTranslationsRoot(pathPrefixList);
  if(translationsRoot.isEmpty()){
    const auto msg = QCoreApplication::translate("findQtTranslations()","Could not find translations directory for Qt libraries. Searched in: '%1'")
                                                 .arg(pathPrefixList.toStringList().join('\n'));
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "");
    return error;
  }

  return findTranslationsInDirectory(qmFileNames, translationsRoot);
}

Mdt::Expected<TranslationInfoList> findQtTranslations(QtModuleList qtModules, const QStringList& languageSuffixes, const PathList& pathPrefixList)
{
  TranslationInfoList list;

  for(const auto qtModule : qtModules){
    const auto translations = findQtTranslations(qtModule, languageSuffixes, pathPrefixList);
    if(!translations){
      return translations;
    }
    list.addTranslations(*translations);
  }

  return list;
}

QStringList getQmFileBaseNameListForQtModule(QtModule qtModule)
{
  QStringList fileBaseNames;

  switch(qtModule){
    case QtModule::Unknown:
      break;
    case QtModule::Core:
      fileBaseNames = QStringList{"qt","qtbase"};
      break;
    case QtModule::Gui:
      // GUI is part of base
      break;
    case QtModule::Multimedia:
      fileBaseNames = QStringList{"multimedia"};
      break;
    case QtModule::MultimediaWidgets:
      break;
    case QtModule::Network:
      break;
    case QtModule::Positioning:
      break;
    case QtModule::PrintSupport:
      break;
    case QtModule::Quick:
      fileBaseNames = QStringList{"qtquick1","qtquickcontrols"};
      break;
    case QtModule::Sensors:
      break;
    case QtModule::Sql:
      break;
    case QtModule::Svg:
      break;
    case QtModule::Widgets:
      // Widgets is part of base
      break;
    case QtModule::Qml:
    case QtModule::Test:
    case QtModule::DBus:
      break;
  }

  return fileBaseNames;
}

QString findQtTranslationsRoot(const PathList& pathPrefixList)
{
  return findDirectoryRoot("translations", {"qt5",".."}, pathPrefixList);
}

Expected<TranslationInfoList> findMdtTranslations(const LibraryInfo& mdtLibrary, const QStringList& languageSuffixes, const PathList& pathPrefixList)
{

}

Expected<TranslationInfoList> findMdtTranslations(const LibraryInfoList& mdtLibraries, const QStringList& languageSuffixes, const PathList& pathPrefixList)
{

}

QString findMdtTranslationsRoot(const PathList& pathPrefixList)
{
  return findDirectoryRoot("translations", {".."}, pathPrefixList);
}

QString findDirectoryRoot(const QString& directory, const QStringList& possibleSubdirectories, const PathList& pathPrefixList)
{
  QString directoryRoot;
  SearchPathList searchPathList;
  searchPathList.setIncludePathPrefixes(true);
  if(pathPrefixList.isEmpty()){
    searchPathList.setPathPrefixList( PathList::getSystemLibraryPathList() );
  }else{
    searchPathList.setPathPrefixList(pathPrefixList);
  }
  searchPathList.setPathSuffixList(possibleSubdirectories);
  const auto pathList = searchPathList.pathList();

  for(const auto & path : pathList){
    qDebug() << "Searchin in " << path;
    QDir dir( QDir::cleanPath(path + QLatin1String("/") + directory) );
    if(dir.exists()){
      directoryRoot = dir.absolutePath();
      qDebug() << " - found: " << directoryRoot;
      return directoryRoot;
    }
  }

  return directoryRoot;
}

QStringList getQmFileNameList(const QStringList& qmFileBaseNames, const QStringList& languageSuffixes)
{
  QStringList qmFileNames;

  for(const auto & fileBaseName : qmFileBaseNames){
    for(const auto & languageSuffixe : languageSuffixes){
      qmFileNames.append( fileBaseName % QLatin1String("_") % languageSuffixe % QLatin1String(".qm") );
    }
  }

  return qmFileNames;
}

Mdt::Expected<TranslationInfoList> findTranslationsInDirectory(const QStringList& qmFileNames, const QString& directoryPath)
{
  TranslationInfoList translations;

  QDir dir(directoryPath);
  if(!dir.exists()){
    const auto msg = QCoreApplication::translate("findTranslationsInDirectory()","Directory '%1' does not exist.").arg(directoryPath);
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "");
    return error;
  }
  const auto fiList = dir.entryInfoList(QDir::Files);
  for(const auto & fi : fiList){
    if(qmFileNames.contains(fi.fileName(), Qt::CaseInsensitive)){
      translations.addTranslation( TranslationInfo::fromQmFilePath(fi.absoluteFilePath()) );
    }
  }

  return translations;
}

}} // namespace Mdt{ namespace DeployUtils{
