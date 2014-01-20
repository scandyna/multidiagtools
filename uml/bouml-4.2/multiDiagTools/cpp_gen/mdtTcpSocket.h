#ifndef _MDTTCPSOCKET_H
#define _MDTTCPSOCKET_H


class mdtTcpSocket : public mdtAbstractPort {
  private:
    QTcpSocket pvSocket;


  public:
    mdtTcpSocket(const QObject & parent);

    ~mdtTcpSocket();


  private:
    int pvReadTimeout;

    int pvWriteTimeout;


  public:
    virtual bool open(const mdtPortConfig & cfg);

    virtual void close();

    virtual void setReadTimeout(int timeout);

    virtual void setWriteTimeout(int timeout);

    virtual bool waitForReadyRead();

    qInt64 read(char & data, const qInt64 & maxSize);

    virtual bool waitEventWriteReady();

    qInt64 write(char data, const qInt64 & maxSize);

};
#endif
