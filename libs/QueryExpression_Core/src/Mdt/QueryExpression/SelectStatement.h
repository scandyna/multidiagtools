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
#ifndef MDT_QUERY_EXPRESSION_SELECT_STATEMENT_H
#define MDT_QUERY_EXPRESSION_SELECT_STATEMENT_H

#include "EntityName.h"
#include "FieldName.h"
#include "SelectEntity.h"
#include "SelectField.h"
#include "SelectFieldList.h"
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
   * stm.setEntity( EntityName("Person"), "P" );
   * \endcode
   *
   * To specify the fields that the query must report:
   * \code
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField("name");
   * stm.addField( FieldName("age"), "A" );
   * \endcode
   * notice the age field, to which a alias was passed, A.
   *
   * It is also possible to specify a filter:
   * \code
   * using Like = LikeExpression;
   *
   * SelectField name( FieldName("name") );
   * SelectField age( FieldName("age"), "A" );
   *
   * SelectStatement stm;
   * stm.setEntityName("Person");
   * stm.addField(name);
   * stm.addField(age);
   * stm.addField( FieldName("remarks") );
   * stm.setFilter( (name == Like("A*")) && (age > 29) );
   * \endcode
   *
   * To create more complex queries that joins multiple entities,
   *  passing only field names can be ambigious.
   *  To solve this, it is recommended to specify also the entity name when adding fields.
   *
   * Example of a statement that joins a other entity to the primary entity:
   * \code
   * SelectEntity person( EntityName("Person") );
   * SelectEntity address( EntityName("Address"), "ADR");
   *
   * SelectField personId( person, FieldName("id") );
   * SelectField personName( person, FieldName("name") );
   * SelectField addressPersonId( address, FieldName("personId") );
   * SelectField addressStreet( address, FieldName("street"), "AddressStreet" );
   *
   * SelectStatement stm;
   * stm.setEntity( person );
   * stm.addField( personName );
   * stm.addField( person, FieldName("remarks"), "PersonRemarks" );
   * stm.addField( addressStreet );
   * stm.addField( address, FieldName("remarks"), "AddressRemarks" );
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

    /*! \brief Add a field to this statement
     *
     * \pre \a fieldName must not be empty
     */
    void addField(const QString & fieldName);

    /*! \brief Add a field to this statement
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldAlias must not be empty
     */
    void addField(const FieldName & fieldName, const QString & fieldAlias);

    /*! \brief Add a field to this statement
     *
     * \todo Preconditions ?
     */
    void addField(const SelectField & field);

    /*! \brief Add a field to this statement
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be null
     */
    void addField(const SelectEntity & entity, const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Get the count of fields this statement contains
     */
    int fieldCount() const noexcept
    {
      return mFieldList.fieldCount();
    }

    /*! \brief Access the list of fields in this statement
     */
    const SelectFieldList & fieldList() const
    {
      return mFieldList;
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

    SelectEntity mEntity;
    SelectFieldList mFieldList;
    FilterExpression mFilter;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_STATEMENT_H
