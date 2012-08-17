/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtFrameCodecK8055.h"
#include "mdtError.h"

#include <QDebug>

mdtFrameCodecK8055::mdtFrameCodecK8055()
{
  pvDigitalChannels = 0;
  pvAnalog1Value = 0;
  pvAnalog2Value = 0;
}

mdtFrameCodecK8055::~mdtFrameCodecK8055()
{
}

bool mdtFrameCodecK8055::decode(QByteArray &data)
{
  char byte;
  int word;

  pvValues.clear();
  // Check data
  if(data.size() != 8){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Wrong size, expected: 8", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecK8055");
    e.commit();
    return false;
  }
  /// \todo check statu field ??
  // Digital in
  qDebug() << "Statu: " << (quint8)data.at(1);
  byte = data.at(0);
  qDebug() << "Byte: " << (quint8)byte;
  pvValues.append((bool)(byte & (1<<4)));
  pvValues.append((bool)(byte & (1<<5)));
  pvValues.append((bool)(byte & 0x01));
  pvValues.append((bool)(byte & (1<<6)));
  pvValues.append((bool)(byte & (1<<7)));
  /**
  pvValues.append((bool)(byte & (1<<5)));
  pvValues.append((bool)(byte & (1<<6)));
  pvValues.append((bool)(byte & (1<<7)));
  */
  // Analog in
  pvValues.append((int)((quint8)data.at(2)));
  pvValues.append((int)((quint8)data.at(3)));
  // Counters
  word = 0;
  word = ((quint8)data.at(4) << 8) | (quint8)data.at(5);
  pvValues.append(word);
  word = 0;
  word = ((quint8)data.at(6) << 8) | (quint8)data.at(7);
  pvValues.append(word);

  return true;
}

QByteArray mdtFrameCodecK8055::encodeReset()
{
  QByteArray frame(8, (char)0x00);

  return frame;
}

QByteArray mdtFrameCodecK8055::encodeSetCounterDebounceValue(int counterNumber, int debounceValue)
{
  Q_ASSERT((counterNumber >= 1)&&(counterNumber <= 2));
  Q_ASSERT((debounceValue >= 0)&&(debounceValue <= 255));

  QByteArray frame;

  // Command
  frame.append((char)0x01);
  // Unused bytes
  frame.append((char)0x00);
  frame.append((char)0x00);
  frame.append((char)0x00);
  frame.append((char)0x00);
  frame.append((char)0x00);
  // Debounce value
  if(counterNumber == 1){
    frame.append((char)debounceValue);
    frame.append((char)0x00);
  }else{
    frame.append((char)0x00);
    frame.append((char)debounceValue);
  }

  return frame;
}

QByteArray mdtFrameCodecK8055::encodeResetCounter(int counterNumber)
{
  Q_ASSERT((counterNumber >= 1)&&(counterNumber <= 2));

  QByteArray frame(8, (char)0x00);

  // Reset commande depending of selected counter
  if(counterNumber == 1){
    frame[0] = (char)0x03;
  }else{
    frame[0] = (char)0x04;
  }

  return frame;
}

QByteArray mdtFrameCodecK8055::encodeSetOutputs()
{
  QByteArray frame;

  // Command
  frame.append((char)0x05);
  // Digital out
  frame.append((char)pvDigitalChannels);
  // Analog out
  frame.append((char)pvAnalog1Value);
  frame.append((char)pvAnalog2Value);
  frame.append((char)0x00);
  frame.append((char)0x00);
  frame.append((char)0x00);
  frame.append((char)0x00);

  return frame;
}

void mdtFrameCodecK8055::setDigitalOut(int channel, bool state)
{
  Q_ASSERT((channel >= 1)&&(channel <= 8));

  if(state){
    pvDigitalChannels |= (1 << (channel-1));
  }else{
    pvDigitalChannels &= !(1 << (channel-1));
  }
}

void mdtFrameCodecK8055::setAnalogOut(int channel, int value)
{
  Q_ASSERT((channel >= 1)&&(channel <= 2));
  Q_ASSERT((value >= 0)&&(value <= 255));

  if(channel == 1){
    pvAnalog1Value = value;
  }else{
    pvAnalog2Value = value;
  }
}

