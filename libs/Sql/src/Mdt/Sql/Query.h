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
#ifndef MDT_SQL_QUERY_H
#define MDT_SQL_QUERY_H

#include "Mdt/Error.h"
#include <QSqlDatabase>
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Helper class for some common SQL statements
   *
   * To execute SQL queries, Qt offers QSqlQuery.
   *  Query add some helper methods for common cases,
   *  like building INSERT or UPDATE statement.
   */
  class Query
  {
   public:

    /*! \brief Constructor
     */
    Query(const QSqlDatabase & db);

    /*! \brief Returns a translated version of sourceText
     *
     * Call QObject::tr()
     */
    static QString tr(const char *sourceText);

   private:

    QSqlDatabase pvDatabase;
    Mdt::Error pvLastError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_QUERY_H
