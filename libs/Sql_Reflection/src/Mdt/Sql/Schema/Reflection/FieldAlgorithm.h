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
#ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_FIELD_ALGORITHM_H
#define MDT_SQL_REFLECTION_SCHEMA_REFLECTION_FIELD_ALGORITHM_H

#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsField.h"
#include "Mdt/Reflection/TypeTraits/IsFieldAssociatedWithReflectedStruct.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/FieldType.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Sql/Schema/FieldLength.h"
#include <QMetaType>
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{

  /*! \internal Create a SQL field from a field part of a struct definition
   */
  template<typename Field, typename Struct>
  Mdt::Sql::Schema::Field fieldFromReflected(const Struct & s, const FieldTypeMap & fieldTypeMap)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsField<Field>::value , "Field must be a field defined in the struct definition associated with Struct" );
    static_assert( Mdt::Reflection::TypeTraits::IsFieldAssociatedWithReflectedStruct<Struct, Field>::value ,
                   "Field refers to a struct definition that is not associated with Struct" );

    Mdt::Sql::Schema::Field sqlField;
    const QMetaType::Type metaType = Mdt::Reflection::qMetaTypeFromField<Field>();
    constexpr int maxLength = Mdt::Reflection::fieldMaxLength<Field>();

    sqlField.setType( fieldTypeMap.fieldTypeFromQMetaType(metaType, FieldLength(maxLength)) );
    sqlField.setName( QLatin1String(Mdt::Reflection::fieldName<Field>()) );
    sqlField.setRequired( Mdt::Reflection::isFieldRequired<Field>() );
    sqlField.setLength( Mdt::Reflection::fieldMaxLength<Field>() );
    if(Mdt::Reflection::hasFieldDefaultValue<Field>()){
      sqlField.setDefaultValue( Mdt::Reflection::fieldValue<Field>(s) );
    }

    return sqlField;
  }

}}}} // namespace Mdt{ namespace Sql{ namespace Schema{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_SCHEMA_REFLECTION_FIELD_ALGORITHM_H
