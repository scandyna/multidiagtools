#ifndef _MDTGRAPHVERTEXDATA_H
#define _MDTGRAPHVERTEXDATA_H


class mdtGraphVertexData {
  private:
    QVariant pvKey;

    QMap<QVariant, QVariant> pvData;


  public:
    mdtGraphVertexData();

    virtual ~mdtGraphVertexData();

    void setKey(const QVariant & key);

    QVariant key();

    void setData(const QVariant & data, const QVariant & field);

    QVariant data(const QVariant & field);

};
#endif
