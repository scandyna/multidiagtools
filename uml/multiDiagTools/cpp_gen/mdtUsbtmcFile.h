#ifndef _MDTUSBTMCFILE_H
#define _MDTUSBTMCFILE_H


#include "mdtDeviceFile.h"

class mdtUsbtmcFile : public mdtDeviceFile {
  public:
    mdtUsbtmcFile();

    ~mdtUsbtmcFile();

    bool waitEventRead();

    virtual bool waitEventWriteReady();

};
#endif
