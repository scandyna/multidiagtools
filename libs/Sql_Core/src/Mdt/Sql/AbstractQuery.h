/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_ABSTRACT_QUERY_H
#define MDT_SQL_ABSTRACT_QUERY_H

#include "Mdt/Error.h"
#include "MdtSql_CoreExport.h"
#include <QObject>
#include <QSqlDatabase>

namespace Mdt{ namespace Sql{

  /*! \brief Base class for query
   */
  class MDT_SQL_CORE_EXPORT AbstractQuery : public QObject
  {
   public:

    /*! \brief Construct a query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    AbstractQuery(const QSqlDatabase & db);

    /*! \brief Construct a query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    AbstractQuery(QObject *parent, const QSqlDatabase & db);

    /*! \brief Get last error
     *
     * The returned error's user defined error
     *  is Mdt::ErrorCode.
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   protected:

    /*! \brief Access database
     */
    QSqlDatabase & database()
    {
      return mDatabase;
    }

    /*! \brief Access database
     */
    const QSqlDatabase & constDatabase() const
    {
      return mDatabase;
    }

    /*! \brief Set last error
     *
     * \pre \a error must not be null
     * \pre the user defined error in \a error must be of type Mdt::ErrorCode
     */
    void setLastError(const Mdt::Error & error);

   private:

    QSqlDatabase mDatabase;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ABSTRACT_QUERY_H
