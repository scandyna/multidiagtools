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
#include "mdtDeviceIos.h"
#include "mdtDeviceIosSegment.h"
#include <QMapIterator>
#include <QMutableListIterator>
#include <QtGlobal>

#include <QDebug>

mdtDeviceIos::mdtDeviceIos(QObject *parent)
 : QObject(parent)
{
  pvAutoDeleteIos = true;
  ///pvAnalogInputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressWrite = 0;
  pvDigitalInputsFirstAddressRead = 0;
  pvDigitalOutputsFirstAddressRead = 0;
  pvDigitalOutputsFirstAddressWrite = 0;
}

mdtDeviceIos::~mdtDeviceIos()
{
  // Delete I/O objects
  qDebug() << "mdtDeviceIos::~mdtDeviceIos() ...";
  if(pvAutoDeleteIos){
    deleteIos();
  }else{
    pvAnalogInputs.clear();
    pvAnalogInputsByAddressRead.clear();
    ///pvAnalogInputsFirstAddressRead = 0;
    qDeleteAll(pvAnalogInputsSegments);
    pvAnalogInputsSegments.clear();
    pvAnalogOutputs.clear();
    pvAnalogOutputsByAddressRead.clear();
    pvAnalogOutputsByAddressWrite.clear();
    pvAnalogOutputsFirstAddressRead = 0;
    pvAnalogOutputsFirstAddressWrite = 0;
    qDeleteAll(pvAnalogOutputsSegments);
    pvAnalogOutputsSegments.clear();
    pvDigitalInputs.clear();
    pvDigitalInputsByAddressRead.clear();
    pvDigitalInputsFirstAddressRead = 0;
    /// \todo Don't forget segment container
    pvDigitalOutputs.clear();
    pvDigitalOutputsByAddressRead.clear();
    pvDigitalOutputsByAddressWrite.clear();
    pvDigitalOutputsFirstAddressRead = 0;
    pvDigitalOutputsFirstAddressWrite = 0;
    /// \todo Don't forget segment container
  }

  qDebug() << "mdtDeviceIos::~mdtDeviceIos() DONE";
}

void mdtDeviceIos::setAutoDeleteIos(bool autoDelete)
{
  pvAutoDeleteIos = autoDelete;
}

void mdtDeviceIos::deleteIos()
{
  qDebug() << "delete I/Os ...";
  qDeleteAll(pvAnalogInputs);
  pvAnalogInputs.clear();
  pvAnalogInputsByAddressRead.clear();
  ///pvAnalogInputsFirstAddressRead = 0;
  qDeleteAll(pvAnalogInputsSegments);
  pvAnalogInputsSegments.clear();
  qDeleteAll(pvAnalogOutputs);
  pvAnalogOutputs.clear();
  pvAnalogOutputsByAddressRead.clear();
  pvAnalogOutputsByAddressWrite.clear();
  pvAnalogOutputsFirstAddressRead = 0;
  pvAnalogOutputsFirstAddressWrite = 0;
  qDeleteAll(pvAnalogOutputsSegments);
  pvAnalogOutputsSegments.clear();
  qDeleteAll(pvDigitalInputs);
  pvDigitalInputs.clear();
  pvDigitalInputsByAddressRead.clear();
  pvDigitalInputsFirstAddressRead = 0;
  /// \todo Don't forget segment container
  qDeleteAll(pvDigitalOutputs);
  pvDigitalOutputs.clear();
  pvDigitalOutputsByAddressRead.clear();
  pvDigitalOutputsByAddressWrite.clear();
  pvDigitalOutputsFirstAddressRead = 0;
  pvDigitalOutputsFirstAddressWrite = 0;
  /// \todo Don't forget segment container
  qDebug() << "delete I/Os DONE";
}

