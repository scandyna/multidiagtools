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
#ifndef MDT_ENTITY_RELATION_H
#define MDT_ENTITY_RELATION_H

#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityDef.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "TypeTraits/RelationTag.h"

namespace Mdt{ namespace Entity{

  /*! \brief Relation between 2 entities
   *
   * Define a relation between 2 entities.
   *
   * For example, define %Team and %Employee entities:
   * \code
   * struct TeamDataStruct
   * {
   *   qulonglong id;
   *   QString name;
   * };
   *
   * MDT_ENTITY_DEF(
   *   (TeamDataStruct),
   *   Team,
   *   (id, FieldFlag::IsPrimaryKey),
   *   (name)
   * )
   *
   * struct EmployeeDataStruct
   * {
   *   qulonglong id;
   *   QString firstName;
   *   qulonglong teamId;
   * };
   *
   * MDT_ENTITY_DEF(
   *   (EmployeeDataStruct),
   *   Employee,
   *   (id, FieldFlag::IsPrimaryKey),
   *   (firstName),
   *   (teamId)
   * )
   * \endcode
   * in this example, a %Team can have many %Employee
   *  and a %Employee works only in 1 %Team ,
   *  which is a 1 to many relation.
   *
   * A Relation is composed of a primary entity
   *  and a foreign entity.
   *  The primary entity is typically at the 1 side of a relation
   *  and the foreign entity at the many side of the relation.
   *
   * \code
   * using TeamEmployeeRelation = Relation<TeamEntity, EmployeeEntity, EmployeeDef::teamIdField>;
   * \endcode
   *
   * Notice that the field in TeamEntity was not specified in the relation.
   *  The field that has the IsPrimaryKey flag in TeamEntity, id, will be considered.
   *
   * A relation can also have more than 1 couple of fields.
   *
   * For example, if a person is identified as unique by its first name and last name,
   *  the relation will have 2 couple of related fields:
   * \code
   * struct PersonDataStruct
   * {
   *   QString firstName;
   *   QString lastName;
   * };
   *
   * MDT_ENTITY_DEF(
   *   (PersonDataStruct),
   *   Person,
   *   (firstName, FieldFlag::IsPrimaryKey),
   *   (lastName, FieldFlag::IsPrimaryKey)
   * )
   *
   * struct AddressDataStruct
   * {
   *   qulonglong id;
   *   QString street;
   *   QString personFirstName;
   *   QString personLastName;
   * };
   *
   * MDT_ENTITY_DEF(
   *   (AddressDataStruct),
   *   Address,
   *   (personFirstName, FieldFlag::IsRequired),
   *   (personLastName, FieldFlag::IsRequired)
   * )
   * \endcode
   *
   * The relation will look like this:
   * \code
   * using PersonAddressRelation = Relation<PersonEntity, AddressEntity, AddressDef::personFirstNameField, AddressDef::personLastNameField>;
   * \endcode
   *
   * Here, the mapping between fields for %Person and %Address is done in the order
   *  of the declaration in MDT_ENTITY_DEF() :
   *  - PersonDef::firstNameField - AddressDef::personFirstNameField
   *  - PersonDef::lastNameField - AddressDef::personLastNameField
   *
   * Take a example of a electrical link that links 2 connections:
   * \code
   * struct ConnectionDataStruct
   * {
   *   qulonglong id;
   *   QString label;
   * };
   *
   * MDT_ENTITY_DEF(
   *   (ConnectionDataStruct),
   *   Connection,
   *   (id, FieldFlag::IsPrimaryKey),
   *   (label)
   * )
   *
   * struct LinkDataStruct
   * {
   *   qulonglong id;
   *   qulonglong startConnectionId;
   *   qulonglong endConnectionId;
   *   QString label;
   * };
   *
   * MDT_ENTITY_DEF(
   *   (LinkDataStruct),
   *   Link,
   *   (id, FieldFlag::IsPrimaryKey),
   *   (startConnectionId, FieldFlag::IsRequired),
   *   (endConnectionId, FieldFlag::IsRequired),
   *   (label)
   * )
   * \endcode
   *
   * For this example, 2 relations could be used:
   * \code
   * using StartConnectionLinkRelation = Relation<ConnectionEntity, LinkEntity, LinkDef::startConnectionIdField>;
   * using EndConnectionLinkRelation = Relation<ConnectionEntity, LinkEntity, LinkDef::endConnectionIdField>;
   * \endcode
   *
   * \todo Could it be possible to have such syntax, or better:
   * \code
   * using TeamEmployeeRelation = Relation<TeamEntity, EmployeeEntity, EmployeeEntity::Def.teamId>;
   * \endcode
   *
   * \pre \a PrimaryEntity and \a ForeignEntity must be entities based on EntityTemplate (generated by MDT_ENTITY_DEF() )
   * \pre The count of fields declared for \a ForeignEntity must match the count of fields of the primary key in \a PrimaryEntity
   */
  template<typename PrimaryEntity, typename ForeignEntity, typename ...ForeignEntityFields>
  class Relation : TypeTraits::RelationTag
  {
    static_assert( TypeTraits::IsEntity<PrimaryEntity>::value, "PrimaryEntity must be a entity type" );
    static_assert( TypeTraits::IsEntity<ForeignEntity>::value, "ForeignEntity must be a entity type" );
    static_assert( sizeof...(ForeignEntityFields) >= 1, "A relation must refer to at least 1 field in ForeignEntity" );

   public:

    using primary_entity = PrimaryEntity;
    using foreign_entity = ForeignEntity;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_RELATION_H
