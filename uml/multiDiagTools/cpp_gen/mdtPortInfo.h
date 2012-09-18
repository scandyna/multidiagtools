#ifndef _MDTPORTINFO_H
#define _MDTPORTINFO_H


class mdtDeviceInfo;

class mdtPortInfo {
  public:
    mdtPortInfo();

    ~mdtPortInfo();


  private:
    <mdtDeviceInfo> pvDeviceInfos;

    QString pvPortName;


  public:
    void setPortName(const QString & name);

    QString portName();

};
#endif
