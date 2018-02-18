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
#ifndef MDT_ENTITY_ITEM_MODEL_PRIMARY_KEY_H
#define MDT_ENTITY_ITEM_MODEL_PRIMARY_KEY_H

#include "TypeTraits/IsEntityDef.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "MdtEntity_CoreExport.h"
#include <boost/fusion/include/for_each.hpp>

namespace Mdt{ namespace Entity{

  namespace Impl{

    /*! \internal Functor called by ItemModelPrimaryKey::fromEntity()
     */
    template<typename EntityDataStruct>
    class AddFieldToItemModelPrimaryKeyIf
    {
     public:

      AddFieldToItemModelPrimaryKeyIf(Mdt::ItemModel::PrimaryKey & pk)
       : mPrimaryKey(pk)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef & entityFieldDef) const
      {
        if( entityFieldDef.fieldAttributes().isPrimaryKey() ){
          mPrimaryKey.addColumn(mCurrentFieldIndex);
        }
        ++mCurrentFieldIndex;
      }
 
     private:

      Mdt::ItemModel::PrimaryKey & mPrimaryKey;
      mutable int mCurrentFieldIndex = 0;
    };

  } // namespace Impl{

  /*! \brief Helper class to get a ItemModel primary key from a entity
   */
  class MDT_ENTITY_CORE_EXPORT ItemModelPrimaryKey
  {
   public:

    /*! \brief Get a item model primary key from a entity
     */
    template<typename EntityDataStruct, typename EntityDef>
    static Mdt::ItemModel::PrimaryKey fromEntity()
    {
      static_assert( TypeTraits::IsEntityDef<EntityDef>::value, "EntityDef must be a entity definition type" );

      Mdt::ItemModel::PrimaryKey imPk;
      Impl::AddFieldToItemModelPrimaryKeyIf<EntityDataStruct> op(imPk);

      boost::fusion::for_each(EntityDef{}, op);

      return imPk;
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ITEM_MODEL_PRIMARY_KEY_H
