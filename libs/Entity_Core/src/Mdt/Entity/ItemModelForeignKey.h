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
#ifndef MDT_ENTITY_ITEM_MODEL_FOREIGN_KEY_H
#define MDT_ENTITY_ITEM_MODEL_FOREIGN_KEY_H

#include "ForeignKey.h"
#include "FieldAt.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityDef.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "MdtEntity_CoreExport.h"
#include <initializer_list>

namespace Mdt{ namespace Entity{

  namespace Impl{ namespace ItemModelForeignKey{

    void addFieldIndexToForeignKeyImpl(Mdt::ItemModel::ForeignKey &);

    template<typename EntityDef, typename Field, typename ...Fields>
    void addFieldIndexToForeignKeyImpl(Mdt::ItemModel::ForeignKey & imFk)
    {
      static_assert( TypeTraits::IsEntityDef<EntityDef>::value, "EntityDef must be a entity definition type" );
      static_assert( TypeTraits::IsEntityFieldDef<Field>::value, "Field must be a entity field definition type" );

      imFk.addColumn( fieldIndex<EntityDef, Field>() );
    }

    template<typename ForeignEntity, typename ...Fields>
    void addFieldIndexToForeignKey(const Mdt::Entity::ForeignKey<ForeignEntity, Fields...> &, Mdt::ItemModel::ForeignKey & imFk )
    {
      static_assert( TypeTraits::IsEntity<ForeignEntity>::value, "ForeignEntity must be a entity type" );

      using entity_def_type = typename ForeignEntity::def_type;

      (void)std::initializer_list<int>{ (addFieldIndexToForeignKeyImpl<entity_def_type, Fields>(imFk) ,0)... };
    }

  }} // namespace Impl{ namespace ItemModelForeignKey{

  /*! \brief Helper class to get a item model foreign key from a entity foreign key
   */
  class MDT_ENTITY_CORE_EXPORT ItemModelForeignKey
  {
   public:

    /*! \brief Get a item model foreign key from a entity foreign key
     */
    template<typename EntityForeignKey>
    static Mdt::ItemModel::ForeignKey fromEntityForeignKey()
    {
      Mdt::ItemModel::ForeignKey imFk;

      Impl::ItemModelForeignKey::addFieldIndexToForeignKey( EntityForeignKey{}, imFk );

      return imFk;
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ITEM_MODEL_FOREIGN_KEY_H
