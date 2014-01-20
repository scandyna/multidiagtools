#ifndef _MDTCLPATHGRAPHICSLINK_H
#define _MDTCLPATHGRAPHICSLINK_H


#include "mdtClPathGraphicsConnection.h"

class mdtClPathGraphicsLink : public QGraphicsItem
{
  private:
    mdtClPathGraphicsConnection * [2];


  public:
  mdtClPathGraphicsLink(const mdtClPathGraphicsConnection & startConnection, const mdtClPathGraphicsConnection & endConnection);

  mdtClPathGraphicsConnection startConnection();

  mdtClPathGraphicsConnection endConnection();

  void adjust();

  virtual int type() const;

  virtual QRectF boundingRect() const;

  virtual void paint(const QPainter & painter, const QStyleOptionGraphicsItem & option, const QWidget & widget);


  private:
    QPointF pvpvStartPoint;

    QPointF pvpvEndPoint;

};

#endif // #ifndef MDTCLPATHGRAPHICSLINK_H
