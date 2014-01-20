#ifndef _MDTGRAPHVERTEX_H
#define _MDTGRAPHVERTEX_H


#include "mdtGraphVertexData.h"

class mdtGraphEdgeData;
class mdtGraphVertexAdjacent;

class mdtGraphVertex {
  private:
    mdtGraphVertexData * pvData;


  public:
    mdtGraphVertex();

    virtual ~mdtGraphVertex();

    void setData(const mdtGraphVertexData & data);

    mdtGraphVertexData data();

    void addAdjacent(const mdtGraphVertex & v, const mdtGraphEdgeData & d);

    bool removeAdjacent(const QVariant & key, bool deleteEdgeData);

    bool containsAdjacent(const QVariant & key);

    bool hasAdjacents();

    int adjacentsCount();


  private:
    color_t pvColor;

    <mdtGraphVertexAdjacent> pvAdjacencyList;

};
#endif
