/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ENTITY_FIELD_MAX_LENGTH_H
#define MDT_ENTITY_FIELD_MAX_LENGTH_H

#include <QtGlobal>

namespace Mdt{ namespace Entity{

  /*! \brief Max length attribute of a entity field
   */
  class FieldMaxLength
  {
   public:

    /*! \brief Construct a null field max length
     */
    constexpr FieldMaxLength() noexcept = default;

    /*! \brief Construct a field max length
     *
     * \pre \a length must be > 0
     */
    constexpr explicit FieldMaxLength(int length) noexcept
     : mLength(length)
    {
      Q_ASSERT(mLength > 0);
    }

    /*! \brief Check if this field max length is null
     */
    constexpr bool isNull() const noexcept
    {
      return (mLength == 0);
    }

    /*! \brief Get the field max length
     */
    constexpr int length() const noexcept
    {
      return mLength;
    }

   private:

    int mLength = 0;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FIELD_MAX_LENGTH_H
