#ifndef _MDTANALOGIO_H
#define _MDTANALOGIO_H


#include "mdtAbstractIo.h"

class mdtAnalogIo : public mdtAbstractIo {
  public:
     mdtAnalogIo(const QObject & parent = 0);

     ~mdtAnalogIo();

    void setRange(double min, double max, int steps);

    double minimum();

    double maximum();

    void setValue(double value);

    double value();

    void setValueInt(int value);

    int valueInt();


  private:
    double pvValue;

    double pvMinimum;

    double pvMaximum;

    double pvStep;

    double pvStepInverse;


  public:
    void valueChanged(double newValue);


  private:
    void rangeChangedForUi(double min, double max);

    void valueChangedForUi(double newValue);

    void setValueFromUi(double value);

};
#endif
