#ifndef _MDTUSBTMCPORTMANAGER_H
#define _MDTUSBTMCPORTMANAGER_H


#include "mdtUsbtmcFile.h"

class mdtUsbtmcPortManager : public mdtPortManager {
  public:
    mdtUsbtmcPortManager(const QObject & parent);


  private:
    mdtUsbtmcPort pvPort;


  public:
    QStringList scan();

};
#endif
