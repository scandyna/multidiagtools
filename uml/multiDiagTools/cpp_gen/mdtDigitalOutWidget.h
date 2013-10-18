#ifndef _MDTDIGITALOUTWIDGET_H
#define _MDTDIGITALOUTWIDGET_H


#include "mdtAbstractIoWidget.h"
#include "mdtDigitalIo.h"

class mdtDigitalOutWidget : public mdtAbstractIoWidget {
  public:
     mdtDigitalOutWidget(const QWidget & parent = 0);

     ~mdtDigitalOutWidget();

    void setIo(const mdtDigitalIo & io);


  private:
    void setOn(bool on);

};
#endif
