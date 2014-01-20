
#include "mdtGraphVertex.h"
#include "mdtGraphEdgeData.h"
#include "mdtGraphVertexAdjacent.h"

mdtGraphVertex::mdtGraphVertex() {
}

mdtGraphVertex::~mdtGraphVertex() {
}

void mdtGraphVertex::setData(const mdtGraphVertexData & data) {
}

mdtGraphVertexData mdtGraphVertex::data() {
}

void mdtGraphVertex::addAdjacent(const mdtGraphVertex & v, const mdtGraphEdgeData & d) {
}

bool mdtGraphVertex::removeAdjacent(const QVariant & key, bool deleteEdgeData) {
}

bool mdtGraphVertex::containsAdjacent(const QVariant & key) {
}

bool mdtGraphVertex::hasAdjacents() {
}

int mdtGraphVertex::adjacentsCount() {
}

