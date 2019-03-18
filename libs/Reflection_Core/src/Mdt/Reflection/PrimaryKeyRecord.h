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
#ifndef MDT_REFLECTION_PRIMARY_KEY_RECORD_H
#define MDT_REFLECTION_PRIMARY_KEY_RECORD_H

#include "FieldAlgorithm.h"
#include "PrimaryKeyAlgorithm.h"
#include "TypeTraits/PrimaryKeyRecordTag.h"
#include "TypeTraits/IsPrimaryKeyClass.h"
#include "TypeTraits/IsField.h"
#include <QVariant>
#include <boost/mpl/at.hpp>
#include <boost/mpl/contains.hpp>
#include <array>

namespace Mdt{ namespace Reflection{

  /*! \brief Primary key record for a reflected struct
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   */
  template<typename PrimaryKey>
  class PrimaryKeyRecord : TypeTraits::PrimaryKeyRecordTag
  {
    static_assert( TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );

  public:

    using primary_key = PrimaryKey;
    using field_list = typename PrimaryKey::field_list;
    using value_type = QVariant;

    /*! \brief Construct a null primary key record
     *
     * \sa isNull()
     */
    PrimaryKeyRecord() noexcept = default;

    /*! \brief Copy construct a primary key record from \a other
     */
    PrimaryKeyRecord(const PrimaryKeyRecord & other) = default;

    /*! \brief Copy assign \a other to this primary key record
     */
    PrimaryKeyRecord & operator=(const PrimaryKeyRecord & other) = default;

    /*! \brief Move construct a primary key record from \a other
     */
    PrimaryKeyRecord(PrimaryKeyRecord && other) = default;

    /*! \brief Move assign \a other to this primary key record
     */
    PrimaryKeyRecord & operator=(PrimaryKeyRecord && other) = default;

    /*! \brief Check if this primary record is null
     *
     * A null primary key record has a allready
     *  the count of fields related to the primary key,
     *  but initialized with null values.
     *
     * \note if at least 1 value in this primary key record is null,
     *   this method returns true.
     */
    bool isNull() const noexcept
    {
      for(const auto & v : mRecord){
        if(v.isNull()){
          return true;
        }
      }
      return false;
    }

    /*! \brief Get the count of fields in this record
     */
    static constexpr int fieldCount() noexcept
    {
      return primaryKeyFieldCount<PrimaryKey>();
    }

    /*! \brief Set the value for \a Field
     *
     * \pre \a Field must be a field defined in the struct definition associated with a reflected struct
     * \pre \a Field must exist in this primary key record
     */
    template<typename Field, typename T>
    void setValue(const T & v)
    {
      static_assert( TypeTraits::IsField<Field>::value ,
                     "Field must be a field defined in a struct definition associated with a reflected struct" );
      static_assert( boost::mpl::contains<field_list, Field>::value ,
                     "Field must exist in this primary key record" );

      setValueAt( fieldIndexInMplSequence<Field, field_list>(), v );
    }

    /*! \brief Set the value at \a fieldIndex
     *
     * \pre \a fieldIndex must be in valid range ( 0 <= \a fieldIndex < fieldCount() )
     */
    template<typename T>
    void setValueAt(int fieldIndex, const T & v)
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < fieldCount());

      mRecord[fieldIndex].setValue(v);
    }

    /*! \brief Get the variant value at \a fieldIndex
     *
     * \pre \a fieldIndex must be in valid range ( 0 <= \a fieldIndex < fieldCount() )
     */
    const QVariant & variantValueAt(int fieldIndex) const
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < fieldCount());

      return mRecord[fieldIndex];
    }

    /*! \brief Get the value for \a Field
     *
     * \pre \a Field must be a field defined in the struct definition associated with a reflected struct
     * \pre \a Field must exist in this primary key record
     */
    template<typename Field>
    auto value() const
    {
      static_assert( TypeTraits::IsField<Field>::value ,
                     "Field must be a field defined in a struct definition associated with a reflected struct" );
      static_assert( boost::mpl::contains<field_list, Field>::value ,
                     "Field must exist in this primary key record" );

      using field_value_type = typename TypeFromField<Field>::type;
      constexpr int index = fieldIndexInMplSequence<Field, field_list>();

      return variantValueAt(index).template value<field_value_type>();
    }

//     /*! \brief Get the value at \a fieldIndex
//      *
//      * \pre \a fieldIndex must be in valid range ( 0 <= \a fieldIndex < fieldCount() )
//      */
//     auto valueAt(int fieldIndex) const
//     {
//       Q_ASSERT(fieldIndex >= 0);
//       Q_ASSERT(fieldIndex < fieldCount());
// 
//   //     using field = 
//     }

  private:

    std::array<value_type, fieldCount()> mRecord;
  };

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_PRIMARY_KEY_RECORD_H
