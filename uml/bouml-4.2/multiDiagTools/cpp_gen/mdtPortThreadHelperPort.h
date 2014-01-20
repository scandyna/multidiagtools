#ifndef _MDTPORTTHREADHELPERPORT_H
#define _MDTPORTTHREADHELPERPORT_H


#include "mdtPortThreadHelper.h"

class mdtPortThreadHelperPort : public mdtPortThreadHelper {
  public:
    virtual mdtAbstractPort::error_t reconnect(bool notify);

    int readFromPort(bool emitNewFrameReaden,  );

    int writeDataToPort(int maxSize, const  & );

    mdtAbstractPort::error_t writeToPort(bool bytePerByteWrite, int maxWriteTry, int interByteTime,  );

};
#endif
