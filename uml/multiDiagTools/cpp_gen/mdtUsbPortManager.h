#ifndef _MDTUSBPORTMANAGER_H
#define _MDTUSBPORTMANAGER_H


class mdtUsbPortManager : public mdtPortManager {
  public:
    mdtUsbPortManager(const QObject & parent);

    virtual ~mdtUsbPortManager();

    virtual QList<mdtPortInfo*> scan();

    virtual QList<mdtPortInfo*> scan(int bDeviceClass, int bDeviceSubClass, int bDeviceProtocol);

    int sendControlRequest(const mdtFrameUsbControl & frame);

    void fromThreadControlResponseReaden();

    void fromThreadMessageInReaden();

};
#endif
