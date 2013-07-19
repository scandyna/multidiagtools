
#include "mdtGraph.h"
#include "mdtGraphVertex.h"

mdtGraph::mdtGraph(bool autoDeleteData) {
}

bool mdtGraph::insertVertex(const mdtGraphVertex & v) {
}

bool mdtGraph::insertVertex(const QVariant & key, const QVariant & data) {
}

bool mdtGraph::insertEdge(const QVariant & keyFrom, const QVariant & keyTo) {
}

bool mdtGraph::removeEdge(const QVariant & keyFrom, const QVariant & keyTo) {
}

mdtGraphVertexData mdtGraph::vertexData(const QVariant & key) {
}

QList<mdtGraphVertexData> mdtGraph::adjacencyList(const QVariant & key) {
}

