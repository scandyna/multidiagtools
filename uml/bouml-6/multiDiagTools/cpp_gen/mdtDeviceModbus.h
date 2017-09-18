#ifndef _MDTDEVICEMODBUS_H
#define _MDTDEVICEMODBUS_H


class mdtDeviceModbus : public mdtDevice {
  private:
    <mdtModbusTcpPortManager> pvTcpPortManager;

    mdtFrameCodecModbus pvCodec;

    mdtDeviceModbusWidget * ;


  public:
    mdtDeviceModbus(const QObject & parent);

    virtual ~mdtDeviceModbus();

  mdtAbstractPort::error_t connectToDevice(const QList<mdtPortInfo*> & scanResult, int hardwareNodeId, int bitsCount, int startFrom);

  virtual mdtAbstractPort::error_t connectToDevice(const mdtPortInfo & portInfo) = 0;

    bool getRegisterValues(int address, int n);

const   QList<int> registerValues() const;

    QVariant getAnalogInputValue(int address, bool readDirectly);

    bool setAnalogOutput(int address, int value, bool writeDirectly);

    virtual QVariant getAnalogOutputValue(int address, bool readDirectly) = 0;

    QVariant getDigitalInputState(int address, bool readDirectly);

    bool setDigitalOutput(int address, bool state, bool writeDirectly);

    virtual QVariant getDigitalOutputState(int address, bool readDirectly) = 0;


  protected:
    bool readAnalogInputs();

    virtual bool readAnalogOutputs() = 0;

    bool writeAnalogOutputs();

    bool readDigitalInputs();

    bool writeDigitalOutputs(const mdtPortTransaction & transaction);

    virtual bool readDigitalOutputs(const mdtPortTransaction & transaction);

};
#endif
