#ifndef SCHEMA_CLIENT_H
#define SCHEMA_CLIENT_H

#include "Mdt/Sql/Schema/TableTemplate.h"

namespace Schema{

  class Client : public Mdt::Sql::Schema::TableTemplate<Client>
  {
   public:

    Client();

    Mdt::Sql::Schema::AutoIncrementPrimaryKey Id_PK() const
    {
      return autoIncrementPrimaryKey();
    }

    Mdt::Sql::Schema::Field Name() const
    {
      return field(1);
    }
  };

} // namespace Schema{

#endif // #ifndef SCHEMA_CLIENT_H
