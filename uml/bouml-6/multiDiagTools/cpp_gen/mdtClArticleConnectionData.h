#ifndef _MDTCLARTICLECONNECTIONDATA_H
#define _MDTCLARTICLECONNECTIONDATA_H


#include "mdtSqlRecord.h"

class mdtClArticleConnectionData : public mdtSqlRecord
{
  public:
  bool setup(const QSqlDatabase & db);

};

#endif // #ifndef MDTCLARTICLECONNECTIONDATA_H
