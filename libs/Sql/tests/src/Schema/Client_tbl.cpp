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
#include "Client_tbl.h"

namespace Schema{

Client_tbl::Client_tbl()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;

  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  // FieldA
  Field FieldA;
  FieldA.setName("FieldA");
  FieldA.setType(FieldType::Varchar);
  FieldA.setLength(50);
  // FieldA
  Field FieldB;
  FieldB.setName("FieldB");
  FieldB.setType(FieldType::Varchar);
  FieldB.setLength(50);
  // Setup table
  setTableName("Client_tbl");
  setPrimaryKey(Id_PK);
  addField(Name);
  addField(FieldA);
  addField(FieldB);
}

} // namespace Schema{
