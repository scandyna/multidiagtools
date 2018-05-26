/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef SCHEMA_LINK_H
#define SCHEMA_LINK_H

#include "Mdt/Sql/Schema/TableTemplate.h"

namespace Schema{

  class Link : public Mdt::Sql::Schema::TableTemplate<Link>
  {
   public:

    Link();

    Mdt::Sql::Schema::Field StartConnectionId_PK() const
    {
      return field(0);
    }

    Mdt::Sql::Schema::Field EndConnectionId_PK() const
    {
      return field(1);
    }
  };

} // namespace Schema{

#endif // #ifndef SCHEMA_LINK_H
