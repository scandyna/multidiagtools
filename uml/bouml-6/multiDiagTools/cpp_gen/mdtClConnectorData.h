#ifndef _MDTCLCONNECTORDATA_H
#define _MDTCLCONNECTORDATA_H


#include "mdtSqlRecord.h"

class mdtClConnectorData : public mdtSqlRecord
{
  public:
  bool setup(const QSqlDatabase & db);


  private:
    QList<mdtSqlRecord> pvContactDataList;


  public:
  void setContactDataList(const mdtSqlRecord & dataList);

  mdtSqlRecord contactDataList();

  void addContactData(const mdtSqlRecord & data);

  bool setContactData(const QVariant & contactId, const mdtSqlRecord & data);

  mdtSqlRecord contactData(const QVariant & contactId, bool ok);

};

#endif // #ifndef MDTCLCONNECTORDATA_H
