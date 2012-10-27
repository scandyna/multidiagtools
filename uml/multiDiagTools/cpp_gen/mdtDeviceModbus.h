#ifndef _MDTDEVICEMODBUS_H
#define _MDTDEVICEMODBUS_H


class mdtModbusTcpPortManager;

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
    QVariant getAnalogInputValue(int address, bool readDirectly);

    bool readAnalogInputs();

    bool setAnalogOutput(int address, int value, bool writeDirectly);

    virtual QVariant getAnalogOutputValue(int address, bool readDirectly) = 0;

    bool writeAnalogOutputs();

    virtual bool readAnalogOutputs() = 0;

    QVariant getDigitalInputState(int address, bool readDirectly);

    bool readDigitalInputs();

    bool setDigitalOutput(int address, bool state, bool writeDirectly);

    virtual QVariant getDigitalOutputState(int address, bool readDirectly) = 0;

    bool writeDigitalOutputs(const  & );

    virtual bool readDigitalOutputs(const  & ) = 0;

};
#endif
