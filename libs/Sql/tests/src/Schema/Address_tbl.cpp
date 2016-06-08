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
#include "Address_tbl.h"

namespace Schema{

Address_tbl::Address_tbl()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;

  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  /// \todo Client_Id_FK
  
  // Street
  Field Street;
  Street.setName("Street");
  Street.setType(FieldType::Varchar);
  Street.setLength(150);
  // FieldAA
  Field FieldAA;
  FieldAA.setName("FieldAA");
  FieldAA.setType(FieldType::Varchar);
  FieldAA.setLength(50);
  // FieldAB
  Field FieldAB;
  FieldAB.setName("FieldAB");
  FieldAB.setType(FieldType::Varchar);
  FieldAB.setLength(50);
  /// \todo fk_Client_Id_FK
  
  // Setup table
  setTableName("Address_tbl");
  setPrimaryKey(Id_PK);
  
  addField(Street);
  addField(FieldAA);
  addField(FieldAB);

}


} // namespace Schema{