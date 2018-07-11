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
#ifndef MDT_ENTITY_ENTITY_SELECT_QUERY_H
#define MDT_ENTITY_ENTITY_SELECT_QUERY_H

#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "QueryEntity.h"
#include "Mdt/QueryExpression/SelectQuery.h"
#include "Mdt/QueryExpression/EntityName.h"
#include "Mdt/QueryExpression/FieldName.h"
#include "Mdt/QueryExpression/SelectEntity.h"
#include "Mdt/QueryExpression/SelectField.h"
#include <QString>

namespace Mdt{ namespace Entity{

  /*! \brief Creation of a query to select data
   *
   * The following examples assume using the Entiy namespace:
   * \code
   * using namespace Mdt::Entity;
   * \endcode
   *
   * A query could be create like this:
   * \code
   * EntitySelectQuery<PersonEntity> query;
   * query.selectAllFields();
   * query.setFilter( query.def().age() > 29 );
   * \endcode
   *
   * It is also possible to specify a entity alias:
   * \code
   * EntitySelectQuery<PersonEntity> query("P");
   * \endcode
   *
   * To specify the fields that the query must report:
   * \code
   * EntitySelectQuery<PersonEntity> query;
   * query.addField( query.def().name() );
   * query.addField( query.def().age(), "A" );
   * \endcode
   *  notice the age field, to which a alias was passed, A.
   *
   * It is also possible to specify a filter:
   * \code
   * EntitySelectQuery<PersonEntity> query;
   * const auto name = query.makeSelectField( query.def().name() );
   * const auto age = query.makeSelectField( query.def().age(), "A" );
   *
   * query.addField(name);
   * query.addField(age);
   * query.setFilter( (name == "A") && (age > 29) );
   * \endcode
   *
   * To create more complex queries that joins multiple entities,
   *  passing only field can be ambigious.
   *  To solve this, it is recommended to specify also the entity when adding fields.
   *
   * Example of a query that joins a other entity to the primary entity:
   * \code
   * QueryEntity<PersonEntity> person;
   * QueryEntity<AddressEntity> address("ADR");
   *
   * const auto personId = person.makeSelectField( person.def().id() );
   * const auto personName = person.makeSelectField( person.def().name() );
   * const auto addressPersonId = address.makeSelectField( address.def().personId() );
   * const auto addressStreet = address.makeSelectField( address.def().street(), "AddressStreet" );
   *
   * EntitySelectQuery<PersonEntity> query;
   * query.addField( personName );
   * query.addField( person, person.def().remarks(), "PersonRemarks" );
   * query.addField( addressStreet );
   * query.addField( address, address.def().remarks(), "AddressRemarks" );
   * query.joinEntity( address, addressPersonId == personId );
   * query.setFilter( (personName == "A") || (addressStreet == "Street name B") );
   * \endcode
   *
   * If a query is used at many places in the application,
   *  a query object could be created:
   * \code
   * class PersonAbove29Query : public Mdt::Entity::EntitySelectQuery<PersonEntity>
   * {
   *  public:
   *
   *   PersonAbove29Query()
   *   {
   *     addField( def().name() );
   *     addField( def().age() );
   *     setFilter( def().age() > 29 );
   *   }
   * };
   * \endcode
   */
  template<typename Entity>
  class EntitySelectQuery : public Mdt::QueryExpression::SelectQuery
  {
    static_assert( TypeTraits::IsEntity<Entity>::value, "Entity must be a entity type" );

    using ParentClass = SelectQuery;

   public:

    using entity_def_type = typename Entity::def_type;

    /*! \brief Construct a query
     */
    EntitySelectQuery()
    {
      setEntityName( def().entityName() );
    }

    /*! \brief Construct a query entity with a alias
     *
     * \pre \a alias must nut be empty
     */
    EntitySelectQuery(const QString & alias)
    {
      Q_ASSERT(!alias.trimmed().isEmpty());

      setEntityName( Mdt::QueryExpression::EntityName(def().entityName()), alias );
    }

    /*! \brief Get entity definition
     */
    static const entity_def_type def()
    {
      return Entity::def();
    }

    /*! \brief Add a field to this query
     *
     * \todo Preconditions ?
     */
    void addField(const Mdt::QueryExpression::SelectField & field)
    {
      ParentClass::addField(field);
    }

    /*! \brief Add a field to this query
     */
    template<typename FieldDef>
    void addField(const FieldDef & fieldDef)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( entity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()) );
    }

    /*! \brief Add a field to this query
     *
     * \pre \a fieldAlias must not be empty
     */
    template<typename FieldDef>
    void addField(const FieldDef & fieldDef, const QString & fieldAlias)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( entity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()), fieldAlias );
    }

    /*! \brief Add a field to this query
     */
    template<typename FEntity, typename FieldDef>
    void addField(const QueryEntity<FEntity> & queryEntity, const FieldDef & fieldDef)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( queryEntity.toSelectEntity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()) );
    }

    /*! \brief Add a field to this query
     *
     * \pre \a fieldAlias must not be empty
     */
    template<typename FEntity, typename FieldDef>
    void addField(const QueryEntity<FEntity> & queryEntity, const FieldDef & fieldDef, const QString & fieldAlias)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( queryEntity.toSelectEntity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()), fieldAlias );
    }

    /*! \brief Create a select field
     */
    template<typename FieldDef>
    Mdt::QueryExpression::SelectField makeSelectField(const FieldDef & fieldDef) const
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      return Mdt::QueryExpression::SelectField( entity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()) );
    }

    /*! \brief Create a select field with a field alias
     *
     * \pre \a fieldAlias must not be empty
     */
    template<typename FieldDef>
    Mdt::QueryExpression::SelectField makeSelectField(const FieldDef & fieldDef, const QString & fieldAlias) const
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );
      Q_ASSERT(!fieldAlias.trimmed().isEmpty());

      return Mdt::QueryExpression::SelectField( entity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()), fieldAlias );
    }

    /*! \brief Clear this query
     *
     * Will clear all attributes except the entity
     */
    void clear()
    {
      clearAttributesExceptEntity();
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ENTITY_SELECT_QUERY_H
