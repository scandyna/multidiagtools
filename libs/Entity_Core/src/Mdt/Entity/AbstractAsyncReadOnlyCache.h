/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_ABSTRACT_ASYNC_READ_ONLY_CACHE_H
#define MDT_ENTITY_ABSTRACT_ASYNC_READ_ONLY_CACHE_H

#include "Mdt/Container/VariantRecord.h"
#include "AbstractReadOnlyCache.h"
#include "MdtEntity_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace Entity{

  /*! \brief Cache between a asynchronous storage and a view
   *
   * When presenting a set of data, typically with Qt's Model/View framework,
   *  the view will query the model about data many times.
   *  If data must be fetched from a storage (for example network, database, ...),
   *  it is not very trivial to map it to a table, and fetching the same data
   *  again and again will be slow.
   *
   * AbstractAsyncReadOnlyCache does not know how to fetch the data,
   *  and therefor it can not be instantiated.
   *
   * A concrete class has to be implemented:
   * \code
   * class AsyncPersonCache : public Mdt::Entity::AbstractAsyncReadOnlyCache
   * {
   *  Q_OBJECT
   *
   *   using SelectQuery = Mdt::QueryExpression::AbstractAsyncSelectQuery;
   *   using SelectQueryFactory = Mdt::QueryExpression::AbstractAsyncSelectQueryFactory;
   *
   *  public:
   *
   *   AsyncPersonCache(QObject *parent = nullptr);
   *
   *   void setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory);
   *
   *   int columnCount() const noexcept override
   *   {
   *     return mSelectAllPersonStatement.fieldCount();
   *   }
   *
   *   QString horizontalHeaderName(int column) const override;
   *
   *  private:
   *
   *   bool fetchRecords(int count) override;
   *
   *   Mdt::QueryExpression::SelectStatement mSelectAllPersonStatement;
   *   std::shared_ptr<SelectQueryFactory> mSelectQueryFactory;
   *   std::unique_ptr<SelectQuery> mSelectQuery;
   * };
   * \endcode
   *
   * In the constructor, the statement to select all persons can be created:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * AsyncPersonCache::AsyncPersonCache(QObject *parent = nullptr)
   *  : AbstractAsyncReadOnlyCache(parent)
   * {
   *   mSelectAllPersonStatement.setEntityName("Person");
   *   mSelectAllPersonStatement.addField("name");
   *   mSelectAllPersonStatement.addField("age");
   * }
   * \endcode
   *
   * The horizontal header names looks like:
   * \code
   * QString AsyncPersonCache::horizontalHeaderName(int column) const
   * {
   *   switch(column){
   *     case 0:
   *       return "Name";
   *     case 1:
   *       return "Age";
   *     default:
   *       break;
   *   }
   *
   *   return QString();
   * }
   * \endcode
   *
   * A instance of the shared query factory is hold as member of the cache
   *  to enshure that it exists the whole lifetime of this cache object.
   *  Once the factory is set, the query can be created:
   * \code
   * void AsyncPersonCache::setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory)
   * {
   *   mSelectQueryFactory = factory;
   *   mSelectQuery = std::move( mSelectQueryFactory->createSelectQuery() );
   *   connect( mSelectQuery.get(), &SelectQuery::newRecordAvailable, this, &AsyncPersonCache::fromBackendAppendRecord );
   *   connect( mSelectQuery.get(), &SelectQuery::errorOccured, this, &AsyncPersonCache::setLastError );
   * }
   * \endcode
   *
   * Fetching records could be implemented like this:
   * \code
   * bool AsyncPersonCache::fetchRecords(int count)
   * {
   *   mSelectQuery->submitStatement(mSelectAllPersonStatement, count);
   *
   *   return true;
   * }
   * \endcode
   *
   * \sa Mdt::QueryExpression::SelectStatement
   * \sa Mdt::Entity::EntitySelectStatement
   * \sa Mdt::QueryExpression::AbstractAsyncSelectQuery
   */
  class MDT_ENTITY_CORE_EXPORT AbstractAsyncReadOnlyCache : public AbstractReadOnlyCache
  {
   Q_OBJECT

    using BaseClass = AbstractReadOnlyCache;

   public:

    /*! \brief Constructor
     */
    explicit AbstractAsyncReadOnlyCache(QObject *parent = nullptr);

   private:

    
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_ASYNC_READ_ONLY_CACHE_H
