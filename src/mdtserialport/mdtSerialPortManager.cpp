
#include "mdtSerialPortManager.h"
#include "mdtSerialPortConfig.h"

mdtSerialPortManager::mdtSerialPortManager()
{
}

mdtSerialPortManager::~mdtSerialPortManager()
{
}

mdtSerialPortInterface *mdtSerialPortManager::interface(int portNumber)
{
  // Map portNumber to index
  portNumber--;
  // Check index validity
  if((portNumber < 0)||(portNumber >= pvInterfaces.size())){
    return 0;
  }
  return pvInterfaces.at(portNumber);
}
