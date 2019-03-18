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
#ifndef MDT_REFLECTION_FIELD_ALGORITHM_H
#define MDT_REFLECTION_FIELD_ALGORITHM_H

#include "TypeTraits/IsField.h"
#include "TypeTraits/IsFieldAssociatedWithReflectedStruct.h"
#include <QMetaType>
#include <QString>
#include <QLatin1String>
#include <boost/fusion/include/adapt_assoc_struct.hpp>
#include <boost/fusion/include/distance.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/find.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/find.hpp>
#include <type_traits>

namespace Mdt{ namespace Reflection{

  /*! \brief Get the value for \a Field in \a s
   *
   * \pre \a Field must be a field defined in the struct definition associated with \a Struct
   */
  template<typename Field, typename Struct>
  auto & fieldValue(const Struct & s) noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in the struct definition associated with Struct" );
    static_assert( TypeTraits::IsFieldAssociatedWithReflectedStruct<Struct, Field>::value ,
                   "Field refers to a struct definition that is not associated with Struct" );

    return boost::fusion::at_key<Field>(s);
  }

  /*! \brief Get field name at a index in a reflected struct
   *
   * \pre \a FieldIndex must be >= 0
   */
  template<typename Struct, int FieldIndex>
  static constexpr const char* fieldNameAtInStruct() noexcept
  {
    static_assert( FieldIndex >= 0, "FieldIndex must be >= 0" );

    return boost::fusion::extension::struct_member_name<Struct, FieldIndex>::call();
  }

  /*! \brief Get the field index in a Boost MPL sequence from a field defined in a struct definition
   *
   * \pre \a Field must be a field defined in the struct definition associated with a reflected struct
   */
  template< typename Field, typename MplSequence>
  static constexpr int fieldIndexInMplSequence() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    using first = typename boost::mpl::begin<MplSequence>::type;
    using it = typename boost::mpl::find<MplSequence, Field>::type;

    return boost::mpl::distance<first, it>::value;
  }

  /*! \brief Get the field index in a reflected struct from a field defined in a struct definition
   *
   * \pre \a Field must be a field defined in the struct definition associated with \a Struct
   */
  template<typename Field, typename Struct>
  static constexpr int fieldIndexInStruct() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in the struct definition associated with Struct" );
    static_assert( TypeTraits::IsFieldAssociatedWithReflectedStruct<Struct, Field>::value ,
                   "Field refers to a struct definition that is not associated with Struct" );

    using first = typename boost::fusion::result_of::begin<Struct>::type;
    using it = typename boost::fusion::result_of::find<Struct, Field>::type;

    return boost::fusion::result_of::distance<first, it>::value;
  }

  /*! \brief Get field name from given field defined in a struct def
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   *
   * \sa fieldNameQString()
   */
  template<typename Field>
  static constexpr const char *fieldName() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    using struct_def = typename Field::struct_def;
    using reflected_struct = typename struct_def::reflected_struct;
    constexpr int fieldIndex = fieldIndexInStruct<Field, reflected_struct>();

    return fieldNameAtInStruct<reflected_struct, fieldIndex>();
  }

  /*! \brief Get field name from given field defined in a struct def
   *
   * \sa fieldName()
   */
  template<typename Field>
  static const QString fieldNameQString()
  {
    return QString::fromLatin1( fieldName<Field>() );
  }

  /*! \brief Get the type (in the reflected struct) for a field
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   */
  template<typename Field>
  class TypeFromField
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    using struct_def = typename Field::struct_def;
    using reflected_struct = typename struct_def::reflected_struct;
    using field_type_raw = typename boost::fusion::result_of::at_key<reflected_struct, Field>::type;

   public:

    using type = typename std::remove_reference<field_type_raw>::type;
  };

  /*! \brief Get the QMetaType type for a field
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   * \note This function can be called as constexpr as long as \a Field refers to a type
   *    natively handled by QMetaType. For types declared with Q_DECLARE_METATYPE(),
   *    it cannot be called as constexpr .
   */
  template<typename Field>
  static constexpr QMetaType::Type qMetaTypeFromField() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    using field_type = typename TypeFromField<Field>::type;

    return static_cast<QMetaType::Type>( qMetaTypeId<field_type>() );
  }

  /*! \brief Check if a field is required
   *
   * \note This function only checks the flags
   *   defined while using MDT_REFLECT_STRUCT() ,
   *   no primary key is taken to account
   *   (primary keys are defined outside)
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   */
  template<typename Field>
  constexpr bool isFieldRequired() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    return Field::fieldAttributes().isRequired();
  }

  /*! \brief Check if a field has a default value
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   */
  template<typename Field>
  constexpr bool hasFieldDefaultValue() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    return Field::fieldAttributes().hasDefaultValue();
  }

  /*! \brief Get the max length of a field
   *
   * A length of 0 (< 1) means that the field has no maximum length.
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   */
  template<typename Field>
  constexpr int fieldMaxLength() noexcept
  {
    static_assert( TypeTraits::IsField<Field>::value , "Field must be a field defined in a struct definition associated with a reflected struct" );

    return Field::fieldAttributes().maxLength();
  }

}} // namespace Mdt{ namespace Reflection{


#endif // #ifndef MDT_REFLECTION_FIELD_ALGORITHM_H
