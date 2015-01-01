/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtUsbtmcBulkTransfer.h"
#include "mdtUsbtmcTransferHandler.h"
#include <QtGlobal>

mdtUsbtmcBulkTransfer::mdtUsbtmcBulkTransfer(mdtUsbtmcTransferHandler& th, mdtUsbEndpointDescriptor endpointDescriptor, libusb_device_handle* dev_handle, int bufferSize)
 : mdtUsbTransfer(dev_handle, bufferSize),
   pvFrame(bufferSize),
   pvTransferHandler(th),
   pvEndpointDescriptor(endpointDescriptor),
   pvResponseExpected(false)
{
  Q_ASSERT(!pvEndpointDescriptor.isEmpty());
  clearSplitAction();
}

void mdtUsbtmcBulkTransfer::setupDevDepMsgOut(uint8_t bTag, mdtUsbtmcTxMessage & message, bool responseExpected, unsigned int timeout)
{
  Q_ASSERT(bTag != 0);

  clearSplitAction();
  pvFrame.clear();
  pvFrame.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT);
  pvFrame.setbTag(bTag);
  pvFrame.setData(message);
  pvFrame.setEOM(message.hasBytesToRead());
  pvResponseExpected = responseExpected;
  libusb_fill_bulk_transfer(transfer(), deviceHandle(),
                            pvEndpointDescriptor.address(), pvFrame.buffer(), pvFrame.bufferLength(),
                            pvTransferHandler.bulkOutTransferCallback, static_cast<void*>(this),
                            timeout);
}

void mdtUsbtmcBulkTransfer::setupRequestDevDepMsgIn(uint8_t bTag, bool termCharEnabled, char termChar, unsigned int timeout)
{
  Q_ASSERT(bTag != 0);

  clearSplitAction();
  pvFrame.clear();
  pvFrame.setMsgID(mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN);
  pvFrame.setbTag(bTag);
  pvFrame.setTransferSizeToMax();
  pvResponseExpected = false;
  if(termCharEnabled){
    pvFrame.setTermChar(termChar);
  }
  libusb_fill_bulk_transfer(transfer(), deviceHandle(),
                            pvEndpointDescriptor.address(), pvFrame.buffer(), 12,
                            pvTransferHandler.bulkOutTransferCallback, static_cast<void*>(this),
                            timeout);
}

void mdtUsbtmcBulkTransfer::setupBulkInRequest ( unsigned int timeout )
{
  clearSplitAction();
  pvFrame.clear();
  pvResponseExpected = false;
  libusb_fill_bulk_transfer(transfer(), deviceHandle(),
                            pvEndpointDescriptor.address(), pvFrame.buffer(), pvFrame.capacity(),
                            pvTransferHandler.bulkInTransferCallback, static_cast<void*>(this),
                            timeout);
}

void mdtUsbtmcBulkTransfer::dbgSetupCustom(uint8_t msgID, uint8_t bTag, uint8_t bTagInverse, uint32_t transferSize, uint8_t bmTransferAttributes, uint8_t termChar,
                                           bool responseExpected, int transferBufferLength, unsigned int timeout)
{
  Q_ASSERT(transferBufferLength <= pvFrame.capacity());

  clearSplitAction();
  pvFrame.clear();
  pvFrame.setMsgID(static_cast<mdtUsbtmcFrame::msgId_t>(msgID));
  pvFrame.setbTag(bTag);
  pvFrame.dbgSetCustomValue(2, bTagInverse);
  pvFrame.setTransferSize(transferSize);
  pvFrame.dbgSetCustomValue(8, bmTransferAttributes);
  pvFrame.dbgSetCustomValue(9, termChar);
  pvResponseExpected = responseExpected;
  libusb_fill_bulk_transfer(transfer(), deviceHandle(),
                            pvEndpointDescriptor.address(), pvFrame.buffer(), transferBufferLength,
                            pvTransferHandler.bulkOutTransferCallback, static_cast<void*>(this),
                            timeout);
}
