#ifndef _MDTDIGITALOUTWIDGET_H
#define _MDTDIGITALOUTWIDGET_H


#include "mdtAbstractIoWidget.h"

class mdtDigitalOutWidget : public mdtAbstractIoWidget {
  public:
     mdtDigitalOutWidget(const QWidget & parent = 0);

     ~mdtDigitalOutWidget();

    void setOn(bool on);

    bool isOn();

    void stateChanged(bool on);

};
#endif
