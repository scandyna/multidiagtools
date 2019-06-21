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
#ifndef MDT_SQL_UPDATE_QUERY_H
#define MDT_SQL_UPDATE_QUERY_H

#include "AbstractQuery.h"
#include "FieldName.h"
#include "PrimaryKeyRecord.h"
#include "UpdateStatement.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QVariant>

namespace Mdt{ namespace Sql{

  /*! \brief Update data in a database table
   *
   * Usage with basic type checking:
   * \code
   * using namespace Mdt::Sql;
   *
   * PrimaryKeyRecord primaryKeyRecord;
   * primaryKeyRecord.addValue(FieldName("Id_PK"), 25);
   *
   * UpdateQuery query(connection);
   * query.setTableName("Client_tbl");
   * query.addValue(FieldName("Name"), "Name 1");
   * query.addValue(FieldName("Remark"), "Remarks 1");
   * query.setConditions(primaryKeyRecord);
   * if(!query.exec()){
   *   // Error handling. query.lastError() constains a error description.
   * }
   * \endcode
   *
   * \note It is also possible to not sepcify the conditions,
   *   if which case all the records in the table will be updated.
   */
  class MDT_SQL_CORE_EXPORT UpdateQuery : public AbstractQuery
  {
   Q_OBJECT

   public:

    /*! \brief Failure mode for affected row count
     */
    enum AffectedRowsFailureMode
    {
      AcceptAnyAffectedRowCount,    /*!< This is the default SQL UPDATE behaviour */
      FailIfNoRowAffected,          /*!< Fail if no row is affecte§d */
      FailIfNotExaclyOneRowAffected /*!< Fail if not exactly 1 row is affected */
    };

    /*! \brief Construct a select query that acts on connection
     */
    explicit UpdateQuery(const Connection & connection, QObject *parent = nullptr);

    /*! \brief Set table name
     *
     * \pre \a name must not be empty
     */
    void setTableName(const QString & name);

    /*! \brief Add a value to this query
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldName must not allready exist in this statement
     */
    void addValue(const FieldName & fieldName, const QVariant & value);

    /*! \brief Set the conditions for this update statement
     *
     * \pre \a primaryKeyRecord must not be null
     */
    void setConditions(const PrimaryKeyRecord & primaryKeyRecord);

    /*! \brief Set the failure mode regarding affected rows
     */
    void setAffectedRowsFailureMode(AffectedRowsFailureMode mode);

    /*! \brief Get the failure mode regarding affected rows
     */
    AffectedRowsFailureMode affectedRowsFailureMode() const noexcept
    {
      return mAffectedRowsFailureMode;
    }

    /*! \brief Execute a update statement
     */
    bool execStatement(const UpdateStatement & statement);

    /*! \brief Execute this query
     */
    bool exec();

   private:

    bool checkAtLeastOneRowAffected(int numRowsAffected);
    bool checkExactlyOneRowAffected(int numRowsAffected);

    UpdateStatement mStatement;
    AffectedRowsFailureMode mAffectedRowsFailureMode = AcceptAnyAffectedRowCount;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_UPDATE_QUERY_H
