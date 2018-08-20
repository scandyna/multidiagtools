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
#ifndef MDT_QUERY_EXPRESSION_SQL_SELECT_QUERY_FACTORY_H
#define MDT_QUERY_EXPRESSION_SQL_SELECT_QUERY_FACTORY_H

#include "SqlSelectQuery.h"
#include "Mdt/QueryExpression/AbstractSelectQueryFactory.h"
#include "MdtQueryExpression_SqlExport.h"
#include <QSqlDatabase>

namespace Mdt{ namespace QueryExpression{

  /*! \brief SQL implementation of a select query factory
   */
  class MDT_QUERYEXPRESSION_SQL_EXPORT SqlSelectQueryFactory : public AbstractSelectQueryFactory
  {
   public:

    /*! \brief Set database connection
     */
    void setDatabase(const QSqlDatabase & db);

    /*! \brief Check if this factory is valid
     */
    bool isValid() const noexcept override
    {
      return mDatabase.isValid();
    }

    /*! \brief Create a select query
     */
    std::unique_ptr<AbstractSelectQuery> createSelectQuery() const override;

    /*! \brief Create a cached select query
     */
    CachedSelectQuery createCachedSelectQuery() const override;

   private:

    void setupQueryImpl(SqlSelectQuery & queryImpl) const;

    QSqlDatabase mDatabase;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SQL_SELECT_QUERY_FACTORY_H
