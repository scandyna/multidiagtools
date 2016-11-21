#ifndef SCHEMA_CLIENT_2_H
#define SCHEMA_CLIENT_2_H

#include "Mdt/Sql/Schema/TableTemplate.h"

namespace Schema{

  class Client2 : public Mdt::Sql::Schema::TableTemplate<Client2>
  {
   public:

    Client2();

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

#endif // #ifndef SCHEMA_CLIENT_2_H
