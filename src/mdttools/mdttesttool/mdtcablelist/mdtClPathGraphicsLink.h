/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CL_PATH_GRAPHICS_LINK_H
#define MDT_CL_PATH_GRAPHICS_LINK_H

#include <QGraphicsItem>
#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QString>
#include <QColor>

class mdtClPathGraphicsConnection;
class QPainter;
class QWidget;
class QStyleOptionGraphicsItem;

/*! \brief Item that represent a link in a graphics scene
 */
class mdtClPathGraphicsLink : public QGraphicsItem
{
 public:

  /*! \brief For QGraphicsView
   */
  enum { Type = UserType + 2};

  /*! \brief
   */
  enum linkType_t {
                    CableLink,    /*! \brief Cable link */
                    Connection,   /*! \brief Connection */
                    InternalLink  /*! \brief Internal link */
                  };

  /*! \brief Constructor
   */
  mdtClPathGraphicsLink(mdtClPathGraphicsConnection *startConnection, mdtClPathGraphicsConnection *endConnection);

  /*! \brief Set text
   */
  void setText(const QString &text);

  /*! \brief Set the link type
   *
   * This will affect the graphical representation of the link.
   *
   * \note : current version will only change color of the link ..
   */
  void setLinkType(linkType_t type);

  /*! \brief Get start connection
   */
  mdtClPathGraphicsConnection *startConnection();

  /*! \brief Get end connection
   */
  mdtClPathGraphicsConnection *endConnection();

  /*! \brief Adjust the start and end points of the link
   */
  void adjust();

  /*! \brief Get type
   */
  int type() const;

  /*! \brief Get bounding rect
   */
  QRectF boundingRect() const;

  /*! \brief Paint the link
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

 private:

  /*! \brief Update the bounding rect regarding current text and line sizes
   */
  void updateBoundingRect();

  Q_DISABLE_COPY(mdtClPathGraphicsLink);

  mdtClPathGraphicsConnection *pvStartConnection;
  mdtClPathGraphicsConnection *pvEndConnection;
  QPointF pvStartPoint;
  QPointF pvEndPoint;
  QString pvText;
  QSizeF pvTextSize;
  QRectF pvBoundingRect;
  QColor pvLinkColor;
};

#endif // #ifndef MDT_CL_PATH_GRAPHICS_LINK_H
