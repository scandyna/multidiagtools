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
#ifndef MDT_SQL_ASYNC_UPDATE_QUERY_H
#define MDT_SQL_ASYNC_UPDATE_QUERY_H

#include "AsyncQueryBase.h"
#include "UpdateStatement.h"
#include "AsyncQueryConnection.h"
#include "UpdateQueryAffectedRowsFailureMode.h"
#include "MdtSql_CoreExport.h"
#include <memory>

namespace Mdt{ namespace Sql{

  /*! \brief Execute a SQL update query asynchronously
   */
  class MDT_SQL_CORE_EXPORT AsyncUpdateQuery : public AsyncQueryBase
  {
    Q_OBJECT

    using BaseClass = AsyncQueryBase;

   public:

    /*! \brief Constructor
     *
     * \pre \a connection must be a valid pointer
     */
    explicit AsyncUpdateQuery(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent = nullptr);

    /*! \brief Set the failure mode regarding affected rows
     */
    void setAffectedRowsFailureMode(UpdateQueryAffectedRowsFailureMode mode);

    /*! \brief Execute a update statement synchronously
     */
    bool execStatement(const UpdateStatement & statement);

   public Q_SLOTS:

    /*! \brief Submit a update statement to be executed asynchronously
     */
    void submitStatement(const UpdateStatement & statement);

   private:

    UpdateQueryAffectedRowsFailureMode mUpdateQueryAffectedRowsFailureMode = UpdateQueryAffectedRowsFailureMode::AcceptAnyAffectedRowCount;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_UPDATE_QUERY_H
