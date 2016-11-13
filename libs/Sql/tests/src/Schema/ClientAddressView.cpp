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

namespace Schema{

namespace Sql = Mdt::Sql;

ClientAdrressView::ClientAdrressView()
{
  using Sql::Schema::View;
  using Sql::SelectTable;

  // Create instance of our defined entities
  Client_tbl client;
  Address_tbl address;

  // Define tables with their aliases
  SelectTable CLI(client, "CLI");
  SelectTable ADR(address, "ADR");

  // Setup the view itself
  setName("ClientAddress_view");
  addAllFields(CLI);
  addField(ADR, address.Id_PK(), "Address_Id");
  addField(ADR, address.Street());
  setFromTable(CLI);
  joinTable(ADR);
}

} // namespace Schema{
