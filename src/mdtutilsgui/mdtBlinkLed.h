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
#ifndef MDT_BLINK_LED_H
#define MDT_BLINK_LED_H

#include "mdtLed.h"

/*! \brief Blinking LED
 * 
 * This class can be helpfull to get a blinking LED
 * in a simple way.<br>
 * If a lot of blinking LED's are needed, it is recommanded
 * to use mdtLed objects and one QTimer, and to connect
 * the QTimer::timeout() signal to each mdtLed::toggleOnOff() slot.
 */
class mdtBlinkLed : public mdtLed
{
 Q_OBJECT

 public:

  /*! \brief
   * 
   */
  mdtBlinkLed(QWidget *parent = 0);

  /*! \brief Set blinking period
   *
   *  In 1 period, the LED toogles 1x ON ant 1x OFF
   *  (The time whenn the LED is ON is 1/2 period)
   *
   *  \param T The period [ms] (min. 160 [ms])
   */
  void setBlinkPeriod(int T);

  /*! \brief Set LED blinking
   *
   *  Set the LED blinking. The blink period can be changed with setBlinkPeriod()
   *  When setOn() or setOff() is called, the blinking is cancelled.
   *
   *  \param on If true, the LED goes blinking.
   */
  void setBlinking(bool on);

 public slots:

  /*! \brief Set the LED ON
   *
   */
  void setOn();

  /*! \brief Set the LED ON for a given time
   *
   * The LED will set ON and hold this state for specified time.
   * After this time, the LED goes back OFF.
   * \param holdTime ON state hold time [ms]
   * \param priority  If true, cancel allready running shot.
   */
  void setOn(int holdTime, bool priority = false);

  /*! \brief Set the LED OFF
   *
   */
  void setOff();

 private:

  QTimer pvTimer;
  int pvBlinkHalfPeriod;
};

#endif  // #ifndef MDT_BLINK_LED_H
