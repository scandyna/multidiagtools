#ifndef _MDTCCTESTCONNECTIONCABLEEDITOR_H
#define _MDTCCTESTCONNECTIONCABLEEDITOR_H


class mdtCcTestConnectionCable;

class mdtCcTestConnectionCableEditor : public QObject
{
  private:
    QSqlDatabase pvpvDatabase;


  public:
  mdtCcTestConnectionCableEditor(const QObject & parent, const QSqlDatabase & db);

  void createCable(const QVariant & testNodeUnitId, const QVariant & dutUnitId);


  private:
  QVariant selectStartConnectorId(const QVariant & dutUnitId);

  QVariant selectEndConnectorIdList(const QVariant & unitConnectorIdList);

};

#endif // #ifndef MDTCCTESTCONNECTIONCABLEEDITOR_H