void mdtDeviceIos::addAnalogInput(mdtAnalogIo *ai)
{
  Q_ASSERT(ai != 0);

  QList<mdtAnalogIo*> aiList;
  mdtDeviceIosSegment *segment;
  QList<mdtAnalogIo*> segmentAiList;
  int i;

  // Check that I/O was not allready set and add it
  Q_ASSERT(!pvAnalogInputs.contains(ai));
  Q_ASSERT(!pvAnalogInputsByAddressRead.contains(ai->addressRead()));
  pvAnalogInputs.append(ai);
  pvAnalogInputsByAddressRead.insert(ai->addressRead(), ai);
  Q_ASSERT(pvAnalogInputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvAnalogInputsByAddressRead.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  ///pvAnalogInputsFirstAddressRead = pvAnalogInputsByAddressRead.values().at(0)->addressRead();
  // We must reorganize segments - We use the QMap conatiner, because it is allready sorted by keys, ascending
  qDeleteAll(pvAnalogInputsSegments);
  pvAnalogInputsSegments.clear();
  aiList = pvAnalogInputsByAddressRead.values();
  Q_ASSERT(aiList.size() > 0);
  // We know that we have at least 1 I/O - Just create a segment and add first item
  segment = new mdtDeviceIosSegment;
  segmentAiList.append(aiList.at(0));
  for(i = 1; i < aiList.size(); ++i){
    Q_ASSERT(aiList.at(i) != 0);
    // If address of current item is not directly successor of previous, we have a hole -> create a new segment
    if((aiList.at(i)->addressRead() - aiList.at(i-1)->addressRead() ) != 1){
      // Add current segment to list and create a new one
      segment->setIos(segmentAiList);
      segmentAiList.clear();
      pvAnalogInputsSegments.append(segment);
      segment = new mdtDeviceIosSegment;
    }
    segmentAiList.append(aiList.at(i));
  }
  // Add current segment to list
  segment->setIos(segmentAiList);
  pvAnalogInputsSegments.append(segment);
}

mdtAnalogIo *mdtDeviceIos::analogInputAt(int address)
{
  return pvAnalogInputsByAddressRead.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogInputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvAnalogInputs.size(); ++i){
    Q_ASSERT(pvAnalogInputs.at(i) != 0);
    if(pvAnalogInputs.at(i)->labelShort() == labelShort){
      return pvAnalogInputs.at(i);
    }
  }

  return 0;
}

const QList<mdtAnalogIo*> mdtDeviceIos::analogInputs() const
{
  return pvAnalogInputs;
}

const QList<mdtDeviceIosSegment*> &mdtDeviceIos::analogInputsSegments() const
{
  return pvAnalogInputsSegments;
}

int mdtDeviceIos::analogInputsCount() const
{
  return pvAnalogInputs.size();
}

void mdtDeviceIos::setAnalogInputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvAnalogInputs.size(); ++i){
    Q_ASSERT(pvAnalogInputs.at(i) != 0);
    pvAnalogInputs.at(i)->setValue(value);
  }
}

