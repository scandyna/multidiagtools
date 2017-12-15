#ifndef SCHEMA_ADDRESS_2_H
#define SCHEMA_ADDRESS_2_H

#include "Mdt/Sql/Schema/TableTemplate.h"

namespace Schema{

  class Address2 : public Mdt::Sql::Schema::TableTemplate<Address2>
  {
   public:

    Address2();

    Mdt::Sql::Schema::AutoIncrementPrimaryKey Id_PK() const
    {
      return autoIncrementPrimaryKey();
    }

    Mdt::Sql::Schema::Field Client_Id_FK() const
    {
      return field(1);
    }

    Mdt::Sql::Schema::Field Street() const
    {
      return field(2);
    }
  };

} // namespace Schema{

#endif // #ifndef SCHEMA_ADDRESS_2_H
