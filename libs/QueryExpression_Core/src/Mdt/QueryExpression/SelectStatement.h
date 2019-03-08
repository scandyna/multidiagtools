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
#ifndef MDT_QUERY_EXPRESSION_SELECT_STATEMENT_H
#define MDT_QUERY_EXPRESSION_SELECT_STATEMENT_H

#include "EntityName.h"
#include "FieldName.h"
#include "SelectEntity.h"
#include "SelectField.h"

#include "QueryField.h"
#include "SelectFieldList.h"
#include "EntityAlias.h"
#include "FieldAlias.h"
#include "QueryEntity.h"
#include "JoinConstraintExpression.h"
#include "JoinClauseList.h"
#include "FilterExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Creation of a statement to select data
   *
   * \note SelectStatement was inspired from the SQL query API (SELECT statement).
   *   Its goal is to be independent of any backend,
   *   but SQL is a interesting language for query purpose.
   *   When designing select statements, you should take care if the targeted backend
   *   can be implemented in a reasonable way.
   *   For example, implementing joins can be difficult for a CSV backend.
   *
   * The following examples assume using the QueryExpression namespace:
   * \code
   * using namespace Mdt::QueryExpression;
   * \endcode
   *
   * Here is a example to create a simple select statement:
   * \code
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.selectAllFields();
   * \endcode
   *
   * It is also possible to specify a entity alias:
   * \code
   * stm.setEntityName("Person", EntityAlias("P"));
   * \endcode
   *
   * To specify the fields that the query must report:
   * \code
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField("name");
   * stm.addField( "age", FieldAlias("A") );
   * \endcode
   * notice the age field, to which a alias was passed, A.
   *
   * It is also possible to specify a filter:
   * \code
   * using Like = LikeExpression;
   *
   * QueryField name("name");
   * QueryField age( "age", FieldAlias("A") );
   *
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField(name);
   * stm.addField(age);
   * stm.addField("remarks");
   * stm.setFilter( (name == Like("A*")) && (age > 29) );
   * \endcode
   *
   * To create more complex queries that joins multiple entities,
   *  passing only field names can be ambigious.
   *  To solve this, it is recommended to specify also the entity name when adding fields.
   *
   * Example of a statement that joins a other entity to the primary entity:
   * \code
   * QueryEntity person("Person");
   * QueryEntity address("Address", EntityAlias("ADR"));
   *
   * QueryField personId( person, "id" );
   * QueryField personName( person, "name" );
   * QueryField addressPersonId( address, "personId" );
   * QueryField addressStreet( address, "street", FieldAlias("AddressStreet") );
   *
   * SelectStatement stm;
   * stm.setEntity( person );
   * stm.addField( personName );
   * stm.addField( person, "remarks", FieldAlias("PersonRemarks") );
   * stm.addField( addressStreet );
   * stm.addField( address, "remarks", FieldAlias("AddressRemarks") );
   * stm.joinEntity( address, addressPersonId == personId );
   * stm.setFilter( (personName == "A") || (addressStreet == "Street name B") );
   * \endcode
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectStatement
  {
   public:

    /*! \brief Set the entity name
     *
     * \pre \a name must not be empty
     */
    void setEntityName(const QString & name, const EntityAlias & entityAlias = EntityAlias());

    /*! \brief Set the entity name and its alias
     *
     * \pre \a name must not be null
     * \pre \a alias must not be empty
     */
    [[deprecaed]]
    void setEntityName(const EntityName & name, const QString & alias)
    {
    }

    /*! \brief Set the entity name and its optional alias
     *
     * \pre \a entity must not be null
     */
    void setEntity(const QueryEntity & entity);

    /*! \brief Set the entity name and its optional alias
     *
     * \pre \a entity must not be null
     */
    [[deprecaed]]
    void setEntity(const SelectEntity & entity)
    {
    }

    /*! \brief Get the entity
     */
    QueryEntity entity() const
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
    void addSelectAllFields(const QueryEntity & entity);

    /*! \brief Select all fields from \a entity
     *
     * This would be equivalent to entity.* in SQL.
     *
     * \pre \a entity must not be null
     */
    [[deprecaed]]
    void addSelectAllFields(const SelectEntity & entity)
    {
    }

    /*! \brief Add a field to this statement
     *
     * \pre \a fieldName must not be empty
     */
    void addField(const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias());

    /*! \brief Add a field to this statement
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldAlias must not be empty
     */
    [[deprecaed]]
    void addField(const FieldName & fieldName, const QString & fieldAlias)
    {
    }

    /*! \brief Add a field to this statement
     *
     * \todo Preconditions ?
     */
    void addField(const QueryField & field);

    /*! \brief Add a field to this statement
     *
     * \todo Preconditions ?
     */
    [[deprecaed]]
    void addField(const SelectField & field)
    {
    }

    /*! \brief Add a field to this statement
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be empty
     */
    void addField(const QueryEntity & entity, const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias());

    /*! \brief Add a field to this statement
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be null
     */
    [[deprecaed]]
    void addField(const SelectEntity & entity, const FieldName & fieldName, const QString & fieldAlias = QString())
    {
    }

    /*! \brief Get the count of fields this statement contains
     */
    int fieldCount() const noexcept
    {
      return mFieldList.fieldCount();
    }

    /*! \brief Get the index of a field
     *
     * If the requested field was not found, -1 is returned.
     */
    [[deprecated]]
    int fieldIndex(const QueryField & field) const
    {
//       return mFieldList.fieldIndex(field);
    }

    /*! \brief Get the index of a field
     *
     * If the requested field was not found, -1 is returned.
     */
    [[deprecaed]]
    int fieldIndex(const SelectField & field) const
    {
      return mFieldList.fieldIndex(field);
    }

    /*! \brief Access the list of fields in this statement
     */
    const SelectFieldList & fieldList() const
    {
      return mFieldList;
    }

    /*! \brief Join \a entity to this statement
     *
     * \pre \a entity must not be null
     */
    template<typename JoinExpr>
    void joinEntity(const QueryEntity & entity, const JoinExpr & join)
    {
      Q_ASSERT(!entity.isNull());

      JoinConstraintExpression joinConstraint;
      joinConstraint.setJoin(join);
      Q_ASSERT(!joinConstraint.isNull());
      joinEntity(entity, joinConstraint);
    }

    /*! \brief Join \a entity to this statement
     *
     * \pre \a entity must not be null
     */
    template<typename JoinExpr>
    [[deprecaed]]
    void joinEntity(const SelectEntity & entity, const JoinExpr & join)
    {
      Q_ASSERT(!entity.isNull());

//       JoinConstraintExpression joinConstraint;
//       joinConstraint.setJoin(join);
//       Q_ASSERT(!joinConstraint.isNull());
//       joinEntity(entity, joinConstraint);
    }

    /*! \brief Join \a entity to this statement
     *
     * \pre \a entity must not be null
     * \pre \a joinConstraintExpression must not be null
     */
    void joinEntity(const QueryEntity & entity, const JoinConstraintExpression & joinConstraintExpression);

    /*! \brief Check if this statement has at least 1 join clause
     */
    bool hasJoin() const noexcept
    {
      return !mJoinClauseList.isEmpty();
    }

    /*! \brief Access the list of join clauses of this statement
     */
    const JoinClauseList & joinClauseList() const
    {
      return mJoinClauseList;
    }

    /*! \brief Set \a filter to this statement
     */
    template<typename Expr>
    void setFilter(const Expr & filter)
    {
      mFilter.setFilter(filter);
    }

    /*! \brief Set \a filter to this statement
     *
     * \pre \a filter must not be null
     */
    void setFilterExpression(const FilterExpression & filter);

    /*! \brief Check if a filter was set
     */
    bool hasFilter() const noexcept
    {
      return !mFilter.isNull();
    }

    /*! \brief Access the filter of this statement
     */
    const FilterExpression & filter() const
    {
      return mFilter;
    }

    /*! \brief Clear this statement
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

    QueryEntity mEntity;
    SelectFieldList mFieldList;
    JoinClauseList mJoinClauseList;
    FilterExpression mFilter;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_STATEMENT_H
