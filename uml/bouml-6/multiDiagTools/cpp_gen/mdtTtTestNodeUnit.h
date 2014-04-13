#ifndef _MDTTTTESTNODEUNIT_H
#define _MDTTTTESTNODEUNIT_H


#include "mdtTtTestNodeUnitData.h"

class mdtTtTestNodeUnit : public mdtTtBase
{
  public:
  mdtTtTestNodeUnit(const QObject & parent, const QSqlDatabase & db);

  mdtTtTestNodeUnitData getData(const QVariant & nodeUnitId, bool & ok);

  bool add(const mdtTtTestNodeUnitData & data);

  bool edit(const QVariant & nodeUnitId, const mdtTtTestNodeUnitData & data);

  bool remove(const QVariant & nodeUnitId);

};

#endif // #ifndef MDTTTTESTNODEUNIT_H