void mdtDeviceIos::addAnalogOutput(mdtAnalogIo *ao, mdtDeviceIos::addressAccess_t sortSegmentsBy)
{
  Q_ASSERT(ao != 0);

  QList<mdtAnalogIo*> aoList;
  mdtDeviceIosSegment *segment;
  QList<mdtAnalogIo*> segmentAoList;
  int i;
  int currentAddress, previousAddress;

  // Check if I/O was allready set and add it
  Q_ASSERT(!pvAnalogOutputs.contains(ao));
  Q_ASSERT(!pvAnalogOutputsByAddressRead.contains(ao->addressRead()));
  Q_ASSERT(!pvAnalogOutputsByAddressWrite.contains(ao->addressWrite()));
  pvAnalogOutputs.append(ao);
  pvAnalogOutputsByAddressRead.insert(ao->addressRead(), ao);
  pvAnalogOutputsByAddressWrite.insert(ao->addressWrite(), ao);
  // Get first addresses for read and write access
  Q_ASSERT(pvAnalogOutputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvAnalogOutputsByAddressRead.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvAnalogOutputsFirstAddressRead = pvAnalogOutputsByAddressRead.values().at(0)->addressRead();
  Q_ASSERT(pvAnalogOutputsByAddressWrite.values().size() > 0);
  Q_ASSERT(pvAnalogOutputsByAddressWrite.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvAnalogOutputsFirstAddressWrite = pvAnalogOutputsByAddressWrite.values().at(0)->addressWrite();  /// \todo Obselete
  // We must reorganize segments - We use the QMap conatiner, because it is allready sorted by keys, ascending
  qDeleteAll(pvAnalogOutputsSegments);
  pvAnalogOutputsSegments.clear();
  if(sortSegmentsBy == Read){
    aoList = pvAnalogOutputsByAddressRead.values();
  }else{
    aoList = pvAnalogOutputsByAddressWrite.values();
  }
  Q_ASSERT(aoList.size() > 0);
  // We know that we have at least 1 I/O - Just create a segment and add first item
  segment = new mdtDeviceIosSegment;
  segmentAoList.append(aoList.at(0));
  for(i = 1; i < aoList.size(); ++i){
    Q_ASSERT(aoList.at(i) != 0);
    // If address of current item is not directly successor of previous, we have a hole -> create a new segment
    if(sortSegmentsBy == Read){
      currentAddress = aoList.at(i)->addressRead();
      previousAddress = aoList.at(i-1)->addressRead();
    }else{
      currentAddress = aoList.at(i)->addressWrite();
      previousAddress = aoList.at(i-1)->addressWrite();
    }
    if((currentAddress - previousAddress) != 1){
      // Add current segment to list and create a new one
      segment->setIos(segmentAoList);
      segmentAoList.clear();
      pvAnalogOutputsSegments.append(segment);
      segment = new mdtDeviceIosSegment;
    }
    segmentAoList.append(aoList.at(i));
  }
  // Add current segment to list
  segment->setIos(segmentAoList);
  pvAnalogOutputsSegments.append(segment);
}

mdtAnalogIo *mdtDeviceIos::analogOutputAtAddressRead(int address)
{
  return pvAnalogOutputsByAddressRead.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogOutputAtAddressWrite(int address)
{
  return pvAnalogOutputsByAddressWrite.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogOutputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    if(pvAnalogOutputs.at(i)->labelShort() == labelShort){
      return pvAnalogOutputs.at(i);
    }
  }

  return 0;
}

const QList<mdtAnalogIo*> mdtDeviceIos::analogOutputs() const
{
  return pvAnalogOutputs;
}

const QList<mdtDeviceIosSegment*> &mdtDeviceIos::analogOutputsSegments() const
{
  return pvAnalogOutputsSegments;
}

int mdtDeviceIos::analogOutputsFirstAddressRead() const
{
  return pvAnalogOutputsFirstAddressRead;
}

int mdtDeviceIos::analogOutputsFirstAddressWrite() const
{
  return pvAnalogOutputsFirstAddressWrite;
}

int mdtDeviceIos::analogOutputsCount() const
{
  return pvAnalogOutputs.size();
}

QList<int> mdtDeviceIos::analogOutputsValuesIntByAddressWrite() const
{
  QList<int> values;
  QList<mdtAnalogIo*> aos;
  int i;

  aos = pvAnalogOutputsByAddressWrite.values();
  for(i=0; i<aos.size(); i++){
    Q_ASSERT(aos.at(i) != 0);
    values.append(aos.at(i)->value().valueInt());
  }

  return values;
}

void mdtDeviceIos::setAnalogOutputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setValue(value, false);
  }
}

void mdtDeviceIos::addDigitalInput(mdtDigitalIo *di)
{
  Q_ASSERT(di != 0);

  pvDigitalInputs.append(di);
  pvDigitalInputsByAddressRead.insert(di->address(), di);
  // QMap returns a list sorted by keys, ascending
  Q_ASSERT(pvDigitalInputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvDigitalInputsByAddressRead.values().at(0) != 0);
  pvDigitalInputsFirstAddressRead = pvDigitalInputsByAddressRead.values().at(0)->addressRead();
}

mdtDigitalIo *mdtDeviceIos::digitalInputAt(int address)
{
  return pvDigitalInputsByAddressRead.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalInputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvDigitalInputs.size(); ++i){
    Q_ASSERT(pvDigitalInputs.at(i) != 0);
    if(pvDigitalInputs.at(i)->labelShort() == labelShort){
      return pvDigitalInputs.at(i);
    }
  }

  return 0;
}

const QList<mdtDigitalIo*> mdtDeviceIos::digitalInputs() const
{
  return pvDigitalInputs;
}

int mdtDeviceIos::digitalInputsFirstAddress() const
{
  return pvDigitalInputsFirstAddressRead;
}

int mdtDeviceIos::digitalInputsCount() const
{
  return pvDigitalInputs.size();
}

void mdtDeviceIos::setDigitalInputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvDigitalInputs.size(); ++i){
    Q_ASSERT(pvDigitalInputs.at(i) != 0);
    pvDigitalInputs.at(i)->setValue(value);
  }
}

