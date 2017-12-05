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
#ifndef MDT_DEPLOY_UTILS_TRANSLATION_H
#define MDT_DEPLOY_UTILS_TRANSLATION_H

#include "PathList.h"
#include "MdtDeployUtils_CoreExport.h"
#include "Mdt/Error.h"
#include "Mdt/Translation/TranslationInfo.h"
#include "Mdt/Translation/TranslationInfoList.h"
#include <QString>
#include <QStringList>
#include <QObject>

namespace Mdt{ namespace DeployUtils{

  class LconvertWrapper;

  /*! \brief Utilities to handle translations
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT Translation : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit Translation(QObject *parent = nullptr);

    /*! \brief Joint QM files
     *
     * Will join \a inTranslations to a single QM file for each binary file in \a binaryFiles ,
     *  as well as existing suffixes (langugaes) existing in \a inTranslations .
     *  Each produced QM file will be named \<binaryFileName\>_\<suffix\>.qm .
     *  \a binaryFiles can contain paths to executables or libraries .
     *  The \<binaryFileName\> is extracted for each file in \a binaryFiles .
     *
     * The resulting QM file will be generated to the directory specified by \a destinationDirectoryPath .
     *  If this directory does not exist, it will be created first .
     *
     * The \a pathPrefixList is used to locate the lconvert tool.
     *  It should be set if a non system wide installed Qt version is used.
     */
    bool joinTranslations(const Mdt::Translation::TranslationInfoList & inTranslations, const QStringList & binaryFiles, const QString & destinationDirectoryPath, const PathList & pathPrefixList);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    bool createDestinationDirectory(const QString & path);
    bool joinTranslationsForSuffixes(const Mdt::Translation::TranslationInfoList & translations, const QString & baseName, const QStringList & suffixes, const QString& destinationDirectoryPath);
    bool joinQmFiles(const Mdt::Translation::TranslationInfoList & translations, const QString & outFilePath);
    void setLastError(const Mdt::Error & error);

    LconvertWrapper *mLconvert;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_TRANSLATION_H
