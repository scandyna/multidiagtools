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
#include "ClientAddressView.h"

#include "Client_tbl.h"
#include "Address_tbl.h"
#include "Mdt/Sql/Schema/JoinHelper.h"

namespace Schema{

ClientAdrressView::ClientAdrressView()
{
  using Mdt::Sql::Schema::View;
  using Mdt::Sql::Schema::ViewTable;
  using Mdt::Sql::Schema::JoinClause;
  using Mdt::Sql::Schema::JoinOperator;
  using Mdt::Sql::Schema::JoinHelper;
  using Mdt::Sql::Schema::MainTableField;
  using Mdt::Sql::Schema::TableToJoinField;

  // Create instance of our defined entities
  Client_tbl client;
  Address_tbl address;

  // Define tables with their aliases
  ViewTable CLI(client, "CLI");
  ViewTable ADR(address, "ADR");

  // Setup the view itself
  setName("ClientAddress_view");
  setTable(CLI);
  addSelectAllFields(CLI);
  addSelectField(ADR, address.Id_PK(), "Address_Id");
  addSelectField(ADR, address.Street());
  addJoinClause(JoinHelper::joinClauseFromTables(client, CLI, address, ADR, JoinOperator::Join));
  /*
   * Or, join clause can also be expressed manually
   */
//   JoinClause join;
//   join.setMainTable(CLI);
//   join.setTableToJoin(ADR);
//   join.addKey( MainTableField(client.Id_PK()), TableToJoinField(address.Client_Id_FK()) );
//   addJoinClause(join);
}

} // namespace Schema{