void mdtDeviceIos::addDigitalOutput(mdtDigitalIo *dout)
{
  Q_ASSERT(dout != 0);

  pvDigitalOutputs.append(dout);
  pvDigitalOutputsByAddressRead.insert(dout->addressRead(), dout);
  pvDigitalOutputsByAddressWrite.insert(dout->addressWrite(), dout);
  // Get first addresses for read and write access
  Q_ASSERT(pvDigitalOutputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvDigitalOutputsByAddressRead.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvDigitalOutputsFirstAddressRead = pvDigitalOutputsByAddressRead.values().at(0)->addressRead();
  Q_ASSERT(pvDigitalOutputsByAddressWrite.values().size() > 0);
  Q_ASSERT(pvDigitalOutputsByAddressWrite.values().at(0) != 0);
  // QMap returns a list sorted by keys, ascending
  pvDigitalOutputsFirstAddressWrite = pvDigitalOutputsByAddressWrite.values().at(0)->addressWrite();
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAtAddressRead(int address)
{
  return pvDigitalOutputsByAddressRead.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAtAddressWrite(int address)
{
  return pvDigitalOutputsByAddressWrite.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalOutputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    if(pvDigitalOutputs.at(i)->labelShort() == labelShort){
      return pvDigitalOutputs.at(i);
    }
  }

  return 0;
}

const QList<mdtDigitalIo*> mdtDeviceIos::digitalOutputs() const
{
  return pvDigitalOutputs;
}

int mdtDeviceIos::digitalOutputsFirstAddressRead() const
{
  return pvDigitalOutputsFirstAddressRead;
}

int mdtDeviceIos::digitalOutputsFirstAddressWrite() const
{
  return pvDigitalOutputsFirstAddressWrite;
}

int mdtDeviceIos::digitalOutputsCount() const
{
  return pvDigitalOutputs.size();
}

void mdtDeviceIos::setDigitalOutputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    pvDigitalOutputs.at(i)->setValue(value);
  }
}

QList<bool> mdtDeviceIos::digitalOutputsStatesByAddressWrite() const
{
  QList<bool> states;
  QList<mdtDigitalIo*> dos;
  int i;

  dos = pvDigitalOutputsByAddressWrite.values();
  for(i=0; i<dos.size(); i++){
    Q_ASSERT(dos.at(i) != 0);
    states.append(dos.at(i)->value().valueBool());
  }

  return states;
}

void mdtDeviceIos::updateAnalogInputValues(const QList<QVariant> &values, const int firstAddress, const int n, bool matchAddresses)
{
  int i, max, cursor, stored;
  mdtDeviceIosSegment *segment;
  QList<mdtDeviceIosSegment*> segmentList;
  int segmentSpace;
  bool found = false;
  int address;

  // Find the first segment
  i = 0;
  if(analogInputsSegments().isEmpty()){
    return;
  }
  if(firstAddress < 0){
    segment = analogInputsSegments().at(0);
    Q_ASSERT(segment != 0);
    address = segment->startAddressRead();
    found = true;
  }else{
    address = firstAddress;
    for(i = 0; i < analogInputsSegments().size(); ++i){
      segment = analogInputsSegments().at(i);
      Q_ASSERT(segment != 0);
      if(segment->containsAddressRead(firstAddress)){
        found = true;
        break;
      }
    }
  }
  if(!found){
    return;
  }
  Q_ASSERT(segment != 0);
  segmentSpace = segment->ioCount();
  segmentList.append(segment);
  // Fix quantity of inputs to update
  if(n < 0){
    max = qMin(values.size(), pvAnalogInputs.size());
  }else{
    max = qMin(n, pvAnalogInputs.size());
  }
  Q_ASSERT(max <= values.size());
  // Find needed amount of segments to store max items
  for(i = 1; i < analogInputsSegments().size(); ++i){
    segment = analogInputsSegments().at(i);
    Q_ASSERT(segment != 0);
    segmentSpace += segment->ioCount();
    segmentList.append(segment);
    if(segmentSpace >= max){
      break;
    }
  }
  // Update
  cursor = 0;
  Q_ASSERT(segmentList.size() > 0);
  segment = segmentList.at(0);
  i = 0;
  while(cursor < max){
    // Store in current segment
    if(matchAddresses){
      stored = segment->updateValuesFromAddressRead(address, values.mid(cursor));
    }else{
      stored = segment->setValues(values.mid(cursor));
    }
    // Get next segment - If no more available, we have finished
    ++i;
    if(i < segmentList.size()){
      segment = segmentList.at(i);
    }else{
      return;
    }
    Q_ASSERT(segment != 0);
    // Update data cursor and address
    cursor += stored;
    address += stored;
    // If matchAddresses is true , we must matter about address match
    if(matchAddresses){
      while((cursor < max)&&(!segment->containsAddressRead(address))){
        ++address;
        ++cursor;
      }
    }
  }
}

