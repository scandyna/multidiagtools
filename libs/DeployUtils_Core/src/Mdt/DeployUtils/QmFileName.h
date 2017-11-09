/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_QM_FILE_NAME_H
#define MDT_DEPLOY_UTILS_QM_FILE_NAME_H

#include "MdtDeployUtils_CoreExport.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Representation of a .qm translation file name
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT QmFileName
  {
   public:

    /*! \brief Construct a null QM file name
     */
    QmFileName() = default;

    /*! \brief Construct a from a file name
     *
     * \a fullName should be the file complete name,
     *  for example: Application_Core_fr.qm
     *  It could also contain a country, like:
     *  foo_fr_ca.qm (French-speaking Canada).
     */
    QmFileName(const QString & fullName);

    /*! \brief Copy construct a QM file name from a other
     */
    QmFileName(const QmFileName &) = default;

    /*! \brief Copy assign to this QM file name from a other
     */
    QmFileName & operator=(const QmFileName &) = default;

    /*! \brief Move construct a QM file name from a other
     */
    QmFileName(QmFileName &&) = default;

    /*! \brief Move assign to this QM file name from a other
     */
    QmFileName & operator=(QmFileName &&) = default;

    /*! \brief Get full name
     *
     * Returns the full file name as it was set.
     */
    QString fullName() const
    {
      return mFullName;
    }

    /*! \brief Get base name
     *
     * Returns the name without any suffix
     *  and extention.
     *
     * For example, for foo_fr.qm, foo is returned.
     */
    QString baseName() const
    {
      return mBaseName;
    }

    /*! \brief Get suffix
     *
     * The suffix is composed of
     *  language code (like fr or de or..)
     *  and maybe a country code (like ca or CN or ..).
     *
     * For example, for foo_fr.qm, fr is returned.
     *  For foo_fr_ca.qm, fr_ca is returned.
     */
    QString suffix() const;

    /*! \brief Get language suffix
     *
     * For example, for foo_fr.qm, fr is returned.
     *  For foo_fr_ca.qm, fr is returned.
     */
    QString languageSuffix() const
    {
      return mLanguageSuffix;
    }

    /*! \brief Get country suffix
     *
     * For example, for foo_fr_ca.qm, ca is returned.
     */
    QString countrySuffix() const
    {
      return mCountrySuffix;
    }

    /*! \brief Check if this is a null QM file name
     */
    bool isNull() const
    {
      return mFullName.isEmpty();
    }

   private:

    static bool isLanguageCode(const QString & str);
    static bool isCountryCode(const QString & str);
    void setBaseName(const QStringList & items, int indexOfLastItem);

    QString mFullName;
    QString mBaseName;
    QString mLanguageSuffix;
    QString mCountrySuffix;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QM_FILE_NAME_H
