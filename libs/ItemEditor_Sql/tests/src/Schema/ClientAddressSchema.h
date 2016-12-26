#ifndef SCHEMA_CLIENT_ADDRESS_SCHEMA_H
#define SCHEMA_CLIENT_ADDRESS_SCHEMA_H

#include "Mdt/Sql/Schema/SchemaTemplate.h"
#include <QString>

namespace Schema{

  class ClientAddressSchema : public Mdt::Sql::Schema::SchemaTemplate<ClientAddressSchema>
  {
   public:

    ClientAddressSchema();

    void addClient(qlonglong id, const QString & name);
    void addAddress(qlonglong id, qlonglong clientId, const QString & street);

   private:

    enum TablePopulationIndex
    {
      ClientTpIndex = 0,
      AddressTpIndex
    };
  };

} // namespace Schema{

#endif // #ifndef SCHEMA_CLIENT_ADDRESS_SCHEMA_H
