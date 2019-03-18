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
#ifndef MDT_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H
#define MDT_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H

#include "PrimaryKeyAlgorithm.h"
// #include "PrimaryKey.h"
// #include "StructAlgorithm.h"
// #include "FieldAlgorithm.h"
#include "TypeTraits/IsPrimaryKeyRecord.h"
// #include "TypeTraits/IsAutoIncrementIdPrimaryKey.h"
// #include "TypeTraits/IsIdPrimaryKey.h"
// #include "TypeTraits/IsPrimaryKeyClass.h"
#include "TypeTraits/IsReflectedStructAssociatedWithPrimaryKey.h"
// #include "TypeTraits/IsFieldStructDefAssociatedWithPrimaryKey.h"
// #include "Impl/AddFieldNameToList.h"
#include <QString>
// #include <QStringList>
// #include <boost/mpl/for_each.hpp>
// #include <boost/mpl/size.hpp>
// #include <boost/mpl/contains.hpp>
// #include <type_traits>

namespace Mdt{ namespace Reflection{

  namespace Impl{

    /*! \internal
     */
    template<typename PrimaryKeyRecord>
    class AddValueToPrimaryKeyRecord
    {
     public:

      AddValueToPrimaryKeyRecord(PrimaryKeyRecord & record)
       : mRecord(record)
      {
      }

      template<typename Field, typename Value>
      void operator()(const Field &, const Value & value)
      {
        mRecord.template setValue<Field>(value);
      }

     private:

      PrimaryKeyRecord & mRecord;
    };

  } // namespace Impl{

  /*! \brief Get a primary key record from a reflected struct
   *
   * \pre \a PrimaryKeyRecord must be a primary key record associated with a primary key,
   *    which is associated with \a Struct
   * \pre \a Strcut must be a reflected struct associated with the primary key associated with \a PrimaryKeyRecord
   */
  template<typename PrimaryKeyRecord, typename Struct>
  PrimaryKeyRecord primaryKeyRecordFromStruct(const Struct & s)
  {
    static_assert( TypeTraits::IsPrimaryKeyRecord<PrimaryKeyRecord>::value,
                   "PrimaryKeyRecord must be a primary key record associated with a primary key" );

    using primary_key = typename PrimaryKeyRecord::primary_key;

    static_assert( TypeTraits::IsReflectedStructAssociatedWithPrimaryKey<Struct, primary_key>::value ,
                   "Strcut must be a reflected struct associated with the primary key associated with PrimaryKeyRecord");

    PrimaryKeyRecord pkr;
    Impl::AddValueToPrimaryKeyRecord<PrimaryKeyRecord> f(pkr);

    forEachPrimaryKeyFieldAndValue<primary_key>(s, f);

    return pkr;
  }

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H
