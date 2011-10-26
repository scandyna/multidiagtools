#ifndef MDT_ABSTRACT_SERIAL_PORT_H
#define MDT_ABSTRACT_SERIAL_PORT_H

#include "mdtSerialPortConfig.h"

class mdtAbstractSerialPort
{
 public:

  mdtAbstractSerialPort();
  ~mdtAbstractSerialPort();

  virtual bool setConfig(mdtSerialPortConfig &cfg) = 0;

};

#endif  // #ifndef MDT_ABSTRACT_SERIAL_PORT_H
