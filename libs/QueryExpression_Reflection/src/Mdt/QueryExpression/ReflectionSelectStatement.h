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
#ifndef MDT_QUERY_EXPRESSION_REFLECTION_SELECT_STATEMENT_H
#define MDT_QUERY_EXPRESSION_REFLECTION_SELECT_STATEMENT_H

#include "ReflectionQueryField.h"
#include "Mdt/QueryExpression/EntityAlias.h"
#include "Mdt/QueryExpression/FieldAlias.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/QueryExpression/QueryField.h"
#include "Mdt/QueryExpression/JoinConstraintExpression.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/RelationAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include "Mdt/Reflection/TypeTraits/IsField.h"
#include "Mdt/Reflection/TypeTraits/IsRelation.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  namespace Impl{

    /*! \internal
     */
    template<typename Statement>
    class AddFieldToReflectionSelectStatement
    {
     public:

      AddFieldToReflectionSelectStatement(Statement & statement)
       : mStatement(statement)
      {
      }

      template<typename Field>
      void operator()(const Field) const
      {
        mStatement.template addField<Field>();
      }

     private:

      Statement & mStatement;
    };

  } // namespace Impl{

  /*! \brief Creation of a statement to select data
   *
   * Here is a example to create a simple select statement:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * ReflectionSelectStatement<PersonDef> stm;
   * stm.selectAllFields();
   * \endcode
   *
   * If the order of fields is important, addAllFields() should be used:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * ReflectionSelectStatement<PersonDef> stm;
   * stm.addAllFields();
   * \endcode
   *
   * It is also possible to specify a entity alias:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * ReflectionSelectStatement<PersonDef> stm( EntityAlias("P") );
   * \endcode
   *
   * To specify the fields that the query must report:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * ReflectionSelectStatement<PersonDef> stm;
   * stm.addField<PersonDef::name>();
   * stm.addField<PersonDef::age>( FieldAlias("A") );
   * \endcode
   * notice the age field, to which a alias was passed, A.
   *
   * It is also possible to specify a filter:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * using Like = LikeExpression;
   *
   * ReflectionQueryField<PersonDef::name> name;
   * ReflectionQueryField<PersonDef::age> age( FieldAlias("A") );
   *
   * ReflectionSelectStatement<PersonDef> stm;
   * stm.addField(name);
   * stm.addField(age);
   * stm.addField<PersonDef::remarks>();
   * stm.setFilter( (name == Like("A*")) && (age > 29) );
   * \endcode
   *
   * Example of a statement that joins a other entity to the primary entity:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * ReflectionQueryField<PersonDef::id> personId;
   * ReflectionQueryField<PersonDef::name> personName;
   * ReflectionQueryField<AddressDef::personId> addressPersonId;
   * ReflectionQueryField<AddressDef::street> addressStreet( FieldAlias("AddressStreet") );
   *
   * ReflectionSelectStatement<PersonDef> stm;
   * stm.addField(personName);
   * stm.addField<PersonDef::remarks>( FieldAlias("PersonRemarks") );
   * stm.addField(addressStreet);
   * stm.addField<AddressDef::remarks>( FieldAlias("AddressRemarks") );
   * stm.joinEntity<AddressDef>( addressPersonId == personId );
   * stm.setFilter( (personName == "A") || (addressStreet == "Street name B") );
   * \endcode
   *
   * The above statement refers to 2 entities (%Person and %Address),
   *  which could produce ambiguities on field names.
   *  This is not a problem here, because the entity names are part of the fields.
   *  For example:
   * \code
   * stm.addField<PersonDef::id>();
   * \endcode
   * if the above expression is converted to SQL, it would be something like:
   * \code
   * Person.id
   * \endcode
   *
   * If entity aliases are used, the above statement would become:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * ReflectionQueryEntity<PersonDef> person( EntityAlias("P") );
   * ReflectionQueryEntity<PersonDef> address( EntityAlias("ADR") );
   *
   * ReflectionQueryField<PersonDef::id> personId(person);
   * ReflectionQueryField<PersonDef::name> personName(person);
   * ReflectionQueryField<AddressDef::personId> addressPersonId(address);
   * ReflectionQueryField<AddressDef::street> addressStreet( address, FieldAlias("AddressStreet") );
   *
   * ReflectionSelectStatement<PersonDef> stm(person);
   * stm.addField(personName);
   * stm.addField<PersonDef::remarks>( person, FieldAlias("PersonRemarks") );
   * stm.addField(addressStreet);
   * stm.addField<AddressDef::remarks>( address, FieldAlias("AddressRemarks") );
   * stm.joinEntity( address, addressPersonId == personId );
   * stm.setFilter( (personName == "A") || (addressStreet == "Street name B") );
   * \endcode
   *
   * Notice that the code become more cumbersome and error prone.
   *  This should be taken into account before using entity aliases.
   *
   * If a relation was defined using Mdt::Reflection::Relation ,
   *  the join can be more simple and less error prone.
   *
   * The relation could befined as this:
   * \code
   * using PersonPrimaryKey = Mdt::Reflection::PrimaryKey<PersonDef::id>;
   * using PersonAddressRelation = Mdt::Reflection::Relation<PersonPrimaryKey, AddressDef::personId>;
   * \endcode
   *
   * The select statement will become:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * ReflectionQueryField<PersonDef::id> personId;
   * ReflectionQueryField<PersonDef::name> personName;
   * ReflectionQueryField<AddressDef::personId> addressPersonId;
   * ReflectionQueryField<AddressDef::street> addressStreet( FieldAlias("AddressStreet") );
   *
   * ReflectionSelectStatement<PersonDef> stm;
   * stm.addField(personName);
   * stm.addField<PersonDef::remarks>( FieldAlias("PersonRemarks") );
   * stm.addField(addressStreet);
   * stm.addField<AddressDef::remarks>( FieldAlias("AddressRemarks") );
   * stm.joinEntity<PersonAddressRelation>();
   * stm.setFilter( (personName == "A") || (addressStreet == "Street name B") );
   * \endcode
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   */
  template<typename StructDef>
  class ReflectionSelectStatement : public SelectStatement
  {
    static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                   "StructDef must be a struct definition assiocated with a reflected struct" );

    using BaseClass = SelectStatement;

   public:

    void setEntityName(const QString & name, const EntityAlias & entityAlias = EntityAlias()) = delete;
    void setEntity(const QueryEntity & entity) = delete;

    /*! \brief Copy construct a select statement from \a other
     */
    ReflectionSelectStatement(const ReflectionSelectStatement & other) = default;

    /*! \brief Copy assign \a other to this select statement
     */
    ReflectionSelectStatement & operator=(const ReflectionSelectStatement & other) = default;

    /*! \brief Move construct a select statement from \a other
     */
    ReflectionSelectStatement(ReflectionSelectStatement && other) = default;

    /*! \brief Move assign \a other to this select statement
     */
    ReflectionSelectStatement & operator=(ReflectionSelectStatement && other) = default;

    /*! \brief Construct a select statement with a optional entity alias
     */
    ReflectionSelectStatement(const EntityAlias & entityAlias = EntityAlias())
    {
      BaseClass::setEntityName( Mdt::Reflection::nameFromStructDefQString<StructDef>(), entityAlias );
    }

    using BaseClass::addField;

    /*! \brief Add a field to this statement
     *
     * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
     */
    template<typename Field>
    void addField(const FieldAlias & fieldAlias = FieldAlias())
    {
      static_assert(Mdt::Reflection::TypeTraits::IsField<Field>::value ,
                    "Field must be a field defined in a struct definition associated with a reflected struct" );

      BaseClass::addField( makeQueryEntityFromField<Field>(), fieldNameQString<Field>(), fieldAlias );
    }

    /*! \brief Add all fields from the main entity to this statement
     *
     * Will add each field defined in \a StructDef to this statement.
     *  Using this method, the backend will explicitly generage
     *  a request that lists each field, in the order defined
     *  by the reflected struct.
     *
     * \code
     * ReflectionSelectStatement<PersonDef> stm;
     * \endcode
     *
     * In SQL, this will generate a query like:
     * \code
     * SELECT Person.id, Person.name
     * FROM Person
     * \endcode
     *
     * \note This is different from selectAllFields()
     */
    void addAllFields()
    {
//       Impl::AddFieldToReflectionSelectStatement< ReflectionSelectStatement > f(*this);
      Impl::AddFieldToReflectionSelectStatement< decltype(*this) > f(*this);
      Mdt::Reflection::forEachFieldInStructDef<StructDef>(f);
    }

    /*! \brief Join a entity to this statement
     *
     * \pre \a JoinStructDef must be a struct definition assiocated with a reflected struct
     * \pre \a join must be a valid join constraint expression
     */
    template<typename JoinStructDef, typename JoinExpr>
    void joinEntity(const JoinExpr & join)
    {
      static_assert( Mdt::Reflection::TypeTraits::IsStructDef<JoinStructDef>::value,
                     "JoinStructDef must be a struct definition assiocated with a reflected struct" );
      static_assert( Mdt::QueryExpression::TypeTraits::isJoinConstraintExpression<JoinExpr>(),
                    "join must be a valid join constraint expression" );

      BaseClass::joinEntity( makeQueryEntityFromStructDef<JoinStructDef>(), join );
    }

    /*! \brief Join a entity to this statement
     *
     * A relation could be defined like this:
     * \code
     * using PersonAddressRelation = Mdt::Reflection::Relation<PersonPrimaryKey, AddressDef::personId>;
     * \endcode
     *
     * When creating a statement,
     *  the entity to join is deduced:
     * \code
     * ReflectionSelectStatement<PersonDef> statement;
     * statement.joinEntity<PersonAddressRelation>();
     * \endcode
     * here, %Address will be joined to the statement.
     *  This is equivalent to:
     * \code
     * ReflectionQueryField<PersonDef::id> personId;
     * ReflectionQueryField<AddressDef::personId> addressPersonId;
     *
     * ReflectionSelectStatement<PersonDef> statement;
     * statement.joinEntity<AddressDef>(addressPersonId == personId);
     * \endcode
     *
     * This can work in both direction of the relation:
     * \code
     * ReflectionSelectStatement<AddressDef> statement;
     * statement.joinEntity<PersonAddressRelation>();
     * \endcode
     * here, %Person will be joined to the statement.
     *  This is equivalent to:
     * \code
     * ReflectionQueryField<PersonDef::id> personId;
     * ReflectionQueryField<AddressDef::personId> addressPersonId;
     *
     * ReflectionSelectStatement<AddressDef> statement;
     * statement.joinEntity<PersonDef>(addressPersonId == personId);
     * \endcode
     *
     * It is also possible to join a entity by a relation
     *  that does not directly refers to the primary entity of the statement,
     *  as long as one part of the relation exists
     *  in the list of joined entities in the statement.
     *
     * For a example, considere a team, that is composed of persons,
     *  each person having a address.
     *
     * Here is the %Team - %Person relation:
     * \code
     * using TeamPersonRelation = Mdt::Reflection::Relation<TeamPrimaryKey, PersonDef::teamId>;
     * \endcode
     *
     * We can create a statement on %Team,
     *  then join %Person then %Address:
     * \code
     * ReflectionSelectStatement<TeamDef> statement;
     * statement.joinEntity<TeamPersonRelation>();
     * statement.joinEntity<PersonAddressRelation>();
     * \endcode
     * here, %Person is joined to %Team,
     *  then %Address is joined to %Person.
     *  The order of calls to joinEntity() is important.
     *  The equivalent explicit code could be:
     * \code
     * ReflectionQueryField<TeamDef::id> teamId;
     * ReflectionQueryField<PersonDef::id> personId;
     * ReflectionQueryField<PersonDef::teamId> personTeamId;
     * ReflectionQueryField<AddressDef::personId> addressPersonId;
     *
     * ReflectionSelectStatement<TeamDef> statement;
     * statement.joinEntity<PersonDef>(personTeamId == teamId);
     * statement.joinEntity<AddressDef>(addressPersonId == personId);
     * \endcode
     *
     * We can create a statement on %Person,
     *  then join %Team then %Address:
     * \code
     * ReflectionSelectStatement<PersonDef> statement;
     * statement.joinEntity<TeamPersonRelation>();
     * statement.joinEntity<PersonAddressRelation>();
     * \endcode
     * The equivalent explicit code could be:
     * \code
     * ReflectionQueryField<TeamDef::id> teamId;
     * ReflectionQueryField<PersonDef::id> personId;
     * ReflectionQueryField<PersonDef::teamId> personTeamId;
     * ReflectionQueryField<AddressDef::personId> addressPersonId;
     *
     * ReflectionSelectStatement<PersonDef> statement;
     * statement.joinEntity<TeamDef>(personTeamId == teamId);
     * statement.joinEntity<AddressDef>(addressPersonId == personId);
     * \endcode
     *
     * \pre \a Relation must be a relation between two reflected structs
     * \pre either the primary entity, or the foreign entity, defined in \a Relation ,
     *   must allready exist in this statement
     *   (i.e. either as primary entity of this statement, or in the list of joined entities).
     */
    template<typename Relation>
    void joinEntity()
    {
      static_assert( Mdt::Reflection::TypeTraits::IsRelation<Relation>::value,
                     "Relation must be a relation between two reflected structs" );

      // Find which entity of the relation exists in this statement - If none, assert
      QString primaryEntityName = Mdt::Reflection::primaryNameFromRelationQString<Relation>();
      QString relatedEntityName = Mdt::Reflection::relatedNameFromRelationQString<Relation>();
      QString entityNameToJoin;
      if( isPrimaryEntityOrExistsInJoinedEntities(primaryEntityName) ){
        entityNameToJoin = relatedEntityName;
      }else{
        entityNameToJoin = primaryEntityName;
        Q_ASSERT( isPrimaryEntityOrExistsInJoinedEntities(relatedEntityName) );
      }
      /*
       * Create the join constraint expression
       * Because each item of the constraint is a equality, we can use the original relation:
       * EntityA::pk == EntityB::fk is the same as EntityB::fk == EntityA::pk
       */
      QueryEntity entityToJoin(entityNameToJoin);
      const auto joinConstraint = JoinConstraintExpression::fromEquality(
        QueryEntity(primaryEntityName), Mdt::Reflection::primaryStructFieldNameListFromRelation<Relation>(),
        QueryEntity(relatedEntityName), Mdt::Reflection::relatedStructFieldNameListFromRelation<Relation>()
      );

      BaseClass::joinEntity(entityToJoin, joinConstraint);
    }


    /*! \brief Clear this statement
     */
    void clear()
    {
      BaseClass::clearAttributesExceptEntity();
    }

   private:

    template<typename EStructDef>
    static QueryEntity makeQueryEntityFromStructDef(const EntityAlias & entityAlias = EntityAlias())
    {
      return QueryEntity( Mdt::Reflection::nameFromStructDefQString<EStructDef>(), entityAlias );
    }

    template<typename Field>
    static QueryEntity makeQueryEntityFromField(const EntityAlias & entityAlias = EntityAlias())
    {
      return QueryEntity( Mdt::Reflection::nameFromFieldQString<Field>(), entityAlias );
    }

    template<typename Field>
    static QString fieldNameQString()
    {
      return Mdt::Reflection::fieldNameQString<Field>();
    }


  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_REFLECTION_SELECT_STATEMENT_H
