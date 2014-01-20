#ifndef _MDTANALOGIO_H
#define _MDTANALOGIO_H


#include "mdtAbstractIo.h"

class mdtAnalogIo : public mdtAbstractIo {
  public:
     mdtAnalogIo(const QObject & parent = 0);

     ~mdtAnalogIo();

    void setUnit(const QString & unit);

    QString unit();

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
    void valueChanged(double newValue,  );

    void valueChanged(int address, double newValue);


  private:
    void unitChangedForUi(const QString & unit);

    void rangeChangedForUi(double min, double max);

    void valueChangedForUi(double newValue);

    void setValueFromUi(double value);

    QString pvUnit;

};
#endif
