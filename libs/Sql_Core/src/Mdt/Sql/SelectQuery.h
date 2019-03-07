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
#ifndef MDT_SQL_SELECT_QUERY_H
#define MDT_SQL_SELECT_QUERY_H

#include "AbstractQuery.h"
#include "MdtSql_CoreExport.h"
// #include "SelectStatement.h"

namespace Mdt{ namespace Sql{

  /*! \brief Select data from a database
   *
   * Example to execute a SelectStatement:
   * \code
   * using namespace Mdt::Sql;
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
  class MDT_SQL_CORE_EXPORT SelectQuery : public AbstractQuery
  {
   Q_OBJECT

   public:

    /*! \brief Construct a select query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    SelectQuery(const QSqlDatabase & db);

    /*! \brief Construct a select query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    SelectQuery(QObject *parent, const QSqlDatabase & db);

   private:

    
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_QUERY_H
