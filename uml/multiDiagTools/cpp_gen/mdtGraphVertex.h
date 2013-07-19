#ifndef _MDTGRAPHVERTEX_H
#define _MDTGRAPHVERTEX_H


#include "mdtGraphVertexData.h"

class mdtGraphVertex {
  private:
    <mdtGraphVertex> pvAdjacencyList;

    mdtGraphVertexData * pvData;


  public:
    mdtGraphVertex();

    virtual ~mdtGraphVertex();

    void setData(const mdtGraphVertexData & data);

    mdtGraphVertexData data();

    void addAdjacent(const mdtGraphVertex & v);

    bool removeAdjacent(const QVariant & key);

    bool containsAdjacent(const QVariant & key);

    bool hasAdjacents();

    int adjacentsCount();


  private:
    color_t pvColor;

};
#endif
