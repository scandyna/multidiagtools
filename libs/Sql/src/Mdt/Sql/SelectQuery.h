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
#ifndef MDT_SQL_SELECT_QUERY_H
#define MDT_SQL_SELECT_QUERY_H

#include "SelectStatement.h"

namespace Mdt{ namespace Sql{

  /*! \brief Select data from a database
   *
   * SelectQuery executes a SelectStatement.
   *
   * Common usage:
   * \code
   * #include "Mdt/Sql/SelectQuery.h"
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::SelectStatement;
   * using Sql::SelectQuery;
   *
   * QSqlDatabase db;  // See Qt documentation to setup db
   * SelectQuery query(db);
   * SelectStatement stm;
   * // To setup stm, see SelectStatement documentation.
   *
   * if(!query.exec(stm)){
   *   // Error handling. query.lastError() constains a error description.
   * }
   * \endcode
   *
   * Note that SelectQuery uses QSqlQuery internally.
   *  If SelectStatement can not handle some query, and a raw SQL string must be executed,
   *  consider using QSqlQuery directly.
   */
  class SelectQuery
  {
   public:

    

   private:

    
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_QUERY_H
