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
#ifndef MDT_ENTITY_PRIMARY_KEY_RECORD_FIELD_H
#define MDT_ENTITY_PRIMARY_KEY_RECORD_FIELD_H

#include "MdtEntity_CoreExport.h"
#include <QVariant>

namespace Mdt{ namespace Entity{

  /*! \brief Field used by PrimaryKeyRecord
   */
  class MDT_ENTITY_CORE_EXPORT PrimaryKeyRecordField
  {
   public:

    /*! \brief Construct a primary key record field
     *
     * \pre \a fieldIndex must be >= 0
     * \pre \a value must not be null
     */
    PrimaryKeyRecordField(int fieldIndex, const QVariant & value)
     : mFieldIndex(fieldIndex),
       mValue(value)
    {
      Q_ASSERT(mFieldIndex >= 0);
      Q_ASSERT(!mValue.isNull());
    }

    /*! \brief Get field index
     */
    int fieldIndex() const noexcept
    {
      return mFieldIndex;
    }

    /*! \brief Get value
     */
    QVariant value() const
    {
      return mValue;
    }

   private:

    int mFieldIndex;
    QVariant mValue;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_PRIMARY_KEY_RECORD_FIELD_H
