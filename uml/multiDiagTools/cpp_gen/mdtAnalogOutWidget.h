#ifndef _MDTANALOGOUTWIDGET_H
#define _MDTANALOGOUTWIDGET_H


#include "mdtAbstractIoWidget.h"

class mdtAnalogOutWidget : public mdtAbstractIoWidget {
  public:
     mdtAnalogOutWidget(const QWidget & parent = 0);

     ~mdtAnalogOutWidget();

    void setRange(double min, double max, int steps);

    void setValue(double value);

    double value();

    void setValueInt(int value);

    int valueInt();

    void valueChanged(double value);


  private:
    double pvStep;

    double pvStepInverse;

};
#endif
