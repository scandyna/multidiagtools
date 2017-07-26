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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_H
#define MDT_DEPLOY_UTILS_LIBRARY_INFO_H

#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Data value class that stores informations about a library
   */
  class LibraryInfo
  {
   public:

    /*! \brief Set absolute file path
     */
    void setAbsoluteFilePath(const QString & path);

    /*! \brief Get absolute file path
     */
    QString absoluteFilePath() const
    {
      return mAbsoluteFilePath;
    }

    /*! \brief Check if this library info is null
     */
    bool isNull() const
    {
      return mAbsoluteFilePath.isEmpty();
    }

   private:

    QString mAbsoluteFilePath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_H
