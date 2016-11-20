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

namespace Sql = Mdt::Sql;

using Sql::Schema::FieldType;
using Sql::Schema::Field;

namespace Schema{

Address_tbl::Address_tbl()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::ForeignKeyAction;
  using Sql::Schema::ParentTableFieldName;
  using Sql::Schema::ChildTableFieldName;

  Client_tbl client_tbl;

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
  Fk_Client_Id_FK.setForeignTable(client_tbl);
  Fk_Client_Id_FK.setOnDeleteAction(ForeignKeyAction::Restrict);
  Fk_Client_Id_FK.setOnUpdateAction(ForeignKeyAction::Cascade);
  Fk_Client_Id_FK.setIndexed(true);
  Fk_Client_Id_FK.addKeyFields(ParentTableFieldName(client_tbl.Id_PK()), ChildTableFieldName(Client_Id_FK));
  // Setup table
  setTableName("Address_tbl");
  setAutoIncrementPrimaryKey("Id_PK");
  addField(Client_Id_FK);
  addField(Street);
  addField(FieldAA);
  addField(FieldAB);
  addForeignKey(Fk_Client_Id_FK);
}

Address2_tbl::Address2_tbl()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::ForeignKeyAction;
  using Sql::Schema::ParentTableFieldName;
  using Sql::Schema::ChildTableFieldName;

  Client2_tbl client2_tbl;

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
  Fk_Client_Id_FK.setParentTable(client2_tbl);
  Fk_Client_Id_FK.setOnDeleteAction(ForeignKeyAction::Restrict);
  Fk_Client_Id_FK.setOnUpdateAction(ForeignKeyAction::Cascade);
  Fk_Client_Id_FK.setIndexed(true);
  Fk_Client_Id_FK.addKeyFields(ParentTableFieldName(client2_tbl.Id_PK()), ChildTableFieldName(Client_Id_FK));
  // Setup table
  setTableName("Address2_tbl");
  setAutoIncrementPrimaryKey("Id_PK");
  addField(Client_Id_FK);
  addField(Street);
  addField(FieldAA);
  addField(FieldAB);
  addForeignKey(Fk_Client_Id_FK);
}

} // namespace Schema{
