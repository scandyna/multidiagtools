/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include <QTest>
#include "mdtApplication.h"
#include <QString>
#include <QVariant>
#include <QSet>
#include <QList>
#include "mdtClSandbox.h"

#include <boost/config.hpp>
#include <vector>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>

#include <QHash>
#include <QPair>

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>

#include <QDebug>

/*
 * Vertex class
 */
class Vertex
{
 public:

  Vertex();
  ~Vertex();
  void setData(const QVariant &data);
  QVariant data() const;
  bool operator==(const Vertex &other);
  void addAdjacent(Vertex *v);
  bool removeAdjacent(const QVariant &data);
  bool containsAdjacent(const QVariant &data);
  bool hasAdjacent() const;
  void display();

 private:

  QVariant pvData;
  QList<Vertex*> pvAdjacencyList; // Liste d'adjacence

  Q_DISABLE_COPY(Vertex);
};

/*
 * Vertex class implementation
 */
Vertex::Vertex()
{
}

Vertex::~Vertex()
{
}

void Vertex::setData(const QVariant &data)
{
  pvData = data;
}

QVariant Vertex::data() const
{
  return pvData;
}

bool Vertex::operator==(const Vertex &other)
{
  return (pvData == other.pvData);
}

void Vertex::addAdjacent(Vertex *v)
{
  Q_ASSERT(v != 0);

  pvAdjacencyList.append(v);
}

bool Vertex::removeAdjacent(const QVariant &data)
{
  int i;

  // Recherche de l'index du sommet adjacent conetant data
  for(i = 0; i < pvAdjacencyList.size(); ++i){
    Q_ASSERT(pvAdjacencyList.at(i) != 0);
    if(pvAdjacencyList.at(i)->data() == data){
      pvAdjacencyList.removeAt(i);
      return true;
    }
  }

  return false;
}

bool Vertex::containsAdjacent(const QVariant &data)
{
  int i;

  // Recherche de l'index du sommet adjacent conetant data
  for(i = 0; i < pvAdjacencyList.size(); ++i){
    Q_ASSERT(pvAdjacencyList.at(i) != 0);
    if(pvAdjacencyList.at(i)->data() == data){
      return true;
    }
  }

  return false;

}

bool Vertex::hasAdjacent() const
{
  return (!pvAdjacencyList.isEmpty());
}

void Vertex::display()
{
  int i;
  QString log;

  log = "Vertex: " + pvData.toString();
  for(i = 0; i < pvAdjacencyList.size(); ++i){
    Q_ASSERT(pvAdjacencyList.at(i) != 0);
    log += "->" + pvAdjacencyList.at(i)->data().toString();
  }
  qDebug() << log;
}

/*
 * Graph class
 */
class Graph
{
 public:
  enum VertexColor{White, Gray, Black};
  Graph();
  ~Graph();
  bool insertVertex(const QVariant &data);
  bool removeVertex(const QVariant &data);
  bool insertEdge(const QVariant &dataFrom, const QVariant &dataTo);
  bool removeEdge(const QVariant &dataFrom, const QVariant &dataTo);
  void display();

 private:
  int pvVertexCount;  // Nombre de sommets
  int pvEdgeCount;    // Nombre d'arcs (traduction de edge: fil)
  QList<Vertex*> pvVertexList;  // Liste des sommets
};

/*
 * Graph class implementation
 */
Graph::Graph()
{
  pvVertexCount = 0;
  pvEdgeCount = 0;
}

Graph::~Graph()
{
  qDeleteAll(pvVertexList);
}

bool Graph::insertVertex(const QVariant &data)
{
  int i;
  Vertex *v;

  // On interdit l'insertion de sommets duppliqués
  for(i = 0; i < pvVertexList.size(); ++i){
    Q_ASSERT(pvVertexList.at(i) != 0);
    if(pvVertexList.at(i)->data() == data){
      return false;
    }
  }
  // Insertion du sommet
  v = new Vertex;
  if(v == 0){
    return false;
  }
  v->setData(data);
  pvVertexList.append(v);
  // Ajuste le nombre de sommets pour prendre en compte l'insertion
  pvVertexCount++;

  return true;
}

bool Graph::removeVertex(const QVariant &data)
{
  int i;
  int vToRemoveIndex = -1;
  Vertex *v;

  // Recherche du sommet conetant data
  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    if(v->data() == data){
      // On interdit la suppression de sommet contenant des sommets dans sa liste d'adjacence
      if(v->hasAdjacent()){
        return false;
      }
      // On mémorise l'indexe du sommet à supprimer
      vToRemoveIndex = i;
    }else{
      // On interdit la suppression si le sommet est référencé dans une liste d'adjacence
      if(v->containsAdjacent(data)){
        return false;
      }
    }
  }
  // Si le sommet n'a pas été trouvé, on l'indique
  if(vToRemoveIndex < 0){
    return false;
  }
  // Suppression du sommet
  v = pvVertexList.at(vToRemoveIndex);
  pvVertexList.removeAt(vToRemoveIndex);
  delete v;
  pvVertexCount--;
  return true;
}

