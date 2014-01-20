#ifndef _MDTFRAMECODECMODBUS_H
#define _MDTFRAMECODECMODBUS_H


class mdtFrameCodecModbus : public mdtFrameCodec {
  public:
    mdtFrameCodecModbus();

    ~mdtFrameCodecModbus();

    QByteArray encodeReadCoils(const quint16 & startAddress, int n);

    int decode();


  private:
    bool decodeReadCoils();

    QByteArray pvPdu;

};
#endif
