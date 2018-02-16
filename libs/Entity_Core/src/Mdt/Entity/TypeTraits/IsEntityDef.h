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
#ifndef MDT_ENTITY_TYPE_TRAITS_IS_ENTITY_DEF_H
#define MDT_ENTITY_TYPE_TRAITS_IS_ENTITY_DEF_H

#include "EntityDefTag.h"
#include <type_traits>

namespace Mdt{ namespace Entity{ namespace TypeTraits{

  /*! \brief Check if T is a entity definition type
   */
  template<typename T>
  struct IsEntityDef : std::is_base_of<EntityDefTag, T>
  {
  };

}}} // namespace Mdt{ namespace Entity{ namespace TypeTraits{

#endif // #ifndef MDT_ENTITY_TYPE_TRAITS_IS_ENTITY_DEF_H
