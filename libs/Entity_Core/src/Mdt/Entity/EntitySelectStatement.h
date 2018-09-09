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
#ifndef MDT_ENTITY_ENTITY_SELECT_STATEMENT_H
#define MDT_ENTITY_ENTITY_SELECT_STATEMENT_H

#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "QueryEntity.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/QueryExpression/EntityName.h"
#include "Mdt/QueryExpression/FieldName.h"
#include "Mdt/QueryExpression/SelectEntity.h"
#include "Mdt/QueryExpression/SelectField.h"
#include <QString>

namespace Mdt{ namespace Entity{

  /*! \brief Creation of a query to select data
   *
   * The following example assume using the Entiy namespace:
   * \code
   * using namespace Mdt::Entity;
   * \endcode
   *
   * A select statement could be create like this:
   * \code
   * using namespace Mdt::Entity;
   *
   * EntitySelectStatement<PersonEntity> stm;
   * stm.selectAllFields();
   * stm.setFilter( stm.def().age() > 29 );
   * \endcode
   *
   * It is also possible to specify a entity alias:
   * \code
   * EntitySelectStatement<PersonEntity> stm("P");
   * \endcode
   *
   * To specify the fields that the statement must report:
   * \code
   * EntitySelectStatement<PersonEntity> stm;
   * stm.addField( stm.def().name() );
   * stm.addField( stm.def().age(), "A" );
   * \endcode
   *  notice the age field, to which a alias was passed, A.
   *
   * It is also possible to specify a filter:
   * \code
   * EntitySelectStatement<PersonEntity> stm;
   * const auto name = stm.makeSelectField( stm.def().name() );
   * const auto age = stm.makeSelectField( stm.def().age(), "A" );
   *
   * stm.addField(name);
   * stm.addField(age);
   * stm.setFilter( (name == "A") && (age > 29) );
   * \endcode
   *
   * To create more complex statements that joins multiple entities,
   *  passing only field can be ambigious.
   *  To solve this, it is recommended to specify also the entity when adding fields.
   *
   * Example of a statement that joins a other entity to the primary entity:
   * \code
   * QueryEntity<PersonEntity> person;
   * QueryEntity<AddressEntity> address("ADR");
   *
   * const auto personId = person.makeSelectField( person.def().id() );
   * const auto personName = person.makeSelectField( person.def().name() );
   * const auto addressPersonId = address.makeSelectField( address.def().personId() );
   * const auto addressStreet = address.makeSelectField( address.def().street(), "AddressStreet" );
   *
   * EntitySelectStatement<PersonEntity> stm;
   * stm.addField( personName );
   * stm.addField( person, person.def().remarks(), "PersonRemarks" );
   * stm.addField( addressStreet );
   * stm.addField( address, address.def().remarks(), "AddressRemarks" );
   * stm.joinEntity( address, addressPersonId == personId );
   * stm.setFilter( (personName == "A") || (addressStreet == "Street name B") );
   * \endcode
   *
   * If a select statement is used at many places in the application,
   *  a query object could be created:
   * \code
   * class PersonAbove29Statement : public Mdt::Entity::EntitySelectStatement<PersonEntity>
   * {
   *  public:
   *
   *   PersonAbove29Statement()
   *   {
   *     addField( def().name() );
   *     addField( def().age() );
   *     setFilter( def().age() > 29 );
   *   }
   * };
   * \endcode
   */
  template<typename Entity>
  class EntitySelectStatement : public Mdt::QueryExpression::SelectStatement
  {
    static_assert( TypeTraits::IsEntity<Entity>::value, "Entity must be a entity type" );

    using ParentClass = SelectStatement;

   public:

    using entity_def_type = typename Entity::def_type;

    /*! \brief Construct a select statement
     */
    EntitySelectStatement()
    {
      setEntityName( def().entityName() );
    }

    /*! \brief Construct a select statement with a alias
     *
     * \pre \a alias must nut be empty
     */
    EntitySelectStatement(const QString & alias)
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

    /*! \brief Add a field to this statement
     *
     * \todo Preconditions ?
     */
    void addField(const Mdt::QueryExpression::SelectField & field)
    {
      ParentClass::addField(field);
    }

    /*! \brief Add a field to this statement
     */
    template<typename FieldDef>
    void addField(const FieldDef & fieldDef)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( entity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()) );
    }

    /*! \brief Add a field to this statement
     *
     * \pre \a fieldAlias must not be empty
     */
    template<typename FieldDef>
    void addField(const FieldDef & fieldDef, const QString & fieldAlias)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( entity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()), fieldAlias );
    }

    /*! \brief Add a field to this statement
     */
    template<typename FEntity, typename FieldDef>
    void addField(const QueryEntity<FEntity> & queryEntity, const FieldDef & fieldDef)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( queryEntity.toSelectEntity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()) );
    }

    /*! \brief Add a field to this statement
     *
     * \pre \a fieldAlias must not be empty
     */
    template<typename FEntity, typename FieldDef>
    void addField(const QueryEntity<FEntity> & queryEntity, const FieldDef & fieldDef, const QString & fieldAlias)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      ParentClass::addField( queryEntity.toSelectEntity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()), fieldAlias );
    }

    /*! \brief Join a entity to this statement
     */
    template<typename FEntity, typename JoinConstrainExpr>
    void joinEntity(const QueryEntity<FEntity> & queryEntity, const JoinConstrainExpr & joinConstraint)
    {
      ParentClass::joinEntity(queryEntity.toSelectEntity(), joinConstraint);
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

#endif // #ifndef MDT_ENTITY_ENTITY_SELECT_STATEMENT_H
