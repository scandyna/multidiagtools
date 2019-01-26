/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_REFLECTION_RELATION_H
#define MDT_REFLECTION_RELATION_H

#include "PrimaryKeyAlgorithm.h"
#include "TypeTraits/RelationTag.h"
#include "TypeTraits/IsPrimaryKeyClass.h"
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

namespace Mdt{ namespace Reflection{

  /*! \brief Relation between 2 reflected structs
   *
   * For example, define %Person and %Address structs:
   * \code
   * struct PersonDataStruct
   * {
   *   qlonglong id = 0;
   *   QString firstName;
   *   QString lastName;
   * };
   *
   * MDT_REFLECT_STRUCT(
   *   (PersonDataStruct),
   *   Person,
   *   (id),
   *   (firstName),
   *   (lastName)
   * )
   *
   *
   * struct AddressDataStruct
   * {
   *   qlonglong id = 0;
   *   QString street;
   *   qlonglong personId = 0;
   * };
   *
   * MDT_REFLECT_STRUCT(
   *   (AddressDataStruct),
   *   Address,
   *   (id),
   *   (street),
   *   (personId)
   * )
   * \endcode
   *
   * In this example, a person can have multiple addresses, and a address belongs to one person.
   *
   * A Relation is composed of a primary struct and a related struct.
   *  The primary struct is typically at the 1 side of a relation and the related struct at the many side of the relation.
   *
   * To create a relation, a primary key must be defined for the primary struct:
   * \code
   * using PersonPrimaryKey = AutoIncrementIdPrimaryKey<PersonDef::id>;
   * \endcode
   *
   * Define now the relation:
   * \code
   * using PersonAddressRelation = Relation<PersonPrimaryKey, AddressDef::personId>;
   * \endcode
   *
   * \note current version of relation requires a primary key on the primary struct.
   *   This maps a strong recommendation for relational databases.
   *   This could envolve if a use case shows the need of a other concept.
   *
   * A relation can also have more than 1 couple of fields.
   *
   * For (non real world) example, if a person is identified as unique by its first name and last name,
   *  the relation will have 2 couple of related fields:
   * \code
   * struct PersonDataStruct
   * {
   *   QString firstName;
   *   QString lastName;
   * };
   *
   * MDT_REFLECT_STRUCT(
   *   (PersonDataStruct),
   *   Person,
   *   (firstName),
   *   (lastName)
   * )
   *
   * struct AddressDataStruct
   * {
   *   qlonglong id = 0;
   *   QString street;
   *   QString personFirstName;
   *   QString personLastName;
   * };
   *
   * MDT_REFLECT_STRUCT(
   *   (AddressDataStruct),
   *   Address,
   *   (personFirstName, FieldFlag::IsRequired),
   *   (personLastName, FieldFlag::IsRequired)
   * )
   * \endcode
   *
   * Define the primary key for person:
   * \code
   * using PersonPrimaryKey = PrimaryKey<PersonDef::firstName, PersonDef::lastName>;
   * \endcode
   *
   * The relation would be:
   * \code
   * using PersonAddressRelation = Relation<PersonPrimaryKey, AddressDef::personFirstName, AddressDef::personLastName>;
   * \endcode
   *
   * Here, the mapping between fields for person and address is done in the order
   *  of the fields defined in %PersonPrimaryKey for the primary struct, person,
   *  and in the order of the fields defined in the relation for the related struct, address:
   *  - PersonDef::firstName - AddressDef::personFirstName
   *  - PersonDef::lastName - AddressDef::personLastName
   *
   * Take a example of a electrical link that links 2 connections:
   * \code
   * struct ConnectionDataStruct
   * {
   *   qlonglong id = 0;
   *   QString label;
   * };
   *
   * MDT_REFLECT_STRUCT(
   *   (ConnectionDataStruct),
   *   Connection,
   *   (id),
   *   (label)
   * )
   *
   * struct LinkDataStruct
   * {
   *   qlonglong id = 0;
   *   qlonglong startConnectionId = 0;
   *   qlonglong endConnectionId = 0;
   *   QString label;
   * };
   *
   * MDT_REFLECT_STRUCT(
   *   (LinkDataStruct),
   *   Link,
   *   (id),
   *   (startConnectionId, FieldFlag::IsRequired),
   *   (endConnectionId, FieldFlag::IsRequired),
   *   (label)
   * )
   * \endcode
   *
   * Define the primary key for connection:
   * \code
   * using ConnectionPrimaryKey = AutoIncrementIdPrimaryKey<ConnectionDef::id>;
   * \endcode
   *
   * For this example, 2 relations could be used:
   * \code
   * using StartConnectionLinkRelation = Relation<ConnectionPrimaryKey, LinkDef::startConnectionId>;
   * using EndConnectionLinkRelation = Relation<ConnectionPrimaryKey, LinkDef::endConnectionId>;
   * \endcode
   *
   * \pre \a Pk must be a primary key class for a reflected struct
   * \pre A relation must contain at least 1 field to the related struct
   * \pre The count of fields in \a Pk must match the count of fields passed as \a RelatedStructFieldList
   */
  template<typename Pk, typename ...RelatedStructFieldList>
  class Relation : TypeTraits::RelationTag
  {
    static_assert( TypeTraits::IsPrimaryKeyClass<Pk>::value, "Pk must be a primary key class for a reflected struct" );
    static_assert( sizeof...(RelatedStructFieldList) >= 1 , "A relation must contain at least 1 field to the related struct" );
    static_assert( sizeof...(RelatedStructFieldList) == primaryKeyFieldCount<Pk>() ,
                   "The count of fields in Pk must match the count of fields passed as RelatedStructFieldList" );


   public:

    using primary_struct_def = typename Pk::struct_def;
    using primary_struct_field_list = typename Pk::field_list;
    using related_struct_field_list = boost::mpl::vector<RelatedStructFieldList...>;
    using related_struct_def = typename boost::mpl::at_c<related_struct_field_list, 0>::type::struct_def;

  };

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_RELATION_H
