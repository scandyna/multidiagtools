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
#ifndef MDT_ENTITY_PRIMARY_KEY_FIELD_H
#define MDT_ENTITY_PRIMARY_KEY_FIELD_H

#include "Mdt/Assert.h"
#include "FieldAt.h"
#include "TypeTraits/IsEntityDef.h"
#include "MdtEntity_CoreExport.h"
#include <QMetaType>
#include <QVariant>
#include <QString>

namespace Mdt{ namespace Entity{

  /*! \brief Used by PrimaryKey to holds field informations
   */
  class MDT_ENTITY_CORE_EXPORT PrimaryKeyField
  {
   public:

    /*! \brief Construct a primary key field
     */
    constexpr PrimaryKeyField(int index, QVariant::Type type) noexcept
     : mFieldIndex(index),
       mFieldType( static_cast<QMetaType::Type>(type) )
    {
      MDT_ASSERT(mFieldIndex >= 0);
    }

    /*! \brief Get field index
     */
    constexpr int fieldIndex() const noexcept
    {
      return mFieldIndex;
    }

    /*! \brief Get field type
     */
    constexpr QMetaType::Type fieldType() const noexcept
    {
      return mFieldType;
    }

    /*! \brief Get field name
     */
    template<typename EntityDef>
    QString fieldName() const
    {
      static_assert( TypeTraits::IsEntityDef<EntityDef>::value, "EntityDef must be a entity definition type" );
      return fieldNameAt(EntityDef{}, mFieldIndex);
    }

   private:

    int mFieldIndex;
    QMetaType::Type mFieldType;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_PRIMARY_KEY_FIELD_H
