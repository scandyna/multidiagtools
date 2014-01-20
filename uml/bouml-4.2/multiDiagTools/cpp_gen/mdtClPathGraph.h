#ifndef _MDTCLPATHGRAPH_H
#define _MDTCLPATHGRAPH_H


#include "mdtClUnitConnectionData.h"
#include "mdtClLinkData.h"

class mdtClPathGraphicsConnection;

//Handle also the (Boost) graph
class mdtClPathGraph
{
  private:
    QSqlQueryModel pvLinkListModel;

    QSqlDatabase pvDatabase;

    QQueue<QPair<QVariant, QVariant>> pvEdgeQueue;

    QList<mdtClPathGraphicsConnection *> pvDrawnConnections;


  public:
    mdtClPathGraph(QSqlDatabase db);

    ~mdtClPathGraph();

  bool loadLinkList(const  & );

    bool drawPath(const QVariant & fromConnectionId);

  QVariant getLinkedConnectionIds(const QVariant & fromConnectionId);


  private:
    QGraphicsScene pvGraphicsScene;


  public:
  void attachView(const QGraphicsView & view);


  private:
  mdtClUnitConnectionData getUnitConnectionStartDataFromModel(int row);

  mdtClUnitConnectionData getUnitConnectionEndDataFromModel(int row);

  mdtClLinkData getLinkDataFromModel(int row);

};

#endif // #ifndef MDTCLPATHGRAPH_H
