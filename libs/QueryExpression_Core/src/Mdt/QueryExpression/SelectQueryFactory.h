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
#ifndef MDT_QUERY_EXPRESSION_SELECT_QUERY_FACTORY_H
#define MDT_QUERY_EXPRESSION_SELECT_QUERY_FACTORY_H

#include "SelectQuery.h"
#include "AbstractSelectQueryFactory.h"
#include "MdtQueryExpression_CoreExport.h"
#include <memory>
#include <type_traits>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Create concrete and configured SelectQuery in a backend independent way
   *
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * QSqlDatabase db; // See Qt documentation and Mdt::Sql to setup db
   * SelectQueryFactory queryFactory;
   * auto & sqlQueryFactory = queryFactory.makeImpl<SqlSelectQuery>();
   * sqlQueryFactory.setDatabase(db);
   *
   * auto selectQuery = queryFactory.createQuery();
   * if(!selectQuery.exec( PersonAbove29Statement() )){
   *   // Error handling
   * }
   * while(selectQuery.next()){
   *   qDebug() << "Name: " << selectQuery.value(0) << ", age: " << selectQuery.value(1);
   * }
   * \endcode
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectQueryFactory
  {
   public:

    /*! \brief Set the implementation of this factory
     *
     * \pre QueryImpl must derive from AbstractSelectQueryFactory
     */
    template<typename QueryImpl>
    QueryImpl & makeImpl()
    {
      static_assert( std::is_base_of<AbstractSelectQueryFactory, QueryImpl>::value, "QueryImpl must be derived from AbstractSelectQueryFactory" );

      mImpl = std::make_shared<QueryImpl>();

      return impl<QueryImpl>();
    }

    /*! \brief Check if this factory is null
     */
    bool isNull() const noexcept
    {
      return (mImpl.get() == nullptr);
    }

    /*! \brief Check if this factory is valid
     *
     * This query is valid if it has a valid implementation
     */
    bool isValid() const noexcept
    {
      return ( !isNull()&&(mImpl->isValid()) );
    }

    /*! \brief Access the hold query implementation
     *
     * Returns a reference to the implementation.
     *
     * \pre \a QueryImpl must be the real implementation
     */
    template<typename QueryImpl>
    QueryImpl & impl()
    {
      Q_ASSERT(!isNull());
      Q_ASSERT(dynamic_cast<QueryImpl*>( mImpl.get()) != nullptr);
      return *( dynamic_cast<QueryImpl*>( mImpl.get()) );
    }

    /*! \brief Create a new SelectQuery
     *
     * \pre this factory must be valid
     * \sa isValid()
     */
    SelectQuery createQuery() const
    {
      Q_ASSERT(isValid());

      return mImpl->createSelectQuery();
    }

   private:

    std::shared_ptr<AbstractSelectQueryFactory> mImpl;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_QUERY_FACTORY_H
