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
#ifndef MDT_SQL_DELETE_QUERY_H
#define MDT_SQL_DELETE_QUERY_H

#include "AbstractQuery.h"
#include "FieldName.h"
#include "PrimaryKeyRecord.h"
#include "DeleteStatement.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QVariant>

namespace Mdt{ namespace Sql{

  /*! \brief Delete data in a database table
   *
   * Usage with basic type checking:
   * \code
   * using namespace Mdt::Sql;
   *
   * PrimaryKeyRecord primaryKeyRecord;
   * primaryKeyRecord.addValue(FieldName("Id_PK"), 25);
   *
   * DeleteQuery query(db);
   * query.setTableName("Client_tbl");
   * query.setConditions(primaryKeyRecord);
   * if(!query.exec()){
   *   // Error handling. query.lastError() constains a error description.
   * }
   * \endcode
   *
   * \note It is also possible to not sepcify the conditions,
   *   if which case all the records in the table will be deleted.
   */
  class MDT_SQL_CORE_EXPORT DeleteQuery : public AbstractQuery
  {
   Q_OBJECT

   public:

    /*! \brief Construct a update query that acts on db
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    DeleteQuery(const QSqlDatabase & db);

    /*! \brief Set table name
     *
     * \pre \a name must not be empty
     */
    void setTableName(const QString & name);

    /*! \brief Set the conditions for this update statement
     *
     * \pre \a primaryKeyRecord must not be null
     */
    void setConditions(const PrimaryKeyRecord & primaryKeyRecord);

    /*! \brief Execute this query
     */
    bool exec();

    /*! \brief Clear this query
     */
    void clear();

   private:

    DeleteStatement mStatement;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_DELETE_QUERY_H
