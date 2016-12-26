#include "Address.h"
#include "Client.h"
#include "Mdt/Sql/Schema/ForeignTable.h"

namespace Sql = Mdt::Sql;

using Sql::Schema::FieldType;
using Sql::Schema::Field;

namespace Schema{

Address::Address()
{
  using Sql::Schema::ForeignKey;
  using Sql::Schema::ForeignKeyAction;
  using Sql::Schema::ForeignTable;
  using Sql::Schema::ForeignField;
  using Sql::Schema::ForeignKeySettings;

  Client client;

  // Common foreign key settings
  ForeignKeySettings fkSettings;
  fkSettings.setIndexed(true);
  fkSettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  fkSettings.setOnUpdateAction(ForeignKeyAction::Cascade);
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
  // Setup table
  setTableName("Address_tbl");
  setAutoIncrementPrimaryKey("Id_PK");
  addForeignKey(Client_Id_FK, ForeignTable(client), ForeignField(client.Id_PK()), fkSettings);
  addField(Street);
}


} // namespace Schema{