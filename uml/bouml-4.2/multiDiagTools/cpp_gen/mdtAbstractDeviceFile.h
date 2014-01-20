#ifndef _MDTABSTRACTDEVICEFILE_H
#define _MDTABSTRACTDEVICEFILE_H


class mdtAbstractDeviceFile : public QObject {
  public:
     mdtAbstractDeviceFile(const QObject & parent = 0);

     ~mdtAbstractDeviceFile();

    virtual bool open(const QString & path) = 0;

    virtual void close() = 0;

    virtual void setReadTimeout(int timeout) = 0;

    virtual void setWriteTimeout(int timeout) = 0;

    virtual bool waitEventRead() = 0;

    virtual int readData(char data, int maxLen) = 0;

    virtual void flushIn() = 0;

    virtual bool waitEventWriteReady() = 0;

    virtual int writeData(char data, int maxLen) = 0;

    virtual void flushOut() = 0;


  protected:
    bool pvReadTimeoutOccured;

    bool pvReadTimeoutOccuredPrevious;

    bool pvWriteTimeoutOccured;

    bool pvWriteTimeoutOccuredPrevious;


  public:
    void updateReadTimeoutState(bool state);

    void updateWriteTimeoutState(bool state);

    void readTimeoutStateChanged(bool state);

    void writeTimeoutStateChanged(bool state);


  protected:
    QQueue<mdtFrame*><mdtFrame> pvReadenFrames;

    QQueue<mdtFrame*><mdtFrame> pvReadFramesPool;

    QQueue<mdtFrame*><mdtFrame> pvWriteFrames;

    QQueue<mdtFrame*><mdtFrame> pvWriteFramesPool;

};
#endif
