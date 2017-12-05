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
#ifndef MDT_DEPLOY_UTILS_FIND_TRANSLATION_H
#define MDT_DEPLOY_UTILS_FIND_TRANSLATION_H

#include "QtModule.h"
#include "QtModuleList.h"
#include "LibraryInfo.h"
#include "LibraryInfoList.h"
#include "PathList.h"
#include "MdtDeployUtils_CoreExport.h"
#include "Mdt/Expected.h"
#include "Mdt/Translation/TranslationInfo.h"
#include "Mdt/Translation/TranslationInfoList.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

//   /*! \brief Find translations for a Qt library
//    *
//    * Translations are searched in directories defined
//    *  using findQtTranslationsRoot().
//    */
//   Mdt::Expected<TranslationInfoList> MDT_DEPLOYUTILS_CORE_EXPORT findQtTranslations(QtModule qtModule, const QStringList & languageSuffixes, const PathList & pathPrefixList);

  /*! \brief Find translations for a list of Qt libraries
   *
   * Translations are searched in directories defined
   *  using findQtTranslationsRoot().
   */
  Mdt::Expected<Mdt::Translation::TranslationInfoList> MDT_DEPLOYUTILS_CORE_EXPORT findQtTranslations(QtModuleList qtModules, const QStringList & languageSuffixes, const PathList & pathPrefixList);

  /*! \brief Get a list of QM file base names for a Qt module
   */
  QStringList MDT_DEPLOYUTILS_CORE_EXPORT getQmFileBaseNameListForQtModule(QtModule qtModule);

  /*! \brief Get a list of QM file base names for a list of Qt modules
   */
  QStringList MDT_DEPLOYUTILS_CORE_EXPORT getQmFileBaseNameListForQtModules(const QtModuleList & qtModules);

  /*! \brief Find the root directory of Qt translations
   *
   * For a given path prefix, a subdirectory, named translations,
   *  will be located in known subdirectories,
   *  like translations or qt5/translations .
   *
   * If \a pathPrefixList contains at least 1 non empty path,
   *  translations directory will only be located starting
   *  from prefixes in it.
   *  Else, system path prefixes will be used.
   */
  QString MDT_DEPLOYUTILS_CORE_EXPORT findQtTranslationsRoot(const PathList & pathPrefixList);

//   /*! \brief Find translations for a Mdt library
//    *
//    * Translations are searched in directories defined
//    *  using findMdtTranslationsRoot().
//    */
//   Mdt::Expected<TranslationInfoList> MDT_DEPLOYUTILS_CORE_EXPORT findMdtTranslations(const LibraryInfo & mdtLibrary, const QStringList & languageSuffixes, const PathList & pathPrefixList);

  /*! \brief Find translations for a list of Mdt libraries
   *
   * Translations are searched in directories defined
   *  using findMdtTranslationsRoot().
   */
  Mdt::Expected<Mdt::Translation::TranslationInfoList> MDT_DEPLOYUTILS_CORE_EXPORT findMdtTranslations(const LibraryInfoList & mdtLibraries, const QStringList & languageSuffixes, const PathList & pathPrefixList);

  /*! \brief Find the root directory of Mdt translations
   *
   * For a given path prefix, a subdirectory, named translations,
   *  will be located in known subdirectories,
   *  like translations .
   *
   * If \a pathPrefixList contains at least 1 non empty path,
   *  translations directory will only be located starting
   *  from prefixes in it.
   *  Else, system path prefixes will be used.
   */
  QString MDT_DEPLOYUTILS_CORE_EXPORT findMdtTranslationsRoot(const PathList & pathPrefixList);

  /*! \brief Find the root directory of translations
   *
   * For a given path prefix, a subdirectory, named \a directory,
   *  will be located in \a possibleSubdirectories .
   *
   * A path is considered as the expectet translation only if a QM file,
   *  for which the base name is \a expectedQmFileBaseName , exists.
   *
   * If \a pathPrefixList contains at least 1 non empty path,
   *  \a directory will only be located starting
   *  from prefixes in it.
   *  Else, system path prefixes will be used.
   */
  QString MDT_DEPLOYUTILS_CORE_EXPORT findDirectoryRoot(const QString & directory, const QStringList & possibleSubdirectories, const QString & expectedQmFileBaseName, const PathList & pathPrefixList);

  /*! \brief Generate a list of QM file names
   */
  QStringList MDT_DEPLOYUTILS_CORE_EXPORT  getQmFileNameList(const QString & qmFileBaseName, const QStringList & languageSuffixes);

  /*! \brief Generate a list of QM file names
   */
  QStringList MDT_DEPLOYUTILS_CORE_EXPORT getQmFileNameList(const QStringList & qmFileBaseNames, const QStringList & languageSuffixes);

  /*! \brief Find translations in a directory
   */
  Mdt::Expected<Mdt::Translation::TranslationInfoList> MDT_DEPLOYUTILS_CORE_EXPORT findTranslationsInDirectory(const QStringList & qmFileNames, const QString & directoryPath);

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FIND_TRANSLATION_H
