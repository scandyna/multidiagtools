#ifndef _MDTDEVICEIOS_H
#define _MDTDEVICEIOS_H


#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"

class mdtDeviceIosSegment;

class mdtDeviceIos : public QObject {
  private:
    bool pvAutoDeleteIos;


  public:
    mdtDeviceIos(const QObject & parent);

    ~mdtDeviceIos();

    void setAutoDeleteIos(bool autoDelete);

    void deleteIos();


  protected:
    <mdtAnalogIo *> pvAnalogInputs;

    <mdtAnalogIo *> pvAnalogOutputs;

    <mdtDigitalIo *> pvDigitalInputs;

    <mdtDigitalIo *> pvDigitalOutputs;


  public:
    void addAnalogInput(const mdtAnalogIo & ai);

    mdtAnalogIo analogInputAt(int address);

    QList<mdtAnalogIo> analogInputs();

    void addAnalogOutput(const mdtAnalogIo & ao);

    mdtAnalogIo analogOutputAt(int address);

    QList<mdtAnalogIo> analogOutputs();

    void addDigitalInput(const mdtDigitalIo & di);

    mdtDigitalIo digitalInputAt(int address);

    QList<mdtDigitalIo> digitalInputs();

    void addDigitalOutput(const mdtDigitalIo & di);

    mdtDigitalIo digitalOutputAt(int address);

    QList<mdtDigitalIo> digitalOutputs();

    void analogOutputValueChanged(int address, double value);

    void digitalOutputStateChanged(int address, bool state);

    void updateAnalogInputValue(int address, double value);

    void updateAnalogOutputValue(int address, double value);

    void updateDigitalInputState(int address, bool state);

    void updateDigitalOutputState(int address, bool state);


  private:
    <mdtDeviceIosSegment> ;

};
#endif
