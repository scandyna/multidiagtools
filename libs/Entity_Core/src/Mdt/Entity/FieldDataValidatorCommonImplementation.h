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
#ifndef MDT_ENTITY_FIELD_DATA_VALIDATOR_COMMON_IMPLEMENTATION_H
#define MDT_ENTITY_FIELD_DATA_VALIDATOR_COMMON_IMPLEMENTATION_H

#include "FieldAttributes.h"
#include "FieldDataValidatorState.h"
#include "MdtEntity_CoreExport.h"
#include <QString>
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Common implementation of FieldDataValidator
   */
  class MDT_ENTITY_CORE_EXPORT FieldDataValidatorCommonImplementation
  {
   public:

    template<typename T>
    typename std::enable_if< std::is_integral<T>::value, FieldDataValidatorState >::type
    validateData(T data, const FieldAttributes & fieldAttributes) const
    {
      if( (data == 0) && (fieldAttributes.isRequired()) ){
        return FieldDataValidatorState::RequiredButNull;
      }
      return FieldDataValidatorState::Ok;
    }

    FieldDataValidatorState validateData(const QString & data, const FieldAttributes & fieldAttributes) const;

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FIELD_DATA_VALIDATOR_COMMON_IMPLEMENTATION_H
