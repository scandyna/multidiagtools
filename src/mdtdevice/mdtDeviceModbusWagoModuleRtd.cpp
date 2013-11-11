/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtDeviceModbusWagoModuleRtd.h"
#include "mdtDeviceModbusWago.h"
#include "mdtError.h"
#include "mdtAnalogIo.h"
#include <QString>

mdtDeviceModbusWagoModuleRtd::mdtDeviceModbusWagoModuleRtd(bool autoDeleteIos, mdtDeviceModbusWago *device)
 : mdtDeviceModbusWagoModule(autoDeleteIos, device)
{
  // Set factory settings
  /// \todo Finish/correct
  ///setIoCount(4);
  setIoCount(2);
  /**
  setSensorType(0, Pt100);
  setSensorType(1, Pt100);
  setSensorType(2, Pt100);
  setSensorType(3, Pt100);
  */
  setSensorType(0, Pt100);
  setSensorType(1, Pt1000);
  setProcessValueRepresentation(0, C2);
  setProcessValueRepresentation(1, C2);
  /**
  setProcessValueRepresentation(2, C2);
  setProcessValueRepresentation(3, C2);
  */
}

mdtDeviceModbusWagoModuleRtd::~mdtDeviceModbusWagoModuleRtd()
{
}

void mdtDeviceModbusWagoModuleRtd::setIoCount(int count)
{
  Q_ASSERT((count == 2)||(count == 4));

  int i;

  // Check if I/O count is to update first
  if(analogInputs().size() == count){
    Q_ASSERT(pvSensorTypes.size() == count);
    Q_ASSERT(pvProcessValueRepresentations.size() == count);
    return;
  }
  // Update..
  qDebug() << "RTD - new I/O count: " << count;
  pvSensorTypes.clear();
  pvProcessValueRepresentations.clear();
  for(i = 0; i < count; ++i){
    pvSensorTypes.append(Pt100);
    pvProcessValueRepresentations.append(C2);
  }
}

void mdtDeviceModbusWagoModuleRtd::setPsrrMode(psrrMode_t mode)
{
  pvPsrrMode = mode;
}

mdtDeviceModbusWagoModuleRtd::psrrMode_t mdtDeviceModbusWagoModuleRtd::psrrMode() const
{
  return pvPsrrMode;
}

void mdtDeviceModbusWagoModuleRtd::setSensorType(int channel, sensorType_t type)
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvSensorTypes.size());

  pvSensorTypes[channel] = type;
}

mdtDeviceModbusWagoModuleRtd::sensorType_t mdtDeviceModbusWagoModuleRtd::sensorType(int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvSensorTypes.size());

  return pvSensorTypes.at(channel);
}

void mdtDeviceModbusWagoModuleRtd::setConnectionTypeWiresCount(int channel, int wiresCount)
{
}

int mdtDeviceModbusWagoModuleRtd::connectionTypeWiresCount(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setWatchdogTimerEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::watchdogTimerIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setAverageValueFilterEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::averageValueFilterIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setProcessValueRepresentation(int channel, processValueRepresentation_t representation)
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvProcessValueRepresentations.size());

  pvProcessValueRepresentations[channel] = representation;
}

mdtDeviceModbusWagoModuleRtd::processValueRepresentation_t mdtDeviceModbusWagoModuleRtd::processValueRepresentation(int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvProcessValueRepresentations.size());

  return pvProcessValueRepresentations.at(channel);
}

bool mdtDeviceModbusWagoModuleRtd::siemensFormatIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setWireBreakShortCircuitIndicationEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::wireBreakShortCircuitIndicationIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setOutOfRangeIndicationEnabled(int channel, bool enable)
{
}

bool mdtDeviceModbusWagoModuleRtd::outOfRangeIndicationIsEnabled(int channel) const
{
}

void mdtDeviceModbusWagoModuleRtd::setOutOfRangeIndicationLimits(int channel, int underrange, int overrange)
{
}

