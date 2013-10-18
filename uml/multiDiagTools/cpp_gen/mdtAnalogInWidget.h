#ifndef _MDTANALOGINWIDGET_H
#define _MDTANALOGINWIDGET_H


#include "mdtAbstractIoWidget.h"

class mdtAnalogIo;

class mdtAnalogInWidget : public mdtAbstractIoWidget {
  public:
     mdtAnalogInWidget(const QWidget & parent = 0);

     ~mdtAnalogInWidget();

    void setIo(const mdtAnalogIo & io);


  private:
    void setRange(double min, double max,  );

    void setValue(double value);

};
#endif
