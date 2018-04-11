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
#include "TestSchema.h"
#include "Mdt/Sql/Schema/TablePopulation.h"
#include "Client.h"
#include "Client2.h"
#include "Address.h"
#include "Address2.h"
#include "ClientAddressView.h"

namespace Sql = Mdt::Sql;
using Sql::Schema::TablePopulation;

namespace Schema{

TestSchema::TestSchema()
{
  Client client;

  addTable(client);
  addTable(Address());
  addView(ClientAdrressView());
  addTable(Client2());
  addTable(Address2());
  // Add a table population for Client
  TablePopulation tp;
  tp.setName("Client data");
  tp.setTable(client);
  tp.addField(client.Id_PK());
  tp.addField(client.Name());
  addTablePopulation(tp);
}

void TestSchema::addClient(const QString & name)
{
  refTablePopulationAt(ClientTpIndex).addRow(QVariant(), name);
}

void TestSchema::addClient(qlonglong id, const QString & name)
{
  Q_ASSERT(id > 0);

  refTablePopulationAt(ClientTpIndex).addRow(id, name);
}

} // namespace Schema{