#ifndef _MDTTCPSOCKETTHREAD_H
#define _MDTTCPSOCKETTHREAD_H


class mdtTcpSocketThread : public mdtPortThread {
  public:
    mdtTcpSocketThread(const QObject & parent);

    void newFrameReaden();


  private:
    <char> pvReadBuffer;

    <char> pvWriteBuffer;

    void readFromSocket();

    mdtFrame getNewFrameRead();

    void writeToSocket();

    mdtFrame getNewFrameWrite();

    void run();

    QTcpSocket pvSocket;

};
#endif
