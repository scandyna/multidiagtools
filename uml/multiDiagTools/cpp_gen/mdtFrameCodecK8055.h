#ifndef _MDTFRAMECODECK8055_H
#define _MDTFRAMECODECK8055_H


class mdtFrameCodecK8055 : public mdtFrameCodec {
  private:
    uint8 pvCmd;

    uint8 pvStatus;

    uint8 pvAnalog1Value;

    uint8 pvAnalog2Value;

    uint8 pvDigitalChannels;

    bool pvResetCounter1;

    bool pvResetCounter2;

    uint16 pvCounter1Value;

    uint16 pvCounter2Value;

    uint8 pvCounter1DebounceValue;

    uint8 pvCounter2DebounceValue;


  public:
    mdtFrameCodecK8055();

    ~mdtFrameCodecK8055();

    bool decode(const QByteArray & data);

    QByteArray encodeReset();

    QByteArray encodeSetCounterDebounceValue(int counterNumber, int debounceValue);

    QByteArray encodeResetCounter(int counterNumber);

    QByteArray encodeSetIO();

    bool setDigitalOut(int channel, bool state);

    bool setAnalogOut(int channel, int value);

};
#endif
