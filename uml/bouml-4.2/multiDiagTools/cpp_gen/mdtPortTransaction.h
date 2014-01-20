#ifndef _MDTPORTTRANSACTION_H
#define _MDTPORTTRANSACTION_H


#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"

class mdtPortTransaction {
  private:
    int pvId;

    mdtAnalogIo * pvAnalogIo;

    mdtDigitalIo * pvDigitalIo;

    int pvType;

    bool pvForMultipleIos;

    bool pvIsInput;


  public:
    mdtPortTransaction();

    ~mdtPortTransaction();

    void setId(int id);

    int id();

    void setType(int type);

    int type();

    void setIo(const mdtAnalogIo & io, bool isInput);

    void setIo(const mdtDigitalIo & io, bool isInput);

    mdtAnalogIo analogIo();

    mdtDigitalIo digitalIo();

    void setForMultipleIos(bool forMultipleIos);

    bool forMultipleIos();

    bool setIsInput();

    bool isInput();

    bool isOutput();

};
#endif
