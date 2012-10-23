#ifndef _MDTDIGITALINWIDGET_H
#define _MDTDIGITALINWIDGET_H


#include "mdtAbstractIoWidget.h"

class mdtDigitalInWidget : public mdtAbstractIoWidget {
  public:
     mdtDigitalInWidget(const QWidget & parent = 0);

     ~mdtDigitalInWidget();

    void setOn(bool on);

    bool isOn();

};
#endif
