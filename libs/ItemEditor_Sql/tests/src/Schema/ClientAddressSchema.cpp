#include "ClientAddressSchema.h"
#include "Client.h"
#include "Address.h"
#include "ClientPopulation.h"
#include "Mdt/Sql/Schema/TablePopulation.h"

namespace Sql = Mdt::Sql;
using Sql::Schema::TablePopulation;

namespace Schema{

ClientAddressSchema::ClientAddressSchema()
{
  Client client;
  Address address;

  addTable(client);
  addTablePopulation(ClientPopulation());
  addTable(address);
  // Table population for address
  TablePopulation addressTp;
  addressTp.setName("Address data");
  addressTp.setTable(address);
  addressTp.addField(address.Id_PK());
  addressTp.addField(address.Client_Id_FK());
  addressTp.addField(address.Street());
}

void ClientAddressSchema::addClient(qlonglong id, const QString& name)
{
  refTablePopulationAt(ClientTpIndex).addRow(id, name);
}

void ClientAddressSchema::addAddress(qlonglong id, qlonglong clientId, const QString& street)
{
  refTablePopulationAt(AddressTpIndex).addRow(id, clientId, street);
}

} // namespace Schema{
