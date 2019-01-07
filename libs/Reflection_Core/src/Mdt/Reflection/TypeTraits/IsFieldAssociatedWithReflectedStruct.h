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
#ifndef MDT_REFLECTION_TYPE_TRAITS_IS_FIELD_ASSOCIATED_WITH_REFLECTED_STRUCT_H
#define MDT_REFLECTION_TYPE_TRAITS_IS_FIELD_ASSOCIATED_WITH_REFLECTED_STRUCT_H

#include <type_traits>

namespace Mdt{ namespace Reflection{ namespace TypeTraits{

  /*! \brief 
   */
  template<typename Struct, typename Field>
  struct IsFieldAssociatedWithReflectedStruct : std::is_same< typename Field::struct_def::reflected_struct, Struct >
  {
  };

}}} // namespace Mdt{ namespace Reflection{ namespace TypeTraits{

#endif // #ifndef MDT_REFLECTION_TYPE_TRAITS_IS_FIELD_ASSOCIATED_WITH_REFLECTED_STRUCT_H
