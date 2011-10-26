#ifndef _MDTABSTRACTSERIALPORT_H
#define _MDTABSTRACTSERIALPORT_H


class mdtAbstractSerialPort {
  public:
    mdtAbstractSerialPort();

    ~mdtAbstractSerialPort();

    bool applySetup(const mdtSerialPortConfig & config);

};
#endif
