#ifndef _MDTDEVICEMODBUS_H
#define _MDTDEVICEMODBUS_H


class mdtDeviceModbus : public mdtDevice {
  public:
    mdtDeviceModbus(const QObject & parent);

    virtual ~mdtDeviceModbus();


  private:
    <mdtModbusTcpPortManager> pvTcpPortManager;

    mdtFrameCodecModbus pvCodec;

    mdtDeviceModbusWidget * ;

    QMap<quint16,quint16> pvAnalogInputs;

    QMap<quint16,quint16> pvAnalogOutputs;

    QMap<quint16,bool> pvDigitalInputs;

    QMap<quint16,bool> pvDigitalOutputs;


  public:
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
