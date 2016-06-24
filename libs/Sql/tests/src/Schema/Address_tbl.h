/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef SCHEMA_ADDRESS_TBL_H
#define SCHEMA_ADDRESS_TBL_H

#include "Mdt/Sql/Schema/TableTemplate.h"

namespace Schema{

  class Address_tbl : public Mdt::Sql::Schema::TableTemplate<Address_tbl>
  {
   public:

    Address_tbl();

    Mdt::Sql::Schema::AutoIncrementPrimaryKey Id_PK() const
    {
      return autoIncrementPrimaryKey();
    }

    Mdt::Sql::Schema::Field Client_Id_FK() const
    {
      return field(1);
    }

    Mdt::Sql::Schema::Field Street() const
    {
      return field(2);
    }
  };

  class Address2_tbl : public Mdt::Sql::Schema::TableTemplate<Address_tbl>
  {
   public:

    Address2_tbl();

    Mdt::Sql::Schema::AutoIncrementPrimaryKey Id_PK() const
    {
      return autoIncrementPrimaryKey();
    }

    Mdt::Sql::Schema::Field Client_Id_FK() const
    {
      return field(1);
    }

    Mdt::Sql::Schema::Field Street() const
    {
      return field(2);
    }
  };

} // namespace Schema{

#endif // #ifndef SCHEMA_ADDRESS_TBL_H
