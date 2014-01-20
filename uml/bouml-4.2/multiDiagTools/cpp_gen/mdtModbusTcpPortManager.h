#ifndef _MDTMODBUSTCPPORTMANAGER_H
#define _MDTMODBUSTCPPORTMANAGER_H


class mdtPortInfo;

class mdtModbusTcpPortManager : public mdtPortManager {
  public:
    mdtModbusTcpPortManager(const QObject & parent);

    ~mdtModbusTcpPortManager();

    virtual QList<mdtPortInfo*> scan(const QStringList & hosts);

    virtual bool writeData(const QByteArray & data);

    virtual void fromThreadNewFrameReaden();

};
#endif
