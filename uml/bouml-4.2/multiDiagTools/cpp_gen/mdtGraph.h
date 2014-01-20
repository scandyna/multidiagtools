#ifndef _MDTGRAPH_H
#define _MDTGRAPH_H


#include "mdtGraphVertexData.h"

class mdtGraphVertex;
class mdtGraphEdgeData;

class mdtGraph {
  private:
    <mdtGraphVertex> pvVertexList;

    bool pvAutoDeleteData;

    int pvEdgeCount;


  public:
    mdtGraph(bool autoDeleteData);

    virtual ~mdtGraph();

    bool insertVertex(const mdtGraphVertex & v);

    bool insertVertex(const QVariant & key, const QVariant & data);

    bool insertEdge(const QVariant & keyFrom, const QVariant & keyTo);

    bool insertEdge(const QVariant & keyFrom, const QVariant & keyTo, const mdtGraphEdgeData & data);

    bool insertEdge(const QVariant & keyFrom, const QVariant & keyTo, const QVariant & data, const  & );

    bool removeEdge(const QVariant & keyFrom, const QVariant & keyTo);

    mdtGraphVertexData vertexData(const QVariant & key);

    QList<mdtGraphVertexData> adjacencyList(const QVariant & key);

};
#endif