int mdtDeviceModbusWagoModuleRtd::underrangeIndicationLimit(int channel)
{
}

int mdtDeviceModbusWagoModuleRtd::overrangeIndicationLimit(int channel)
{
}

void mdtDeviceModbusWagoModuleRtd::setManufacturerCalibration(int channel)
{
}

void mdtDeviceModbusWagoModuleRtd::setUserCalibration(int channel, double offset, double gain)
{
}

double mdtDeviceModbusWagoModuleRtd::calibrationOffset() const
{
}

double mdtDeviceModbusWagoModuleRtd::calibrationGain() const
{
}

void mdtDeviceModbusWagoModuleRtd::setScaling(int channel, bool useManufacturerScaling, bool useUserScaling, double userOffset, double userGain, double twoWireOffset)
{
}

bool mdtDeviceModbusWagoModuleRtd::manufacturerScalingIsEnabled(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::manufacturerScalingOffset(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::manufacturerScalingGain(int channel) const
{
}

bool mdtDeviceModbusWagoModuleRtd::userScalingIsEnabled(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::userScalingOffset(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::userScalingGain(int channel) const
{
}

double mdtDeviceModbusWagoModuleRtd::twoWireOffset(int channel) const
{
}

QVariant mdtDeviceModbusWagoModuleRtd::analogInputValueMin(int partNumber, int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvSensorTypes.size());

  switch(sensorType(channel)){
    case Pt100:
      return -200.0;
    case Pt200:
      return -200.0;
    case Pt500:
      return -200.0;
    case Pt1000:
      return -200.0;
    case Ni100:
      return -60.0;
    case Ni120:
      return -80.0;
    case Ni1000:
      return -60.0;
    case Ni1000TK:
      return -60.0;
    case R10R1200:
      return 10.0;
    case R10R5000:
      return 10.0;
    case Potentiometer:
      return 0.0;
    case Unknown:
      return QVariant();
  }
}

QVariant mdtDeviceModbusWagoModuleRtd::analogInputValueMax(int partNumber, int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvSensorTypes.size());

  switch(sensorType(channel)){
    case Pt100:
      return 850.0;
    case Pt200:
      return 850.0;
    case Pt500:
      return 850.0;
    case Pt1000:
      return 850.0;
    case Ni100:
      return 300.0;
    case Ni120:
      return 260.0;
    case Ni1000:
      return 300.0;
    case Ni1000TK:
      return 250.0;
    case R10R1200:
      return 1200.0;
    case R10R5000:
      return 5000.0;
    case Potentiometer:
      return 100.0;
    case Unknown:
      return QVariant();
  }
}

int mdtDeviceModbusWagoModuleRtd::analogInputValueBitsCount(int partNumber, int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvProcessValueRepresentations.size());

  switch(processValueRepresentation(channel)){
    case C2:
      return 16;
    case AmountSign:
      return 16;
    case C2S5:
      return 13;
    case AmountSignS5:
      return 13;
  }
}

int mdtDeviceModbusWagoModuleRtd::analogInputValueLsbIndex(int partNumber, int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvProcessValueRepresentations.size());

  switch(processValueRepresentation(channel)){
    case C2:
      return 0;
    case AmountSign:
      return 0;
    case C2S5:
      return 3;
    case AmountSignS5:
      return 3;
  }
}

QVariant mdtDeviceModbusWagoModuleRtd::analogInputValueSigned(int partNumber, int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvProcessValueRepresentations.size());

  switch(processValueRepresentation(channel)){
    case C2:
      return true;
    case AmountSign:
      return true;
    case C2S5:
      return true;
    case AmountSignS5:
      return true;
  }
}

QVariant mdtDeviceModbusWagoModuleRtd::analogInputValueScaledFromMinToMax(int partNumber, int channel) const
{
  return false;
}

