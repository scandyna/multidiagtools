#ifndef SCHEMA_CLIENT_POPULATION_H
#define SCHEMA_CLIENT_POPULATION_H

#include "Mdt/Sql/Schema/TablePopulationTemplate.h"
#include <QString>

namespace Schema{

  class ClientPopulation : public Mdt::Sql::Schema::TablePopulationTemplate<ClientPopulation>
  {
   public:

    ClientPopulation();

    void addClient(const QString & name);
    void addClient(qlonglong id, const QString & name);
  };

} // namespace Schema{

#endif // #ifndef SCHEMA_CLIENT_POPULATION_H
