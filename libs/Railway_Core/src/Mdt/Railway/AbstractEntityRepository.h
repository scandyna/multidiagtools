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
#ifndef MDT_RAILWAY_ABSTRACT_ENTITY_REPOSITORY_H
#define MDT_RAILWAY_ABSTRACT_ENTITY_REPOSITORY_H

#include "Mdt/Expected.h"
#include "Mdt/Entity/AbstractEntityRepository.h"

namespace Mdt{ namespace Railway{

  /*! \brief Base class to create a generic repository
   */
  template<typename EntityData, typename UniqueId>
  class AbstractEntityRepository : public virtual Mdt::Entity::AbstractEntityRepository<EntityData, UniqueId>
  {
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_ABSTRACT_ENTITY_REPOSITORY_H
