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
#ifndef MDT_TRANSLATION_TRANSLATION_INFO_H
#define MDT_TRANSLATION_TRANSLATION_INFO_H

#include "QmFileName.h"
#include "MdtTranslation_CoreExport.h"
#include <QString>

namespace Mdt{ namespace Translation{

  /*! \brief Value class that stores informations about a translation
   */
  class MDT_TRANSLATION_CORE_EXPORT TranslationInfo
  {
   public:

    /*! \brief Get absolute file path
     */
    QString absoluteFilePath() const
    {
      return mQmFilePath;
    }

    /*! \brief Get complete file name, without its path
     */
    QString fullFileName() const
    {
      return mQmFileName.fullName();
    }

    /*! \brief Get language code
     *
     * \sa QmFileName::languageCode()
     */
    LanguageCode languageCode() const
    {
      return mQmFileName.languageCode();
    }

    /*! \brief Get file suffix
     *
     * \sa QmFileName::suffix()
     */
    QString fileSuffix() const
    {
      return mQmFileName.suffix();
    }

    /*! \brief Check if translation plugin info a and b are equal
     */
    friend
    bool operator==(const TranslationInfo & a, const TranslationInfo & b)
    {
      return ( QString::compare( a.mQmFilePath, b.mQmFilePath, Qt::CaseInsensitive ) == 0 );
    }

    /*! \brief Create a translation info from a QM file path
     *
     * \pre File designed by \a qmFilePath must have .qm extension.
     *       (it is not checked if the file exists).
     */
    static TranslationInfo fromQmFilePath(const QString & qmFilePath);

   private:

    QmFileName mQmFileName;
    QString mQmFilePath;
  };

}} // namespace Mdt{ namespace Translation{

#endif // #ifndef MDT_TRANSLATION_TRANSLATION_INFO_H
