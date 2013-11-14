#ifndef _MDTCLCONNECTIONDATA_H
#define _MDTCLCONNECTIONDATA_H


#include <string>
using namespace std;

class mdtClConnectionData
{
  private:
    QVariant pvConnectorId;

    QVariant pvConnectionId;

    string pvConnectorName;

    string pvContactName;


  public:
  void setConnectorId(QVariant value);

  inline const QVariant ConnectorId() const;

  void setConnectionId(QVariant value);

  inline const QVariant ConnectionId() const;

  void setConnectorName(string value);

  inline const string ConnectorName() const;

  void setContactName(string value);

  inline const string ContactName() const;

};
inline const QVariant mdtClConnectionData::ConnectorId() const 
{
  return pvConnectorId;
}

inline const QVariant mdtClConnectionData::ConnectionId() const 
{
  return pvConnectionId;
}

inline const string mdtClConnectionData::ConnectorName() const 
{
  return pvConnectorName;
}

inline const string mdtClConnectionData::ContactName() const 
{
  return pvContactName;
}


#endif // #ifndef MDTCLCONNECTIONDATA_H
