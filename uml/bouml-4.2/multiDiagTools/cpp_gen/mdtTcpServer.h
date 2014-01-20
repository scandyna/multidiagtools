#ifndef _MDTTCPSERVER_H
#define _MDTTCPSERVER_H


class mdtTcpServer : public QTcpServer {
  public:
    mdtTcpServer(const QObject & parent);


  protected:
    virtual void incomingConnection(int socketDescriptor);

};
#endif