bool Graph::insertEdge(const QVariant &dataFrom, const QVariant &dataTo)
{
  int i;
  Vertex *v1, *v2;
  bool found;

  // On interdit l'insertion d'un arc sans ses deux sommets dans le graph
  found = false;
  v1 = 0;
  for(i = 0; i < pvVertexList.size(); ++i){
    v1 = pvVertexList.at(i);
    Q_ASSERT(v1 != 0);
    if(v1->data() == dataFrom){
      found = true;
      break;
    }
  }
  if(!found){
    return false;
  }
  found = false;
  v2 = 0;
  for(i = 0; i < pvVertexList.size(); ++i){
    v2 = pvVertexList.at(i);
    Q_ASSERT(v2 != 0);
    if(v2->data() == dataTo){
      found = true;
      break;
    }
  }
  if(!found){
    return false;
  }
  Q_ASSERT(v1 != 0);
  Q_ASSERT(v2 != 0);
  // Insertion du sommet contenant dataTo (v2) dans le sommet contenant dataFrom (v1)
  v1->addAdjacent(v2);
  pvEdgeCount++;

  return true;
}

bool Graph::removeEdge(const QVariant &dataFrom, const QVariant &dataTo)
{
  int i;
  Vertex *v;

  // On cherche le sommet conetant dataFrom
  for(i = 0; i < pvVertexList.size(); ++i){
    v = pvVertexList.at(i);
    Q_ASSERT(v != 0);
    if(v->data() == dataFrom){
      if(v->removeAdjacent(dataTo)){
        pvEdgeCount--;
        return true;
      }else{
        return false;
      }
    }
  }

  return false;
}

void Graph::display()
{
  int i;

  for(i = 0; i < pvVertexList.size(); ++i){
    Q_ASSERT(pvVertexList.at(i) != 0);
    pvVertexList.at(i)->display();
  }
}


/*
 * mdtClSandbox
 */
void mdtClSandbox::essais()
{
  Graph g;

  QVERIFY(g.insertVertex("1"));
  QVERIFY(g.insertVertex("2"));
  QVERIFY(g.insertVertex("3"));
  QVERIFY(g.insertVertex("4"));
  QVERIFY(g.insertVertex("5"));
  QVERIFY(g.insertVertex("6"));
  
  QVERIFY(g.insertEdge("1", "3"));
  QVERIFY(g.insertEdge("1", "4"));
  QVERIFY(g.insertEdge("2", "1"));
  QVERIFY(g.insertEdge("3", "2"));
  QVERIFY(g.insertEdge("4", "5"));
  QVERIFY(g.insertEdge("5", "6"));
  QVERIFY(g.insertEdge("6", "4"));
  g.display();
  QVERIFY(!g.removeVertex("5"));
  qDebug() << "Removing edge (5, 6)";
  QVERIFY(g.removeEdge("5", "6"));
  QVERIFY(!g.removeVertex("5"));
  qDebug() << "Removing edge (4, 5)";
  QVERIFY(g.removeEdge("4", "5"));
  g.display();
  qDebug() << "Removing vertex 5";
  QVERIFY(g.removeVertex("5"));
  g.display();
}

/**
class VertexProperties
{
 public:
  VertexProperties();
  VertexProperties(const QVariant &data);
  ~VertexProperties();
  QVariant data() const;
 private:
  QVariant pvData;
};

VertexProperties::VertexProperties()
{
  qDebug() << "VertexProperties::VertexProperties() ...";
}
VertexProperties::VertexProperties(const QVariant &data)
{
  qDebug() << "VertexProperties::VertexProperties(" << data << ") ...";
  pvData = data;
}
VertexProperties::~VertexProperties()
{
  qDebug() << "VertexProperties::~VertexProperties() ...";
}
QVariant VertexProperties::data() const
{
  return pvData;
}
*/


struct EdgeProperties
{
};

struct GraphProperties
{
};


  typedef boost::adjacency_list<
      boost::vecS, boost::vecS, boost::directedS,
      boost::property<boost::vertex_bundle_t, QPair<int, QVariant> >,
      boost::property<boost::edge_bundle_t, QVariant>,
      boost::property<boost::graph_bundle_t, GraphProperties>
  > graph_t;

typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
typedef boost::graph_traits<graph_t>::edge_descriptor edge_t;

class MyVisitor : public boost::default_bfs_visitor
{
public:
  MyVisitor(){ path = 0;}
  MyVisitor(QString *outPath, QString *outPathV2, QHash<int, QVariant> *outLst) { path = outPath; pathV2 = outPathV2; pvLst = outLst; }
  void discover_vertex(vertex_t v, const graph_t & g)
  {
    QPair<int, QVariant> data = get(boost::vertex_bundle, g)[v];
    qDebug() << "Vertex, ID: " << data.first << " , data: " << data.second.toString();
    *path += data.second.toString();
    *pathV2 += data.second.toString() + "-";
    pvLst->insert(data.first, data.second);
  }
  void examine_edge(edge_t e, const graph_t &g)
  {
    QVariant data = get(boost::edge_bundle, g)[e];
    qDebug() << "-> Edge, data " << data.toString();
    *path += "(" + data.toString() + ")";
    boost::default_bfs_visitor::examine_edge(e, g);
  }
  QString *path, *pathV2;
  QHash<int, QVariant> *pvLst;
};

