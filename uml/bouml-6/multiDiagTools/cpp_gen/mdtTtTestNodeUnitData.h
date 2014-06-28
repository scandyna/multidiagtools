#ifndef _MDTTTTESTNODEUNITDATA_H
#define _MDTTTTESTNODEUNITDATA_H


#include "mdtSqlRecord.h"

class mdtTtTestNodeUnitData : public mdtSqlRecord
{
  public:
  mdtTtTestNodeUnitData(const QSqlDatabase & db);


  private:
    mdtClUnitConnectionData pvTestConnectionData;

    mdtSqlRecord pvUnitData;


  public:
  mdtTtTestNodeUnitData(const QSqlRecord & record);

  bool setup(const QSqlDatabase & db, bool setupUnitData, bool setupConnectionData);

  void setUnitData(const mdtSqlRecord & data);

  mdtSqlRecord unitData();

  void setTestConnectionData(const mdtClUnitConnectionData & data);

  mdtClUnitConnectionData testConnectionData(const  & );

};

#endif // #ifndef MDTTTTESTNODEUNITDATA_H
