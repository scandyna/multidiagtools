
#include "mdtPort.h"

mdtPort::mdtPort(QObject *parent)
 : mdtPortSys(parent)
{
  // Set default timeouts
  setReadTimeout(500);
  setWriteTimeout(500);
}

mdtPort::~mdtPort()
{
  this->close();
}