void mdtClSandbox::tryBoost()
{


  graph_t g;

  vertex_t v1 = boost::add_vertex(QPair<int, QVariant>(1, "v1"), g);
  vertex_t v2 = boost::add_vertex(QPair<int, QVariant>(2, "v2"), g);
  vertex_t v3 = boost::add_vertex(QPair<int, QVariant>(3, "v3"), g);
  vertex_t v4 = boost::add_vertex(QPair<int, QVariant>(4, "v4"), g);
  vertex_t v5 = boost::add_vertex(QPair<int, QVariant>(5, "v5"), g);
  vertex_t v6 = boost::add_vertex(QPair<int, QVariant>(6, "v6"), g);

  ///VertexProperties const & vp = g[v2];
  ///qDebug() << vp.data();
  qDebug() << "N: " << (int)num_vertices(g);

  /**
  graph_t::edge_descriptor e;
  bool ok;
  tie(e, ok) = boost::add_edge(v1, v2, g);
  */
  ///qDebug() << "ok: " << ok;
  ///boost::add_edge(v3, v4, g);
  ///boost::add_edge(v4, v3, g);

  boost::add_edge(v1, v2, QVariant("v1->v2"), g);
  boost::add_edge(v2, v1, QVariant("v2->v1"), g);
  boost::add_edge(v5, v6, QVariant("v5->v6"), g);
  boost::add_edge(v2, v5, QVariant("v2->v5"), g);
  
  boost::graph_traits<graph_t>::vertex_iterator vi, vi_end;
  boost::graph_traits<graph_t>::adjacency_iterator ai, ai_end;

  /** 
  for(boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi){
    qDebug() << "Vertex : " << get(boost::vertex_bundle, g)[*vi];
    boost::tie(ai, ai_end) = adjacent_vertices(*vi, g);
    while(ai != ai_end){
      qDebug() << "-> Adjacent: " << get(boost::vertex_bundle, g)[*ai];
      ai++;
    }
  }
  */
  
  ///boost::default_bfs_visitor visitor;
  
  
  ///g.clear();
  
  QHash<int, vertex_t> lst;
  QPair<int, QVariant> vertexData;  // <cnnIdPk, "X1;35">
  QVariant edgeData;

  
  QString path, pa2;
  QHash<int, QVariant> lst2;
  MyVisitor visitor(&path, &pa2, &lst2);
  breadth_first_search(g, v1, boost::visitor(visitor));
  qDebug() << "Path  : " << path;
  qDebug() << "PathV2: " << pa2;
  qDebug() << "PathV3: " << lst2;

  
  /**
  std::vector<int> component(num_vertices(g));
  int num = connected_components(g, &component[0]);
  qDebug() << "num: " << num;
  qDebug() << "Connected components:";
  for(int i = 0; i < component.size(); ++i){
    qDebug() << get(boost::vertex_bundle, g)[i] << " is in component  " << get(boost::vertex_bundle, g)[component[i]];
  }
  */
  
  /**
  graph_t g(N);
  add_edge(Jeanie, Debbie, g);
  add_edge(Jeanie, John, g);

  graph_traits< adjacency_list<> >::vertex_iterator vi, v_end;
  graph_traits< adjacency_list<> >::adjacency_iterator ai, a_end;
  property_map <adjacency_list<>, vertex_index_t >::type index_map = get(vertex_index, g);

  for(boost::tie(vi, v_end) = vertices(g); vi != v_end; ++vi){
    qDebug() << name[get(index_map, *vi)];
    boost::tie(ai, a_end) = adjacent_vertices(*vi, g);
    if(ai == a_end){
      qDebug() << "-> No child";
    }else{
      qDebug() << "-> Is parent of:";
    }
    for(; ai != a_end; ++ai){
      qDebug() << "-*> " << name[get(index_map, *ai)];
    }
  }
  */
}

void mdtClSandbox::graphicsView()
{
  QGraphicsScene scene;
  QGraphicsTextItem *textItem;
  
  textItem = scene.addText("8564");
  textItem->setPos(200/2 - textItem->boundingRect().width()/2, -15);
  
  scene.addEllipse(0, 0, 10, 10);
  textItem = scene.addText("W\nXK94A:02\nXK94A:02;3");
  textItem->setPos(0-textItem->boundingRect().width()/2, -30-textItem->boundingRect().height()/2);
  
  scene.addEllipse(200-5, 0, 10, 10);
  textItem = scene.addText("CD\n94\nX1;2d");
  textItem->setPos(200-textItem->boundingRect().width()/2, -30-textItem->boundingRect().height()/2);

  scene.addLine(0+5, 0+5, 200, 0+5);
  
  
  
  QGraphicsView view(&scene);
  view.show();

  while(view.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtClSandbox test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
