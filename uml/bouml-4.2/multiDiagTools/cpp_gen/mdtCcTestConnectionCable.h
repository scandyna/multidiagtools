#ifndef _MDTCCTESTCONNECTIONCABLE_H
#define _MDTCCTESTCONNECTIONCABLE_H


#include "mdtClBase.h"

class mdtCcTestConnectionCable : public mdtClBase
{
  public:
  QString sqlForStartConnectorSelection(const QVariant & dutUnitId);

};

#endif // #ifndef MDTCCTESTCONNECTIONCABLE_H
