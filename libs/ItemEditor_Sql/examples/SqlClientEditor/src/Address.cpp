/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "Address.h"
#include "Client.h"
#include "Mdt/Sql/Schema/ForeignTable.h"

namespace Sql = Mdt::Sql;

using Sql::Schema::FieldType;
using Sql::Schema::Field;

Address::Address()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::ForeignKeyAction;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignKeySettings;

  Client client;

  // Foreign key settings
  ForeignKeySettings fkSettings;
  fkSettings.setIndexed(true);
  fkSettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  fkSettings.setOnUpdateAction(ForeignKeyAction::Cascade);
  // Client ID
  Field Client_Id_FK;
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(FieldType::Integer);
  Client_Id_FK.setRequired(true);
  // Street
  Field Street;
  Street.setName("Street");
  Street.setType(FieldType::Varchar);
  Street.setLength(100);
  // Setup table
  setTableName("Address_tbl");
  setAutoIncrementPrimaryKey("Id_PK");
  addForeignKey(Client_Id_FK, ForeignTable(client), ForeignField(client.Id_PK()), fkSettings);
  addField(Street);
}
