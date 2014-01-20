#ifndef _MDTFRAMEMODBUSTCP_H
#define _MDTFRAMEMODBUSTCP_H


class mdtFrameModbusTcp : public mdtFrame {
  private:
    quint16 pvTransactionId;

    quint16 pvProtocolId;

    quint16 pvLength;

    quint8 pvUnitId;


  public:
    mdtFrameModbusTcp();

    ~mdtFrameModbusTcp();

    virtual int putData(char data, int maxLen);

    void setPdu(const QByteArray & pdu);

    QByteArray getPdu();

    void buildMbapHeader();

    void parseMbapHeader();

};
#endif
