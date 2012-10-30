#ifndef _MDTDIGITALIO_H
#define _MDTDIGITALIO_H


#include "mdtAbstractIo.h"

class mdtDigitalIo : public mdtAbstractIo {
  public:
     mdtDigitalIo(const QObject & parent = 0);

     ~mdtDigitalIo();

    void setOn(bool on);

    bool isOn();


  private:
    bool pvIsOn;


  public:
    void stateChanged(bool on);


  private:
    void setStateFromUi(bool on);

    void valueChangedForUi(bool on);

};
#endif
