#ifndef _MDTFRAMECODEC_H
#define _MDTFRAMECODEC_H


class mdtFrameCodec {
  public:
     mdtFrameCodec();

     ~mdtFrameCodec();

    virtual bool decode(const QByteArray & frame) = 0;

    virtual QByteArray encode() = 0;

};
#endif
