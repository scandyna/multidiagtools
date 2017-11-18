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
#ifndef MDT_DEPLOY_UTILS_QT_LIBRARY_TRANSLATION_H
#define MDT_DEPLOY_UTILS_QT_LIBRARY_TRANSLATION_H

#include "TranslationInfoList.h"
#include "LibraryInfo.h"
#include "LibraryInfoList.h"
#include "PathList.h"
#include "MdtDeployUtils_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace DeployUtils{

  /*! \brief QtLibraryTranslation is used to find translations files of Qt libraries
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT QtLibraryTranslation : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtLibraryTranslation(QObject *parent = nullptr);

    /*! \brief Find translations for a Qt library
     */
    static TranslationInfoList findTranslations(const LibraryInfo & qtLibrary, const PathList & pathPrefixList);

    /*! \brief Find translations for a list of Qt libraries
     *
     * \sa findTranslations(const LibraryInfo &, const PathList &)
     */
    static TranslationInfoList findTranslations(const LibraryInfoList & qtLibraries, const PathList & pathPrefixList);

   private:

    
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_LIBRARY_TRANSLATION_H
