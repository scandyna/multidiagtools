#ifndef _MDTDEVICEMODBUSWAGOMODULE_H
#define _MDTDEVICEMODBUSWAGOMODULE_H


class mdtDeviceModbusWago;
class mdtAnalogIo;
class mdtDigitalIo;

class mdtDeviceModbusWagoModule {
  private:
    <mdtAnalogIo> pvAnalogIos;

    <mdtDigitalIo> pvDigitalIos;

    bool pvAutoDeleteIos;

    type_t Type;

    QString TypeText;

    int pvPartNumber;

    QList<QPair<int, quint8> > pvStatusBytes;

    QList<QPair<int, quint8> > pvControlBytes;

    QList<quint16> pvRegisterDataWords;

    <mdtDeviceModbusWagoModuleAttribute> pvAttributes;


  public:
    mdtDeviceModbusWagoModule(bool autoDeleteIos, const mdtDeviceModbusWago & device);

    virtual ~mdtDeviceModbusWagoModule();

    bool setupFromRegisterWord(const quint16 & word);

    void setFirstAddresse(int addressRead, int addressWrite);

    virtual bool getSpecialModuleSetup(const quint16 & word, int firstAddressRead, int firstAddressWrite) = 0;

    int firstAddressRead();

    int firstAddressWrite();

    int lastAddressRead();

    int lastAddressWrite();

    int ioCount() const;

    QList<mdtAnalogIo*> analogIos();

    QList<mdtDigitalIo*> digitalIos();

    type_t type() const;

    QString typeText() const;

    int partNumber() const;

    QString partNumberText() const;


  private:
    QVariant analogIoModuleValueMin(int partNumber) const;

    QVariant analogIoModuleValueMax(int partNumber) const;

    QVariant analogIoModuleValueBitsCount(int partNumber) const;

    QVariant analogIoModuleValueLsbIndex(int partNumber) const;

    QVariant analogIoModuleValueSigned(int partNumber) const;

    QVariant analogIoModuleIsInput(int partNumber) const;

    QVariant analogIoModuleIosCount(int partNumber) const;

    QVariant analogIoModuleUnit(int partNumber) const;

    QVariant digitalIoModuleIsInput(const quint16 & word) const;

    QVariant digitalIoModuleIosCount(const quint16 & word) const;

    bool addAnalogIos();

    bool addDigitalIos();


  protected:
    void addAnalogIo(const mdtAnalogIo & aio);

    void addDigitalIo(const mdtDigitalIo & dio);

    void addRegisters(int channelCount);

    quint8 statusByte(int channel) const;

    void setControlByte(int channel, const quint8 & byte);

    quint8 controlByte(int channel) const;

    void setRegisterDataWord(int channel, const quint16 & word);

    quint16 registerDataWord(int channel);

    void setRegisterDataBytes(int channel, const quint8 & byteD1, const quint8 & byteD0);

    quint8 registerDataByteD1(int channel);

    quint8 registerDataByteD0(int channel);

    //Will also read status byte
    bool readRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState);

    //Will also write control byte
    bool writeRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState);

    bool readStatusBytes(int firstChannel, int lastChannel, bool needDeviceSetupState);

    bool writeControlBytes(int firstChannel, int lastChannel, bool needDeviceSetupState);

};
#endif
