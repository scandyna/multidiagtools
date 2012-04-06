
#include "mdtBlinkLed.h"

mdtBlinkLed::mdtBlinkLed(QWidget *parent)
 : mdtLed(parent)
{
  setBlinkPeriod(1000);
  connect(&pvTimer, SIGNAL(timeout()), this, SLOT(toggleOnOff()));
}

void mdtBlinkLed::setBlinkPeriod(int T)
{
  if(T <= 160){
    T = 160;
  }
  pvBlinkHalfPeriod = T/2;
}

void mdtBlinkLed::setBlinking(bool on)
{
  if(on){
    pvTimer.start(pvBlinkHalfPeriod);
  }else{
    pvTimer.stop();
  }
}

void mdtBlinkLed::setOn()
{
  setBlinking(false);
  mdtLed::setOn();
}

void mdtBlinkLed::setOff()
{
  setBlinking(false);
  mdtLed::setOff();
}
