#ifndef _MDTCLPATHGRAPHICSCONNECTION_H
#define _MDTCLPATHGRAPHICSCONNECTION_H


class mdtClPathGraphicsConnection : public QGraphicsItem
{
  private:
    <mdtClPathGraphicsLink *> pvLinkList;


  public:
  mdtClPathGraphicsConnection();

  void addLink(const mdtClPathGraphicsLink & link);

  mdtClPathGraphicsLink links();

  virtual int type() const;

  virtual QRectF boundingRect() const;

  virtual QPainterPath shape() const;

  virtual void paint(const QPainter & painter, const QStyleOptionGraphicsItem & option, const QWidget & widget);

};

#endif // #ifndef MDTCLPATHGRAPHICSCONNECTION_H
