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
#ifndef MDT_ENTITY_FOREIGN_KEY_H
#define MDT_ENTITY_FOREIGN_KEY_H

#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityFieldDef.h"
// #include "MdtEntity_CoreExport.h"
#include <QString>
#include <QStringList>
#include <initializer_list>

namespace Mdt{ namespace Entity{

  namespace Impl{ namespace ForeignKey{

    void addFieldNameToListImpl(QStringList &);

    template<typename Field, typename ...Fields>
    void addFieldNameToListImpl(QStringList & list)
    {
      static_assert( TypeTraits::IsEntityFieldDef<Field>::value, "Field must be a entity field def type" );
      list.append( Field::fieldName() );
    }

    template<typename ...Fields>
    void addFieldNameToList(QStringList & list)
    {
      (void)std::initializer_list<int>{ (addFieldNameToListImpl<Fields>(list) ,0)... };
    }

  }} // namespace Impl{ namespace ForeignKey{

  /*! \brief Class to define a foreign key to a entity
   *
   * \note ForeignKey is used to implement Relation ,
   *   and is probably not directly useful in a application development-
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
   * Define a foreign key that refers to %Employee:
   * \code
   * using TeamEmployeeFk = ForeignKey<EmployeeEntity, EmployeeDef::teamId>;
   * \endcode
   *
   * \sa Relation
   */
  template<typename ForeignEntity, typename ...Fields>
  class ForeignKey
  {
    static_assert( TypeTraits::IsEntity<ForeignEntity>::value, "ForeignEntity must be a entity type" );
    static_assert( sizeof...(Fields) >= 1, "A ForeignKey must refer to at least 1 field in ForeignEntity" );

   public:

    /*! \brief Get the name of the foreign entity
     */
    static const QString foreignEntityName() noexcept
    {
      constexpr typename ForeignEntity::def_type def;

      return def.entityName();
    }

    /*! \brief Get count of fields referred in foreign entity
     */
    static constexpr int fieldCount() noexcept
    {
      return sizeof...(Fields);
    }

    /*! \brief Get a list of field names referred in foreign entity
     */
    static const QStringList toFieldNameList()
    {
      QStringList fieldNameList;
      fieldNameList.reserve(fieldCount());

      Impl::ForeignKey::addFieldNameToList<Fields...>(fieldNameList);

      return fieldNameList;
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FOREIGN_KEY_H
