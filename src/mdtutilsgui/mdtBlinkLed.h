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

  /*! \brief Set the LED OFF
   *
   */
  void setOff();

 private:

  QTimer pvTimer;
  int pvBlinkHalfPeriod;
};

#endif  // #ifndef MDT_BLINK_LED_H
