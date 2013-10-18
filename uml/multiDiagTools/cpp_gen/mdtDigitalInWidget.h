#ifndef _MDTDIGITALINWIDGET_H
#define _MDTDIGITALINWIDGET_H


#include "mdtAbstractIoWidget.h"
#include "mdtDigitalIo.h"

class mdtDigitalInWidget : public mdtAbstractIoWidget {
  public:
     mdtDigitalInWidget(const QWidget & parent = 0);

     ~mdtDigitalInWidget();

    void setIo(const mdtDigitalIo & io);


  private:
    void setOn(bool on);

};
#endif
