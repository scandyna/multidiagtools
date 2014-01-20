#ifndef _MDTGRAPHVERTEXADJACENT_H
#define _MDTGRAPHVERTEXADJACENT_H


#include "mdtGraphVertex.h"
#include "mdtGraphEdgeData.h"

class mdtGraphVertexAdjacent {
  private:
    mdtGraphVertex * pvVertex;


  public:
    mdtGraphVertexAdjacent(const mdtGraphVertex & vertex, const mdtGraphEdgeData & data);

    ~mdtGraphVertexAdjacent();


  private:
    mdtGraphEdgeData * pvData;


  public:
    mdtGraphVertex vertex();

    mdtGraphEdgeData edgeData();

};
#endif
