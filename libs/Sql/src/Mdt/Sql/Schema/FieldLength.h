/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_SQL_SCHEMA_FIELD_LENGTH_H
#define MDT_SQL_SCHEMA_FIELD_LENGTH_H

#include "Mdt/Assert.h"
#include "MdtSqlExport.h"
#include <QtGlobal>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Express a field length in a safe way
   */
  class MDT_SQL_EXPORT FieldLength
  {
   public:

    /*! \brief Construct a null field length
     */
    constexpr FieldLength() noexcept = default;

    /*! \brief Construct a field length
     *
     * If \a length is < 1,
     *  this field length will be null .
     *
     * \pre \a length must be >= 0
     */
    explicit constexpr FieldLength(int length) noexcept
    {
      MDT_ASSERT(length >= 0);
      mValue = length;
    }

    /*! \brief Copy construct a field length from \a other
     */
    constexpr FieldLength(const FieldLength & other) noexcept = default;

    /*! \brief Copy assign \a other to this field length
     */
    constexpr FieldLength & operator=(const FieldLength & other) noexcept = default;

    /*! \brief Move construct a field length from \a other
     */
    constexpr FieldLength(FieldLength && other) noexcept = default;

    /*! \brief Move assign \a other to this field length
     */
    constexpr FieldLength & operator=(FieldLength && other) noexcept = default;

    /*! \brief Check if this field length is null
     */
    constexpr bool isNull() const noexcept
    {
      return (mValue < 1);
    }

    /*! \brief Get the value of this field length
     */
    constexpr int value() const noexcept
    {
      return mValue;
    }

   private:

    int mValue = 0;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_LENGTH_H
