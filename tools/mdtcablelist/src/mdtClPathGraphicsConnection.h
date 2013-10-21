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
#ifndef MDT_CL_PATH_GRAPHICS_CONNECTION_H
#define MDT_CL_PATH_GRAPHICS_CONNECTION_H

#include <QGraphicsItem>
#include <QList>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QVariant>
#include <QString>

class mdtClPathGraphicsLink;
class QPainter;
class QWidget;
class QStyleOptionGraphicsItem;

/*! \brief Item that represent a connection (circle and text) in a graphics scene
 */
class mdtClPathGraphicsConnection : public QGraphicsItem
{
 public:

  enum { Type = UserType + 1};

  /*! \brief Constructor
   */
  mdtClPathGraphicsConnection();

  /*! \brief Destructor
   */
  ~mdtClPathGraphicsConnection();

  /*! \brief Set text
   */
  void setText(const QString &text);

  /*! \brief Set the circle diameter
   */
  void setCircleDiameter(qreal diameter);

  /*! \brief Get the position for next connection (node)
   *
   * This is a helper method to place connection on a scene.
   *  This will not alter current connection's position.
   *
   * \param reverse If true, next position will be searched from right to left
   * \param linkLength The expected length of link
   * \param dAlpha Angle increment (default is approx pi/4, 45°)
   */
  QPointF nextPos(bool reverse = false, qreal linkLength = 200.0, qreal dAlpha = 0.785398163397) const;

  /*! \brief Add a link
   */
  void addLink(mdtClPathGraphicsLink *link);

  /*! \brief Get related links
   */
  QList<mdtClPathGraphicsLink*> links() const;

  /*! \brief Get type
   */
  int type() const;

  /*! \brief Get bounding rect
   */
  QRectF boundingRect() const;

  /*! \brief Get shape
   */
  QPainterPath shape() const;

  /*! \brief Paint the item
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

 protected:

  /*! \brief Change handler - Will update links
   */
  QVariant itemChange(GraphicsItemChange change, const QVariant &value);

 private:

  /*! \brief Update the bounding rect regarding current text and circle sizes
   */
  void updateBoundingRect();

  Q_DISABLE_COPY(mdtClPathGraphicsConnection);

  QList<mdtClPathGraphicsLink *> pvLinkList;
  QString pvText;
  QRectF pvCircleRect;
  QRectF pvTextRect;
  QRectF pvBoundingRect;
};

#endif // #ifndef MDT_CL_PATH_GRAPHICS_CONNECTION_H
