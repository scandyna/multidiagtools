#ifndef _MDTCLDIRECTLINK_H
#define _MDTCLDIRECTLINK_H


class mdtSqlDatabaseManager;

class mdtClDirectLink : public mdtTtBase
{
  public:
  mdtClDirectLink(const QObject & parent, const QSqlDatabase & db);

  ~mdtClDirectLink();

  QVariant getUnitConnectionIdListPartOfUnit(const QVariant & unitConnectionIdList, const QVariant & unitId, bool & ok);

  QVariant getUnitConnectionIdListPartOfUnitConnector(const QVariant & unitConnectionIdList, const QVariant & unitConnectorId, bool & ok);

  bool addLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId);


  private:
  bool createDirectLinkTable();

  bool removeDirectLinkTable();

};

#endif // #ifndef MDTCLDIRECTLINK_H
