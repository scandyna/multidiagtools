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

#include "ForeignKey.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityDef.h"
#include "TypeTraits/IsEntityFieldDef.h"

// #include "MdtEntity_CoreExport.h"

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
   */
  template<typename PrimaryEntity, typename ForeignEntity, typename ...ForeignEntityFields>
  class Relation
  {
    static_assert( TypeTraits::IsEntity<PrimaryEntity>::value, "PrimaryEntity must be a entity type" );
    static_assert( TypeTraits::IsEntity<ForeignEntity>::value, "ForeignEntity must be a entity type" );
    static_assert( sizeof...(ForeignEntityFields) >= 1, "A relation must refer to at least 1 field in ForeignEntity" );

   public:

//     /*! \brief Define a relation between a primary and a foreign entity
//      *
//      * This version will take the primary key of the primary entity.
//      *
//      * \todo define FK
//      *
//      * Example:
//      * \code
//      * \endcode
//      */
//     template<typename PrimaryEntity, typename ForeignEntity>
//     static Relation fromEntities()
//     {
//     }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_RELATION_H
