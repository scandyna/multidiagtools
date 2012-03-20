#ifndef _MDTTCPSERVERTHREAD_H
#define _MDTTCPSERVERTHREAD_H


class mdtTcpServerThread : public QThread {
  public:
    mdtTcpServerThread(const QObject & parent);

    virtual void run();


  private:
    int pvSocketDescriptor;

};
#endif
