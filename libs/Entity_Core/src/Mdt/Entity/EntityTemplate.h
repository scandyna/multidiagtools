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
#ifndef MDT_ENTITY_ENTITY_TEMPLATE_H
#define MDT_ENTITY_ENTITY_TEMPLATE_H

#include "TypeTraits/EntityTag.h"

namespace Mdt{ namespace Entity{

  /*! \brief Template class for a entity
   */
  template<typename EntityDataStruct, typename EntityDef>
  struct EntityTemplate : TypeTraits::EntityTag
  {
    using def_type = EntityDef;
    using data_struct_type = EntityDataStruct;

    /*! \brief Get entity definition
     */
    static const def_type def()
    {
      return def_type{};
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ENTITY_TEMPLATE_H
