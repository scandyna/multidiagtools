#ifndef _MDTGRAPHVERTEXDATA_H
#define _MDTGRAPHVERTEXDATA_H


class mdtGraphVertexData {
  private:
    QVariant pvKey;

    QMap<QString, QVariant> pvData;


  public:
    mdtGraphVertexData();

    virtual ~mdtGraphVertexData();

    void setKey(const QVariant & key);

    QVariant key();

    void setData(const QVariant & data, const QString & field);

    QVariant data(const QString & field);

};
#endif
