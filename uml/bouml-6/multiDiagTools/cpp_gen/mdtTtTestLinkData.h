#ifndef _MDTTTTESTLINKDATA_H
#define _MDTTTTESTLINKDATA_H


#include "mdtSqlRecord.h"

class mdtTtTestLinkData : public mdtSqlRecord
{
  public:
  mdtTtTestLinkData();

  mdtTtTestLinkData(const QSqlRecord & record);

  bool setup(const QSqlDatabase & db);

};

#endif // #ifndef MDTTTTESTLINKDATA_H
