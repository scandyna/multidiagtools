/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CABLE_LIST_STORAGE_H
#define MDT_CABLE_LIST_STORAGE_H

#include "mdtError.h"
#include <QSqlDatabase>

namespace Mdt{ namespace CableList{

  /*! \brief Common class for entity storage classes
   */
  class Storage
  {
   public:

    /*! \brief Constructor
     */
    Storage(const QSqlDatabase & db);

    // Copy disabled
    Storage(const Storage &) = delete;
    Storage & operator=(const Storage &) = delete;

    /*! \brief Get last error
     */
    mdtError lastError() const
    {
      return pvLastError;
    }

   protected:

    /*! \brief Set last error
     */
    void setLastError(const mdtError & error)
    {
      pvLastError = error;
    }

    /*! \brief Returns a translated version of sourceText
     *
     * Call QObject::tr()
     */
    static QString tr(const char *sourceText);

   private:

    QSqlDatabase pvDatabase;
    mdtError pvLastError;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_STORAGE_H
