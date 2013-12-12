#ifndef _MDTTTTESTNODE_H
#define _MDTTTTESTNODE_H


#include "mdtClBase.h"

class mdtTtTestNode : public mdtClBase
{
  public:
  mdtTtTestNode(const QSqlDatabase & db);

  QString sqlForUnitSelection(const QVariant & vehicleTypeId, const QVariant & TestNodeId);

};

#endif // #ifndef MDTTTTESTNODE_H
