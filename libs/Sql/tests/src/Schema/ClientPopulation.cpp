#include "ClientPopulation.h"
#include "Client.h"

namespace Schema{

ClientPopulation::ClientPopulation()
{
  Client client;

  setName("Client data");
  setTable(client);
  addField(client.Id_PK());
  addField(client.Name());
}

void ClientPopulation::addClient(const QString & name)
{
  addRow(QVariant(), name);
}

void ClientPopulation::addClient(qlonglong id, const QString & name)
{
  addRow(id, name);
}

} // namespace Schema{
