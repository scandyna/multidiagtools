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

#include "EntityName.h"
#include "FieldName.h"
#include "SelectEntity.h"
#include "SelectField.h"
#include "SelectFieldList.h"
#include "FilterExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Creation of a query to select data
   *
   * \todo Should be renamed SelectStatement
   *
   * \note SelectQuery was inspired from the SQL query API (SELECT statement).
   *   Its goal is to be independent of any backend,
   *   but SQL is a interesting language for query purpose.
   *   When designing queries, you should take care if the targeted backend
   *   can be implemented in a reasonable way.
   *   For example, implementing joins can be difficult for a CSV backend.
   *
   * The following examples assume using the QueryExpression namespace:
   * \code
   * using namespace Mdt::QueryExpression
   * \endcode
   *
   * Here is a example to create a simple select query:
   * \code
   * SelectQuery query;
   * query.setEntityName("Person");
   * query.selectAllFields();
   * \endcode
   *
   * It is also possible to specify a entity alias:
   * \code
   * query.setEntity( EntityName("Person"), "P" );
   * \endcode
   *
   * To specify the fields that the query must report:
   * \code
   * SelectQuery query;
   * query.setEntityName("Person");
   * query.addField("name");
   * query.addField( FieldName("age"), "A" );
   * \endcode
   * notice the age field, to which a alias was passed, A.
   *
   * It is also possible to specify a filter:
   * \code
   * SelectField name( FieldName("name") );
   * SelectField age( FieldName("age"), "A" );
   *
   * SelectQuery query;
   * query.setEntityName("Person");
   * query.addField(name);
   * query.addField(age);
   * query.addField( FieldName("remarks") );
   * query.setFilter( (name == "A") && (age > 29) );
   * \endcode
   *
   * To create more complex queries that joins multiple entities,
   *  passing only field names can be ambigious.
   *  To solve this, it is recommended to specify also the entity name when adding fields.
   *
   * Example of a query that joins a other entity to the primary entity:
   * \code
   * SelectEntity person( EntityName("Person") );
   * SelectEntity address( EntityName("Address"), "ADR");
   *
   * SelectField personId( person, FieldName("id") );
   * SelectField personName( person, FieldName("name") );
   * SelectField addressPersonId( address, FieldName("personId") );
   * SelectField addressStreet( address, FieldName("street"), "AddressStreet" );
   *
   * SelectQuery query;
   * query.setEntity( person );
   * query.addField( personName );
   * query.addField( person, FieldName("remarks"), "PersonRemarks" );
   * query.addField( addressStreet );
   * query.addField( address, FieldName("remarks"), "AddressRemarks" );
   * query.joinEntity( address, addressPersonId == personId );
   * query.setFilter( (personName == "A") || (addressStreet == "Street name B") );
   * \endcode
   *
   *
   * ---------- Above SelectStatement  - Below SelectQuery ------------------
   *
   * SelectQuery can execute a SelectStatement
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
   *   Mdt::Expected<PersonList> getPersonAbove29() const;
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
   * Mdt::Expected<PersonList> Person::getPersonAbove29() const
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
   * auto sqlSelectQuery = selectQuery.impl<SqlSelectQuery>();
   * sqlSelectQuery.setDatabse(db);
   *
   * Person person(selectQuery);
   * const auto personAbove29 = person.getPersonAbove29();
   * if(!personAbove29){
   *   // Error handling
   * }
   * doSomeStuff(*personAbove29);
   * \endcode
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectQuery
  {
   public:

    /*! \brief Set the entity name
     *
     * \pre \a name must not be empty
     */
    void setEntityName(const QString & name);

    /*! \brief Set the entity name and its alias
     *
     * \pre \a name must not be null
     * \pre \a alias must not be empty
     */
    void setEntityName(const EntityName & name, const QString & alias);

    /*! \brief Set the entity name and its optional alias
     *
     * \pre \a entity must not be null
     */
    void setEntity(const SelectEntity & entity);

    /*! \brief Get the entity
     */
    SelectEntity entity() const
    {
      return mEntity;
    }

    /*! \brief Select all the fields
     *
     * This would be equivalent to * in SQL.
     */
    void selectAllFields();

    /*! \brief Select all fields from \a entity
     *
     * This would be equivalent to entity.* in SQL.
     *
     * \pre \a entity must not be null
     */
    void addSelectAllFields(const SelectEntity & entity);

    /*! \brief Add a field to this query
     *
     * \pre \a fieldName must not be empty
     */
    void addField(const QString & fieldName);

    /*! \brief Add a field to this query
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldAlias must not be empty
     */
    void addField(const FieldName & fieldName, const QString & fieldAlias);

    /*! \brief Add a field to this query
     *
     * \todo Preconditions ?
     */
    void addField(const SelectField & field);

    /*! \brief Add a field to this query
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be null
     */
    void addField(const SelectEntity & entity, const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Access the list of fields in this query
     */
    const SelectFieldList & fieldList() const
    {
      return mFieldList;
    }

    /*! \brief Clear this query
     */
    void clear();

   protected:

    /*! \brief Clear all attributes of this query except the entity
     */
    void clearAttributesExceptEntity();

    /*! \brief Clear the entity
     */
    void clearEntity();

   private:

    SelectEntity mEntity;
    SelectFieldList mFieldList;
    FilterExpression mFilter;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_QUERY_H
