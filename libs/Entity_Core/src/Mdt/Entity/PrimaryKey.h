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
#ifndef MDT_ENTITY_PRIMARY_KEY_H
#define MDT_ENTITY_PRIMARY_KEY_H

#include "PrimaryKeyField.h"
#include "Mdt/Entity/FieldAttributes.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityDef.h"
#include "MdtEntity_CoreExport.h"
#include <QtGlobal>
#include <QMetaType>
#include <QVariant>
#include <QStringList>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <type_traits>
#include <vector>

namespace Mdt{ namespace Entity{

  namespace Impl{

    /*! \internal Functor called by PrimaryKey::fromEntity()
     */
    template<typename EntityDataStruct>
    class AddFieldToPrimaryKeyIf;

  } // namespace Impl{

  /*! \brief Helper class to get the primary key of a entity
   */
  class MDT_ENTITY_CORE_EXPORT PrimaryKey
  {
   public:

    /*!\brief Add a field to this primary key
     */
    void addField(const PrimaryKeyField & field)
    {
      mFields.emplace_back(field);
    }

    /*! \brief Get count of fields in this primary key
     */
    int fieldCount() const noexcept
    {
      return mFields.size();
    }

    /*! \brief Check if this primary key is null
     */
    bool isNull() const noexcept
    {
      return fieldCount() < 1;
    }

    /*! \brief Get primary key field at \a index
     *
     * \pre \a index must be in valid range ( 0 >= \a index < fieldCount() ).
     */
    const PrimaryKeyField & fieldAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < fieldCount());
      return mFields[index];
    }

    /*! \brief Get a list of field names hold in this list
     *
     * \note The returned list is rebuilt at each call of this method
     */
    template<typename EntityDef>
    QStringList toFieldNameList() const
    {
      static_assert( TypeTraits::IsEntityDef<EntityDef>::value, "EntityDef must be a entity definition type" );

      QStringList fieldNames;

      fieldNames.reserve( fieldCount() );
      for(const auto & field : mFields){
        fieldNames.append( field.fieldName<EntityDef>() );
      }

      return fieldNames;
    }

    /*! \brief Get the primary key of a entity
     */
    template<typename Entity>
    static PrimaryKey fromEntity()
    {
      static_assert( TypeTraits::IsEntity<Entity>::value, "Entity must be a entity type" );

      PrimaryKey pk;
      Impl::AddFieldToPrimaryKeyIf<typename Entity::data_struct_type> op(pk);
      constexpr typename Entity::def_type def;

      boost::fusion::for_each(def, op);

      return pk;
    }

   private:

    std::vector<PrimaryKeyField> mFields;
  };

  namespace Impl{

    /*! \internal Functor called by PrimaryKey::fromEntity()
     */
    template<typename EntityDataStruct>
    class AddFieldToPrimaryKeyIf
    {
     public:

      AddFieldToPrimaryKeyIf(PrimaryKey & pk)
       : mPrimaryKey(pk)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef & entityFieldDef) const
      {
        if( entityFieldDef.fieldAttributes().isPrimaryKey() ){
          using fieldTypeRaw = typename boost::fusion::result_of::at_key<EntityDataStruct, EntityFieldDef>::type;
          using fieldType = typename std::remove_reference_t<fieldTypeRaw>;

          mPrimaryKey.addField( PrimaryKeyField(mCurrentFieldIndex, QVariant(fieldType{}).type()) );
        }
        ++mCurrentFieldIndex;
      }
 
     private:

      PrimaryKey & mPrimaryKey;
      mutable int mCurrentFieldIndex = 0;
    };

  } // namespace Impl{

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_PRIMARY_KEY_H
