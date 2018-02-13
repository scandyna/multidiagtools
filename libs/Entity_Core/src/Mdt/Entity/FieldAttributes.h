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
#ifndef MDT_ENTITY_FIELD_ATTRIBUTES_H
#define MDT_ENTITY_FIELD_ATTRIBUTES_H

#include "FieldFlags.h"
#include "FieldMaxLength.h"
#include "MdtEntity_CoreExport.h"

namespace Mdt{ namespace Entity{

  /*! \brief Attributes of a entity field
   */
  class MDT_ENTITY_CORE_EXPORT FieldAttributes
  {
   public:

    /*! \brief Construct a null field attributes
     */
    constexpr FieldAttributes() noexcept = default;

    /*! \brief Construct a field attributes
     */
    template<typename ...Ts>
    constexpr FieldAttributes(Ts ...args) noexcept
    {
      setArgs(args...);
    }

    /*! \brief Check if this field attributes is null
     */
    constexpr bool isNull() const noexcept
    {
      return ( !mFlags && mMaxLength.isNull() );
    }

    /*! \brief Check if the field is required
     */
    constexpr bool isRequired() const noexcept
    {
      return mFlags.testFlag(FieldFlag::IsRequired);
    }

    /*! \brief Check if the field is unique
     */
    constexpr bool isUnique() const noexcept
    {
      return mFlags.testFlag(FieldFlag::IsUnique);
    }

    /*! \brief Check if the field is part of the primary key
     */
    constexpr bool isPrimaryKey() const noexcept
    {
      return mFlags.testFlag(FieldFlag::IsPrimaryKey);
    }

    /*! \brief Check if this field has a default value
     */
    constexpr bool hasDefaultValue() const noexcept
    {
      return mFlags.testFlag(FieldFlag::HasDefaultValue);
    }

    /*! \brief Get field max length
     */
    constexpr int maxLength() const noexcept
    {
      return mMaxLength.length();
    }

   private:

    constexpr void setArgs() noexcept
    {
    }

    template<typename ...Ts>
    constexpr void setArgs(FieldFlags flags, Ts... rest) noexcept
    {
      mFlags = flags;
      setArgs(rest...);
    }

    template<typename ...Ts>
    constexpr void setArgs(FieldMaxLength maxLength, Ts... rest) noexcept
    {
      mMaxLength = maxLength;
      setArgs(rest...);
    }

    FieldFlags mFlags;
    FieldMaxLength mMaxLength;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FIELD_ATTRIBUTES_H
