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
#include "Client_tbl.h"

namespace Schema{

Address_tbl::Address_tbl()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::ForeignKey;

  Client_tbl client_tbl;

  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Client_Id_FK
  Field Client_Id_FK;
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(FieldType::Integer);
  Client_Id_FK.setRequired(true);
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
  // Fk_Client_Id_FK
  ForeignKey Fk_Client_Id_FK;
  Fk_Client_Id_FK.setParentTable(client_tbl);
  ///Fk_Client_Id_FK.setOnD
  
  // Setup table
  setTableName("Address_tbl");
  setPrimaryKey(Id_PK);
  
  addField(Street);
  addField(FieldAA);
  addField(FieldAB);

}


} // namespace Schema{