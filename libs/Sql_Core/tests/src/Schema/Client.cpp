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
  // FieldA
  Field FieldA;
  FieldA.setName("FieldA");
  FieldA.setType(FieldType::Varchar);
  FieldA.setLength(50);
  // FieldB
  Field FieldB;
  FieldB.setName("FieldB");
  FieldB.setType(FieldType::Varchar);
  FieldB.setLength(50);
  // Setup table
  setTableName("Client_tbl");
  setAutoIncrementPrimaryKey("Id_PK");
  addField(Name);
  addField(FieldA);
  addField(FieldB);
}


} // namespace Schema{
