#ifndef _MDTANALOGINWIDGET_H
#define _MDTANALOGINWIDGET_H


#include "mdtAbstractIoWidget.h"

class mdtAnalogInWidget : public mdtAbstractIoWidget {
  public:
     mdtAnalogInWidget(const QWidget & parent = 0);

     ~mdtAnalogInWidget();

    void setRange(double min, double max, int steps);

    void setValue(double value);

    double value();

    void setValueInt(int value);

    int valueInt();


  private:
    double pvStep;

    double pvStepInverse;

};
#endif
