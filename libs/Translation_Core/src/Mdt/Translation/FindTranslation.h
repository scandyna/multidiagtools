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
#ifndef MDT_TRANSLATION_FIND_TRANSLATION_H
#define MDT_TRANSLATION_FIND_TRANSLATION_H

#include "TranslationInfoList.h"
#include "Mdt/FileSystem/PathList.h"
#include "Mdt/Expected.h"
#include "MdtTranslation_CoreExport.h"
#include <QString>
#include <QStringList>

class QDir;

namespace Mdt{ namespace Translation{

  /*! \brief Find translations in a set of directories
   *
   * Translations are searched in directories defined
   *  using findTranslationsRoot().
   *
   * \pre \a pathPrefixList must contain at least one path.
   */
  Mdt::Expected<Mdt::Translation::TranslationInfoList> MDT_TRANSLATION_CORE_EXPORT findTranslations(const Mdt::FileSystem::PathList & pathPrefixList);

  /*! \brief Find the root directory of translations
   *
   * For a given path prefix, a subdirectory, named translations,
   *  will be located.
   *  It is considered as a translation directory
   *  if it contains at least 1 *.qm file.
   *
   * \pre \a pathPrefixList must contain at least one path.
   */
  QString MDT_TRANSLATION_CORE_EXPORT findTranslationsRoot(const Mdt::FileSystem::PathList & pathPrefixList);

  /*! \brief Find the root directory of translations
   *
   * For a given path prefix, a subdirectory, named \a directory,
   *  will be located in \a possibleSubdirectories .
   *
   * A path is considered as the expected translation only if a QM file,
   *  for which the base name is \a expectedQmFileBaseName , exists.
   *
   * If \a pathPrefixList contains at least 1 non empty path,
   *  \a directory will only be located starting
   *  from prefixes in it.
   *  Else, system path prefixes will be used.
   *
   * \sa findDirectoryRoot(const QString &, const QStringList &, const Mdt::FileSystem::PathList &)
   */
  QString MDT_TRANSLATION_CORE_EXPORT findDirectoryRoot(const QString & directory, const QStringList & possibleSubdirectories, const QString & expectedQmFileBaseName, const Mdt::FileSystem::PathList & pathPrefixList);

  /*! \brief Find the root directory of translations
   *
   * For a given path prefix, a subdirectory, named \a directory,
   *  will be located in \a possibleSubdirectories .
   *
   * A path is considered as the expected translation only if it contains at least one  a QM file.
   *
   * If \a pathPrefixList contains at least 1 non empty path,
   *  \a directory will only be located starting
   *  from prefixes in it.
   *  Else, system path prefixes will be used.
   *
   * \sa findDirectoryRoot(const QString &, const QStringList &, const QString &, const Mdt::FileSystem::PathList &)
   */
  QString MDT_TRANSLATION_CORE_EXPORT findDirectoryRoot(const QString & directory, const QStringList & possibleSubdirectories, const Mdt::FileSystem::PathList & pathPrefixList);

  /*! \brief Check if a directory is a translation directory
   *
   * Will check if the directory \a directoryPath
   *  contains at least 1 QM file.
   *
   * If \a expectedQmFileBaseName is set,
   *  the directory is considered a translation directory
   *  only if it contains at least 1 QM file
   *  named \<\a expectedQmFileBaseName\>_<xy>.qm .
   *
   * \code
   * bool matches = isTranslationDirectory("/opt/Qt5/translations", "qtbase");
   * // matches will be true only if /opt/Qt5/translations/ contains qtbase_en.qm or qtbase_fr.qm, etc..
   * \endcode
   */
  bool MDT_TRANSLATION_CORE_EXPORT isTranslationDirectory(const QString & directoryPath, const QString & expectedQmFileBaseName = QString());

  bool MDT_TRANSLATION_CORE_EXPORT isTranslationDirectory(const QDir & dir, const QString & expectedQmFileBaseName = QString());
}} // namespace Mdt{ namespace Translation{

#endif // #ifndef MDT_TRANSLATION_FIND_TRANSLATION_H