void mdtDeviceIos::updateAnalogOutputValues(const QList<QVariant> &values, int firstAddressRead, int n, bool matchAddresses)
{
  int i, max;
  QList<mdtAnalogIo*> lst;
  QVariant var;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvAnalogOutputsByAddressRead.values();  // We update (G)UI, so we read from device
  // Remove items with address < firstAddressRead
  qDebug() << "Setting values - firstAddressRead: " << firstAddressRead;
  if((firstAddressRead > -1)&&(firstAddressRead > analogOutputsFirstAddressRead())){
    QMutableListIterator<mdtAnalogIo*> it(lst);
    while(it.hasNext()){
      it.next();
      Q_ASSERT(it.value() != 0);
      if(it.value()->addressRead() >= firstAddressRead){
        break;
      }
      it.remove();
    }
  }
  // Fix quantity of outputs and update outputs
  if(n < 0){
    n = lst.size();
  }
  max = qMin(values.size(), n);
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    var = values.at(i);
    switch(var.type()){
      case QVariant::Double:
        lst.at(i)->setValue(var.toDouble(), false);
        break;
      case QVariant::Int:
        lst.at(i)->setValue(var.toInt(), false);
        break;
      default:
        lst.at(i)->setValue(var.value<mdtValue>(), false);
    }
  }
}

void mdtDeviceIos::setAnalogOutputsEnabled(bool enabled)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setEnabled(enabled);
  }
}

void mdtDeviceIos::updateDigitalInputValues(const QList<QVariant> &values, int firstAddress, int n)
{
  int i, max;
  QList<mdtDigitalIo*> lst;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvDigitalInputsByAddressRead.values();
  // Remove items with address < firstAddress
  if((firstAddress > -1)&&(firstAddress > digitalInputsFirstAddress())){
    QMutableListIterator<mdtDigitalIo*> it(lst);
    while(it.hasNext()){
      it.next();
      Q_ASSERT(it.value() != 0);
      if(it.value()->address() >= firstAddress){
        break;
      }
      it.remove();
    }
  }
  // Fix quantity of inputs and update inputs
  if(n < 0){
    n = lst.size();
  }
  max = qMin(values.size(), n);
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    if((values.at(i).isValid())&&(values.at(i).type() == QVariant::Bool)){
      lst.at(i)->setValue(values.at(i).toBool(), false);
    }else{
      lst.at(i)->setValue(mdtValue(), false);
    }
  }
}

void mdtDeviceIos::updateDigitalOutputValues(const QList<QVariant> &values, int firstAddressRead, int n)
{
  int i, max;
  QList<mdtDigitalIo*> lst;

  // Get the list from address conatiner, so we have it sorted by address (QMap returns a sorted list, by keys, ascending)
  lst = pvDigitalOutputsByAddressRead.values();  // We update (G)UI, so we read from device
  // Remove items with addressRead < firstAddressRead
  if((firstAddressRead > -1)&&(firstAddressRead > digitalOutputsFirstAddressRead())){
    QMutableListIterator<mdtDigitalIo*> it(lst);
    while(it.hasNext()){
      it.next();
      Q_ASSERT(it.value() != 0);
      if(it.value()->addressRead() >= firstAddressRead){
        break;
      }
      it.remove();
    }
  }
  // Fix quantity of outputs and update outputs
  if(n < 0){
    n = lst.size();
  }
  max = qMin(values.size(), n);
  for(i=0; i<max; ++i){
    Q_ASSERT(lst.at(i) != 0);
    if((values.at(i).isValid())&&(values.at(i).type() == QVariant::Bool)){
      lst.at(i)->setValue(values.at(i).toBool(), false);
    }else{
      lst.at(i)->setValue(mdtValue());
    }
  }
}

void mdtDeviceIos::setDigitalOutputsEnabled(bool enabled)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    pvDigitalOutputs.at(i)->setEnabled(enabled);
  }
}
