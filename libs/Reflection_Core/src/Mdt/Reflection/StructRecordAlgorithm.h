/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_REFLECTION_STRUCT_RECORD_ALGORITHM_H
#define MDT_REFLECTION_STRUCT_RECORD_ALGORITHM_H

#include "StructAlgorithm.h"
#include "Mdt/Container/VariantRecord.h"

namespace Mdt{ namespace Reflection{

  namespace Impl{

    /*! \internal
     */
    class SetVariantRecordValueToStructField
    {
     public:

      SetVariantRecordValueToStructField(const Mdt::Container::VariantRecord & record)
       : mRecord(record)
      {
      }

      template<typename T>
      void operator()(T & valueRef) const
      {
        Q_ASSERT(mIndex < mRecord.columnCount());
        valueRef = mRecord.value(mIndex).value<T>();
        ++mIndex;
      }

     private:

      mutable int mIndex = 0;
      const Mdt::Container::VariantRecord & mRecord;
    };

  } // namespace Impl{

  /*! \brief Get a struct from a record
   *
   * \pre \a Strcut must have been reflected with MDT_REFLECT_STRUCT()
   * \pre the count of columns in \a record must match the count of fields in \a Strcut
   */
  template<typename Struct>
  Struct structFromVariantRecord(const Mdt::Container::VariantRecord & record)
  {
    Q_ASSERT(record.columnCount() == reflectedFieldCountFromStruct<Struct>());

    Struct s;

    Impl::SetVariantRecordValueToStructField f(record);
    forEachValueInStruct(s, f);

    return s;
  }

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_STRUCT_RECORD_ALGORITHM_H
