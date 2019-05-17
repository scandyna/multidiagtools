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
#ifndef MDT_SQL_ASYNC_DELETE_QUERY_H
#define MDT_SQL_ASYNC_DELETE_QUERY_H

#include "AsyncQueryBase.h"
#include "AsyncQueryConnection.h"
#include "DeleteStatement.h"
#include "Mdt/Expected.h"
#include "MdtSql_CoreExport.h"

namespace Mdt{ namespace Sql{

  /*! \brief Execute a SQL insert query asynchronously
   */
  class MDT_SQL_CORE_EXPORT AsyncDeleteQuery : public AsyncQueryBase
  {
    Q_OBJECT

    using BaseClass = AsyncQueryBase;

   public:

    /*! \brief Constructor
     *
     * \pre \a connection must be a valid pointer
     */
    explicit AsyncDeleteQuery(const std::shared_ptr<AsyncQueryConnection> & connection, QObject *parent = nullptr);

    /*! \brief Execute a delete statement synchronously
     */
    bool execStatement(const DeleteStatement & statement);

   public Q_SLOTS:

    /*! \brief Submit a delete statement to be executed asynchronously
     */
    void submitStatement(const DeleteStatement & statement);

  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ASYNC_DELETE_QUERY_H
