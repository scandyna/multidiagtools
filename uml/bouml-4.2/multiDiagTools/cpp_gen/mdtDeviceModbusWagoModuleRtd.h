#ifndef _MDTDEVICEMODBUSWAGOMODULERTD_H
#define _MDTDEVICEMODBUSWAGOMODULERTD_H


#include "mdtDeviceModbusWagoModule.h"

class mdtDeviceModbusWago;

class mdtDeviceModbusWagoModuleRtd : public mdtDeviceModbusWagoModule {
  public:
    mdtDeviceModbusWagoModuleRtd(bool autoDeleteIos, const mdtDeviceModbusWago & device);

    ~mdtDeviceModbusWagoModuleRtd();

    virtual bool getSpecialModuleSetup(const quint16 & word, int firstAddressRead, int firstAddressWrite);

    //Will also enter password 0x1235 in register 31
    bool enterSetupMode();

    bool exitSetupMode();

    void setIosCount(int count);

    void setPssrMode(const pssrMode_t & mode);

    pssrMode_t pssrMode() const;

    void setSensorType(int channel, const sensorType_t & type);

    sensorType_t sensorType(int channel) const;

    void setConnectionTypeWiresCount(int channel, int wiresCount);

    int connectionTypeWiresCount(int channel) const;

    void setWatchdogTimerEnabled(int channel, bool enable);

    bool watchdogTimerIsEnabled(int channel) const;

    void setAverageValueFilterEnabled(int channel, bool enable);

    bool averageValueFilterIsEnabled(int channel) const;

    void setProcessValueRepresentation(int channel, const processValueRepresentation_t & representation);

    processValueRepresentation_t processValueRepresentation(int channel);

    void setSiemensFormatEnabled(int channel, bool enable);

    bool siemensFormatIsEnabled(int channel) const;

    void setWireBreakShortCircuitIndicationEnabled(int channel, bool enable);

    bool wireBreakShortCircuitIndicationIsEnabled(int channel) const;

    void setOutOfRangeIndicationEnabled(int channel, bool enable);

    bool outOfRangeIndicationIsEnabled(int channel) const;

    void setOutOfRangeIndicationLimits(int channel, int underrange, int overrange);

    int underrangeIndicationLimit(int channel);

    int overrangeIndicationLimit(int channel);

    void setManufacturerCalibration(int channel);

    void setUserCalibration(int channel, double offset, double gain);

    double calibrationOffset() const;

    double calibrationGain() const;

    void setScaling(int channel, bool useManufacturerScaling, double userOffset, double userGain, double twoWireOffset);

    bool manufacturerScalingIsEnabled(int channel) const;

    double manufacturerScalingOffset(int channel) const;

    double manufacturerScalingGain(int channel) const;

    double userScalingOffset(int channel) const;

    double userScalingGain(int channel) const;

    double twoWireOffset(int channel) const;


  private:
    <quint16> pvRegister32;

    <quint16> pvRegister33;

    <quint16> pvRegister34;

    <quint16> pvRegister35;

    <quint16> pvRegister36;

    <quint16> pvRegister37;

    <quint16> pvRegister39;

    <quint16> pvRegister40;

    quint16 pvRegister47;

};
#endif
