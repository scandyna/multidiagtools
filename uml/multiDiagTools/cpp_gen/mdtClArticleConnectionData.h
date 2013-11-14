#ifndef _MDTCLARTICLECONNECTIONDATA_H
#define _MDTCLARTICLECONNECTIONDATA_H


#include "mdtClConnectionData.h"

class mdtClArticleConnectionData : public mdtClConnectionData
{
  private:
    QVariant pvArticleId;

    QVariant pvArticleConnectorId;


  public:
  void setArticleId(QVariant value);

  inline const QVariant ArticleId() const;

  void setArticleConnectorId(QVariant value);

  inline const QVariant ArticleConnectorId() const;

};
inline const QVariant mdtClArticleConnectionData::ArticleId() const 
{
  return pvArticleId;
}

inline const QVariant mdtClArticleConnectionData::ArticleConnectorId() const 
{
  return pvArticleConnectorId;
}


#endif // #ifndef MDTCLARTICLECONNECTIONDATA_H
