#ifndef _MDTANALOGOUTWIDGET_H
#define _MDTANALOGOUTWIDGET_H


#include "mdtAbstractIoWidget.h"

class mdtAnalogIo;

class mdtAnalogOutWidget : public mdtAbstractIoWidget {
  public:
     mdtAnalogOutWidget(const QWidget & parent = 0);

     ~mdtAnalogOutWidget();

    void setIo(const mdtAnalogIo & io);


  private:
    void setRange(double min, double max,  );

    void setValue(double value);

};
#endif
