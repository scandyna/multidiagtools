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
#ifndef MDT_QUERY_EXPRESSION_SELECT_QUERY_H
#define MDT_QUERY_EXPRESSION_SELECT_QUERY_H

#include "AbstractSelectQuery.h"
#include "SelectStatement.h"
#include "SelectField.h"
#include "Mdt/Error.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>
#include <QVariant>
#include <memory>

namespace Mdt{ namespace QueryExpression{

  /*! \brief SelectQuery can execute a SelectStatement
   *
   * SelectQuery has value sementics, but it is a handle to a implementation instance
   *  (internally implemented using std::shared_ptr).
   *
   * Example of a class that can execute a query:
   * \code
   * class Person
   * {
   *  public:
   *
   *   Person(const Mdt::QueryExpression::SelectQuery & query)
   *    : mQuery(query)
   *   {
   *   }
   *
   *   Mdt::Expected<PersonList> getPersonAbove29();
   *
   *  private:
   *
   *   Mdt::QueryExpression::SelectQuery mQuery;
   * };
   * \endcode
   *
   * Note that %Person has no dependency to any backend, like SQL.
   *
   * Here is a possible implementation of the query method:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * Mdt::Expected<PersonList> Person::getPersonAbove29()
   * {
   *   Mdt::Expected<PersonList> personList;
   *
   *   SelectField age( FieldName("age"), "A" );
   *
   *   SelectStatement stm;
   *   stm.setEntityName("Person");
   *   stm.addField( FieldName("name") );
   *   stm.addField(age);
   *   stm.addField( FieldName("remarks") );
   *   stm.setFilter( age > 29 );
   *   if(!mQuery.exec(stm)){
   *     personList = mQuery.lastError();
   *     return personList;
   *   }
   *   while(mQuery.next()){
   *     // PersonList::append() takes name, age, remarks. It accpets QVariant and handles null values
   *     personList.append( mQuery.value(0), mQuery.value(1), mQuery.value(2) );
   *   }
   *
   *   return personList;
   * }
   * \endcode
   *
   * Notice that we get the value at column 0, 1 and 2 from the internal record in the query.
   *  It could be better to specify those indexes by names.
   *  fieldIndex() cane help to fetch them.
   *  This method only accepts SelectField, so it can assume a unique index for each field
   *  in a query that refers to multiple entities.
   *  Here is the new implementation:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * Mdt::Expected<PersonList> Person::getPersonAbove29() const
   * {
   *   Mdt::Expected<PersonList> personList;
   *
   *   SelectField age( FieldName("age"), "A" );
   *   SelectField name( FieldName("name") );
   *   SelectField remarks( FieldName("remarks") );
   *
   *   SelectStatement stm;
   *   stm.setEntityName("Person");
   *   stm.addField(name);
   *   stm.addField(age);
   *   stm.addField(remarks);
   *   stm.setFilter( age > 29 );
   *   if(!mQuery.exec(stm)){
   *     personList = mQuery.lastError();
   *     return personList;
   *   }
   *   const auto nameIndex = mQuery.fieldIndex(name);
   *   const auto ageIndex = mQuery.fieldIndex(age);
   *   const auto remarksIndex = mQuery.fieldIndex(remarks);
   *   while(mQuery.next()){
   *     // PersonList::append() takes name, age, remarks. It accpets QVariant and handles null values
   *     personList.append( mQuery.value(nameIndex), mQuery.value(ageIndex), mQuery.value(remarksIndex) );
   *   }
   *
   *   return personList;
   * }
   * \endcode
   *
   * In the application, which uses a SQL database,
   *  the concrete setup is done:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * QSqlDatabase db; // See Qt documentation and Mdt::Sql to setup db
   * auto selectQuery = SelectQuery::make<SqlSelectQuery>();
   * auto & sqlSelectQuery = selectQuery.impl<SqlSelectQuery>();
   * sqlSelectQuery.setDatabase(db);
   *
   * Person person(selectQuery);
   * const auto personAbove29 = person.getPersonAbove29();
   * if(!personAbove29){
   *   // Error handling
   * }
   * doSomeStuff(*personAbove29);
   * \endcode
   *
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * QSqlDatabase db; // See Qt documentation and Mdt::Sql to setup db
   * SelectQueryFactory queryFactory;
   * auto sqlQueryFactory = queryFactory.makeImpl<SqlSelectQuery>();
   * sqlQueryFactory.setDatabase(db);
   * \endcode
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectQuery
  {
   public:

    /*! \brief Execute \a statement
     */
    bool exec(const SelectStatement & statement);

    /*! \brief Get the next record, if avaliable, and position this query to that record
     */
    bool next();

    /*! \brief Get count of field for the last executed statement
     */
    int fieldCount() const;

    /*! \brief Get the field index of \a field
     *
     * Returns the index of \a field if it exists,
     *  otherwise -1
     *  If more than 1 field matches, the first one is returned.
     *
     * \pre \a field must contain a EntityAndField
     */
    int fieldIndex(const SelectField & field) const;

    /*! \brief Get the value for \a fieldIndex from the current record
     *
     * \pre \a fieldIndex must be in a valid range ( 0 <= \a fieldIndex < fieldCount() ).
     */
    QVariant value(int fieldIndex) const;

    /*! \brief Get last error
     */
    Mdt::Error lastError() const;

    /*! \brief Create a select query with a concrete implementation
     */
    template<typename QueryImpl>
    static SelectQuery make()
    {
      return SelectQuery( std::make_shared<QueryImpl>() );
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
//       Q_ASSERT(!isNull());
      Q_ASSERT(dynamic_cast<QueryImpl*>( mImpl.get()) != nullptr);
      return *( dynamic_cast<QueryImpl*>( mImpl.get()) );
    }

   private:

    /*! \brief Check if this query is null
     */
    bool isNull() const
    {
      return mImpl.get() == nullptr;
    }

    SelectQuery(std::shared_ptr<AbstractSelectQuery> && queryImpl)
     : mImpl(queryImpl)
    {
    }

    std::shared_ptr<AbstractSelectQuery> mImpl;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_QUERY_H
