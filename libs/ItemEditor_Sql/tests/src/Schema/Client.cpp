#include "Client.h"

namespace Sql = Mdt::Sql;

using Sql::Schema::FieldType;
using Sql::Schema::Field;

namespace Schema{

Client::Client()
{
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(100);
  // Setup table
  setTableName("Client_tbl");
  setAutoIncrementPrimaryKey("Id_PK");
  addField(Name);
}


} // namespace Schema{
