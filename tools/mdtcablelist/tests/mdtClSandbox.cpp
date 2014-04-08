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
#include <QSqlDatabase>
///#include "mdtClPathGraphicsConnection.h"
///#include "mdtClPathGraphicsLink.h"
///#include "mdtClPathGraph.h"

#include <QMessageBox>
#include <QInputDialog>

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
}

/**
void mdtClSandbox::graphicsView()
{
  QSqlDatabase db;
  QGraphicsView view;
  bool ok;
  QString password;

  // Setup DB connection
  db = QSqlDatabase::addDatabase("QMYSQL");
  db.setHostName("127.0.0.1");
  db.setDatabaseName("cablelist");
  db.setUserName("scandyna");
  password = QInputDialog::getText(0, "Password for DB connection", \
                                      "Please enter password to connect to DB", \
                                      QLineEdit::Password, "", &ok);
  if(!ok){
    return;
  }
  db.setPassword(password);
  if(!db.open()){
    QMessageBox::warning(0, "DB connection error", "Cannot connect to database");
    return;
  }

  mdtClPathGraph pathGraph(db);

  view.setRenderHint(QPainter::Antialiasing);
  view.setCacheMode(QGraphicsView::CacheBackground);
  view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

  pathGraph.attachView(&view);
  QVERIFY(pathGraph.loadLinkList());
  ///QVERIFY(pathGraph.drawPath(8));
  QVERIFY(pathGraph.drawPath(29));
  qDebug() << pathGraph.lastErrorMessage();
  view.show();

  while(view.isVisible()){
    QTest::qWait(500);
  }
}
*/

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
