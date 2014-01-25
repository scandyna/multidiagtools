#ifndef _MDTCLLINKDATA_H
#define _MDTCLLINKDATA_H


#include "mdtSqlRecord.h"

class mdtClLinkData : public mdtSqlRecord
{
  public:
  mdtClLinkData();

  mdtClLinkData(const QSqlRecord & record);

  bool setup(const QSqlDatabase & db);

};

#endif // #ifndef MDTCLLINKDATA_H
