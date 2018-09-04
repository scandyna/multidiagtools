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
#ifndef MDT_QUERY_EXPRESSION_ABSTRACT_ASYNC_SELECT_QUERY_FACTORY_H
#define MDT_QUERY_EXPRESSION_ABSTRACT_ASYNC_SELECT_QUERY_FACTORY_H

#include "AbstractAsyncSelectQuery.h"
#include "MdtQueryExpression_CoreExport.h"
#include <memory>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Base class to create asynchronous query factory implementations
   *
   * A select query factory can be used in business logic in a implementation independent way.
   *
   * Example of a class that can execute a query:
   * \code
   * class SomeUseCase : public QObject
   * {
   *  Q_OBJECT
   *
   *   using SelectQuery = Mdt::QueryExpression::AbstractAsyncSelectQuery;
   *   using SelectQueryFacotory = Mdt::QueryExpression::AbstractAsyncSelectQueryFacory;
   *
   *  public:
   *
   *   SomeUseCase(const std::shared_ptr<SelectQueryFacotory> & selectQueryFacotory, QObject *parent = nullptr)
   *    : QObject(parent),
   *      mPersonAbove29Query( selectQueryFacotory->createSelectQuery() ),
   *      mSelectQueryFactory(selectQueryFacotory)
   *   {
   *     connect(mPersonAbove29Query.get(), &SelectQuery::newRecordAvailable, this, &SomeUseCase::newPersonAbove29Available);
   *     connect(mPersonAbove29Query.get(), &SelectQuery::errorOccured, this, &SomeUseCase::errorOccured);
   *   }
   *
   *   void getPersonAbove29()
   *   {
   *     mPersonAbove29Query->submitStatement( PersonAbove29Statement() );
   *   }
   *
   *  signals:
   *
   *    void newPersonAbove29Available(const Mdt::Container::VariantRecord & record);
   *    void errorOccured(const Mdt::Error & error);
   *
   *  private:
   *
   *   std::unique_ptr<SelectQuery> mPersonAbove29Query;
   *   std::shared_ptr<SelectQueryFacotory> mSelectQueryFactory; // Enshure the factory lives as long we use our queries
   * };
   * \endcode
   * Notice the %newPersonAbove29Available() signal, which gives a record that is a list (a vector) of QVariant.
   *  The %PersonAbove29Statement should specify the order of the fields, so we know how to extract the data from the result
   *  (avoid addAllFields(), which returns probably a undefined order for some backends).
   *
   * In some place, for example in main, the query factory is set up and passed to the use case:
   * \code
   * using namespace Mdt::QueryExpression;
   * using namespace Mdt::Sql;
   *
   * SQLiteConnectionParameters parameters;
   * parameters.setDatabaseFile("/path/to/dbFile.sql");
   *
   * auto selectQueryFacotory = std::make_shared<SqlAsyncSelectQueryFactory>();
   * if(!selectQueryFacotory->setup( parameters.toConnectionParameters() )){
   *   // Error handling. use setupError() to have mor informations
   * }
   *
   * SomeUseCase someUseCase(selectQueryFactory);
   * SomeUseCaseView view(&someUseCase);
   * \endcode
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT AbstractAsyncSelectQueryFactory
  {
   public:

    /*! \brief Constructor
     */
    explicit AbstractAsyncSelectQueryFactory() = default;

    virtual ~AbstractAsyncSelectQueryFactory() = default;

    AbstractAsyncSelectQueryFactory(const AbstractAsyncSelectQueryFactory &) = delete;
    AbstractAsyncSelectQueryFactory & operator=(const AbstractAsyncSelectQueryFactory &) = delete;
    AbstractAsyncSelectQueryFactory(AbstractAsyncSelectQueryFactory &&) = delete;
    AbstractAsyncSelectQueryFactory & operator=(AbstractAsyncSelectQueryFactory &&) = delete;

    /*! \brief Create a asynchronous select query
     */
    virtual std::unique_ptr<AbstractAsyncSelectQuery> createSelectQuery() = 0;

  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ABSTRACT_ASYNC_SELECT_QUERY_FACTORY_H
