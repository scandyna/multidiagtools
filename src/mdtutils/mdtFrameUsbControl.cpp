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
#include "mdtFrameUsbControl.h"

#include <QDebug>

mdtFrameUsbControl::mdtFrameUsbControl()
 : mdtFrame()
{
  clearSub();
}

mdtFrameUsbControl::~mdtFrameUsbControl()
{
}

void mdtFrameUsbControl::clearSub()
{
  pvbmRequestType = 0;
  pvbRequest = 0;
  pvwValue = 0;
  pvwIndex = 0;
  pvwLength = 0;
}

void mdtFrameUsbControl::setDirectionDeviceToHost(bool deviceToHost)
{
  if(deviceToHost){
    pvbmRequestType |= (1<<7);
  }else{
    pvbmRequestType &= !(1<<7);
  }
}

bool mdtFrameUsbControl::directionIsDeviceToHost() const
{
  return pvbmRequestType & (1<<7);
}

void mdtFrameUsbControl::setRequestType(request_type_t type)
{
  pvbmRequestType |= ((type & 0x03)<<5);
}

mdtFrameUsbControl::request_type_t mdtFrameUsbControl::requestType() const
{
  return (request_type_t)((pvbmRequestType & 0x60)>>5);
}

void mdtFrameUsbControl::setRequestRecipient(request_recipient_t recipient)
{
  pvbmRequestType |= (recipient & 0x0F);
}

mdtFrameUsbControl::request_recipient_t mdtFrameUsbControl::requestRecipient() const
{
  return (request_recipient_t)(pvbmRequestType & 0x0F);
}

void mdtFrameUsbControl::setbRequest(quint8 request)
{
  pvbRequest = request;
}

quint8 mdtFrameUsbControl::bRequest() const
{
  return pvbRequest;
}

void mdtFrameUsbControl::setwValue(quint16 value)
{
  pvwValue = value;
}

quint16 mdtFrameUsbControl::wValue() const
{
  return pvwValue;
}

void mdtFrameUsbControl::setwIndex(quint16 index)
{
  pvwIndex = index;
}

quint16 mdtFrameUsbControl::wIndex() const
{
  return pvwIndex;
}

void mdtFrameUsbControl::setwLength(quint16 length)
{
  pvwLength = length;
}

quint16 mdtFrameUsbControl::wLength() const
{
  return pvwLength;
}

void mdtFrameUsbControl::encode()
{
  mdtFrame::clear();

  append((char)pvbmRequestType);
  append((char)pvbRequest);
  append((char)pvwValue & 0xFF);          // wValue, LSB
  append((char)(pvwValue >> 8) & 0xFF);   // wValue, MSB
  append((char)pvwIndex & 0xFF);          // wIndex, LSB
  append((char)(pvwIndex >> 8) & 0xFF);   // wIndex, MSB
  append((char)pvwLength & 0xFF);         // wLength, LSB
  append((char)(pvwLength >> 8) & 0xFF);  // wLength, MSB
}