QVariant mdtDeviceModbusWagoModuleRtd::analogInputValueConversionFactor(int partNumber, int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvSensorTypes.size());

  /// \todo Depend on format !
  switch(sensorType(channel)){
    case Pt100:
      return 0.1;
    case Pt200:
      return 0.1;
    case Pt500:
      return 0.1;
    case Pt1000:
      return 0.1;
    case Ni100:
      return 0.1;
    case Ni120:
      return 0.1;
    case Ni1000:
      return 0.1;
    case Ni1000TK:
      return 0.1;
    case R10R1200:
      return 0.1;
    case R10R5000:
      return 0.5;
    case Potentiometer:
      return 0.1;
    case Unknown:
      return QVariant();
  }
}

int mdtDeviceModbusWagoModuleRtd::analogInputsCount(int partNumber) const
{
  Q_ASSERT(pvProcessValueRepresentations.size() == pvSensorTypes.size());

  return pvSensorTypes.size();
}

int mdtDeviceModbusWagoModuleRtd::analogOutputsCount(int partNumber) const
{
  return 0;
}

QString mdtDeviceModbusWagoModuleRtd::analogInputUnit(int partNumber, int channel) const
{
  Q_ASSERT(channel >= 0);
  Q_ASSERT(channel < pvSensorTypes.size());

  switch(sensorType(channel)){
    case Pt100:
      return "°C";
    case Pt200:
      return "°C";
    case Pt500:
      return "°C";
    case Pt1000:
      return "°C";
    case Ni100:
      return "°C";
    case Ni120:
      return "°C";
    case Ni1000:
      return "°C";
    case Ni1000TK:
      return "°C";
    case R10R1200:
      return "Ohm";
    case R10R5000:
      return "Ohm";
    case Potentiometer:
      return "%";
    case Unknown:
      return "";
  }
}

/**
void mdtDeviceModbusWagoModuleRtd::updateRegisterCount()
{
  int firstAddressRD, firstAddressWR;
  int count;
  int i;

  // Get module's addreeses and channel count
  firstAddressRD = firstAiAddress();
  firstAddressWR = firstAoAddressWrite();
  Q_ASSERT(firstAddressRD >= 0);
  Q_ASSERT(firstAddressWR >= 0);
  count = ioCountRegisters();
  // Update mdtDeviceModbusWagoModule's register count
  if(channelCountRegisters() != count){
    qDebug() << "* Have a new count of C/S/D bytes: " << count;
    clearRegisters();
    addRegisters(count, firstAddressRD, firstAddressWR);
  }
  // Update internal registers
  Q_ASSERT(pvRegister33.size() == pvRegister32.size());
  Q_ASSERT(pvRegister33.size() == pvRegister32.size());
  Q_ASSERT(pvRegister34.size() == pvRegister32.size());
  Q_ASSERT(pvRegister35.size() == pvRegister32.size());
  Q_ASSERT(pvRegister36.size() == pvRegister32.size());
  Q_ASSERT(pvRegister37.size() == pvRegister32.size());
  Q_ASSERT(pvRegister39.size() == pvRegister32.size());
  Q_ASSERT(pvRegister40.size() == pvRegister32.size());
  if(pvRegister32.size() != count){
    qDebug() << "* Have a new count of registers: " << count;
    pvRegister32.clear();
    pvRegister33.clear();
    pvRegister34.clear();
    pvRegister35.clear();
    pvRegister36.clear();
    pvRegister37.clear();
    pvRegister39.clear();
    pvRegister40.clear();
    for(i = 0; i < count; ++i){
      pvRegister32.append((quint16)0);
      pvRegister33.append((quint16)0);
      pvRegister34.append((quint16)0);
      pvRegister35.append((quint16)0);
      pvRegister36.append((quint16)0);
      pvRegister37.append((quint16)0);
      pvRegister39.append((quint16)0);
      pvRegister40.append((quint16)0);
    }
  }
}
*/
