/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_LED_H
#define MDT_LED_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QSize>
#include <QTimer>

/*! \brief mdtLED is a widget witch displays a LED
 *
 */
class mdtLed : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Contruct a new mdtLED
   *
   */
  mdtLed(QWidget *parent = 0);

  /*! \brief Set the borders
   *
   *  The LED will be drawn in the center, with horizontal and vertical specified spaces.
   *  \param horizontalBorder Set the horizontal borders (will be the same at left and right of the LED)
   *  \param verticalBorder  Set the vertical borders (will be the same at top and bottom of the LED)
   */
  void setBorders(int horizontalBorder, int verticalBorder);

  /*! \brief Set the LED color to green
   *
   */
  void setGreen();

  /*! \brief Set the LED color to orange
   *
   */
  void setOrange();

  /*! \brief Set the LED color to red
   *
   */
  void setRed();

  /*! \brief Set a simple flat LED
   *
   *  The color will be uniform on the LED surface
   */
  void setFlat();

  /*! \brief Set a glint LED
   *
   *  This will use the Qt::RadialGradientPattern brush style
   *  This can slow down the application with many LED's.
   *  For better performance, use setFlat() .
   */
  void setGlint();

  /*! \brief Set the LED as text
   * 
   * The LED will display a ON/OFF text instead a color
   */
  void setTextMode();

  /*! \brief Enable the antialiasing
   *
   *  \param enabled If true, antialiasing will be enabled (else, diseabled)
   */
  void setAntialiasing(bool enabled);

  /*! \brief Force the LED to have a fixed size
   *
   *  By default, ldtLED uses the Qt default QSizePolicy
   *  Wenn calling this method, the LED will keep a fixed size
   *  \param width The width (min. 10)
   *  \param height The height (min. 10)
   */
  void setFixedSize(int width, int height);

  /*! \brief Use Qt QSizePolicy to resize the LED
   *
   *  Use the Qt default QSizePolicy (see the QLayout and QWidget references for details)
   *  This is the default behaviour if setFixedSize() was never called.
   */
  void setAutoSize();

  QSize sizeHint() const;
  QSize minimumSizeHint() const;

  /*! \brief Get the LED ON state
   */
  bool isOn();

 public slots:

  /*! \brief Set the LED ON
   */
  void setOn();

  /*! \brief Set the LED OFF
   */
  void setOff();

  /*! \brief Set the LED ON or OFF
   *
   *  \param on If true, the LED is set ON , else OFF
   */
  void setOn(bool on);

  /*! \brief Toggle On/Off
   *
   */
  void toggleOnOff();

 protected:

  void paintEvent(QPaintEvent *);

 private:

  QPen pvPen;
  QBrush pvFlatBrush;
  QColor pvPenColor;
  QColor pvOnColor;
  QColor pvOnGlance;
  QColor pvOffColor;
  QColor pvOffGlance;
  QColor pvCurrentColor;
  QColor pvCurrentGlance;
  int pvHorzontalBorder;
  int pv2xHorzontalBorder;
  int pvVerticalBorder;
  int pv2xVerticalBorder;
  QSize pvSizeHint; // The preffered size of the ldtLED widget
  QSize pvFixedSize;
  bool pvGlint;
  bool pvAntialiasing;
  bool pvTextMode;        // The LED will display ON/OFF instead a color
  QString pvText;
  bool pvIsOn;
  int pvBlinkHalfPeriod;
};

#endif  // #ifndef MDT_LED_H
