/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_REFLECTION_STORAGE_TABLE_BASE_H
#define MDT_SQL_REFLECTION_STORAGE_TABLE_BASE_H

#include "Mdt/Sql/Connection.h"
#include "Mdt/Error.h"
#include "MdtSql_ReflectionExport.h"
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Base class to create a storage table using reflection
   */
  class MDT_SQL_REFLECTION_EXPORT ReflectionStorageTableBase
  {
   public:

    /*! \brief Construct a storage table
     *
     * \pre \a connection must refer to a valid database handle
     */
    ReflectionStorageTableBase(const Connection & connection);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   protected:

    /*! \brief Get the connection this table uses
     */
    Connection connection() const
    {
      return Connection(mConnectionName);
    }

    /*! \brief Set last error
     *
     * \pre \a error must not be null
     */
    void setLastError(const Mdt::Error & error) const;

   private:

    QString mConnectionName;
    mutable Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Sql{


#endif // #ifndef MDT_SQL_REFLECTION_STORAGE_TABLE_BASE_H
