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
#ifndef MDT_ENTITY_SQL_SELECT_STATEMENT_H
#define MDT_ENTITY_SQL_SELECT_STATEMENT_H

#include "Mdt/Entity/TypeTraits/IsEntity.h"
#include "MdtEntity_SqlExport.h"
#include <QString>
#include <QLatin1String>

namespace Mdt{ namespace Entity{

  /*! \brief Helper class to create SQL SELECT statements from a entity
   */
  class MDT_ENTITY_SQL_EXPORT SqlSelectStatement
  {
   public:

    /*! \brief Create a statement that selects
     */
    template<typename Entity>
    static QString selectAllFromEntity()
    {
      static_assert( TypeTraits::IsEntity<Entity>::value, "Entity must be a entity type" );
      using entity_def = typename Entity::def_type;

      return QLatin1String("SELECT * FROM ") + entity_def::entityName();
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_SELECT_STATEMENT_H
