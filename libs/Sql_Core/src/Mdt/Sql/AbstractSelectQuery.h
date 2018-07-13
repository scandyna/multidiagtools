/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_SQL_ABSTRACT_SELECT_QUERY_H
#define MDT_SQL_ABSTRACT_SELECT_QUERY_H

#include "AbstractQuery.h"
#include "MdtSql_CoreExport.h"

namespace Mdt{ namespace Sql{

  /*! \brief Base class for a select query
   */
  class MDT_SQL_CORE_EXPORT AbstractSelectQuery : public AbstractQuery
  {
   Q_OBJECT

   public:

    /*! \brief Construct a select query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    AbstractSelectQuery(const QSqlDatabase & db);

    /*! \brief Construct a select query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    AbstractSelectQuery(QObject *parent, const QSqlDatabase & db);

   private:

    
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ABSTRACT_SELECT_QUERY_H
