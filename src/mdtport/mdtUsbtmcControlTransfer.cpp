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
#include "mdtUsbtmcControlTransfer.h"
#include "mdtUsbtmcTransferHandler.h"
#include <QtGlobal>
#include <cstring>

mdtUsbtmcControlTransfer::mdtUsbtmcControlTransfer (mdtUsbtmcTransferHandler & th, libusb_device_handle* dev_handle, int bufferSize )
 : mdtUsbControlTransfer(dev_handle, bufferSize),
   pvTransferHandler(th)
{
}

void mdtUsbtmcControlTransfer::setupClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout)
{
  setupClearFeature(pvTransferHandler.controlTransferCallback,
                    mdtUsbControlTransfer::RequestRecipient_t::Endpoint, mdtUsbControlTransfer::StandardFeatureSelector_t::ENDPOINT_HALT, endpointNumber,
                    timeout);
}

void mdtUsbtmcControlTransfer::setupInitiateAbortBulkOut(uint8_t bTag, uint8_t endpointNumber, unsigned int timeout)
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA2, static_cast<uint8_t>(USBTMCbRequest::INITIATE_ABORT_BULK_OUT), bTag, endpointNumber, 2,
               timeout);
}

void mdtUsbtmcControlTransfer::setupCheckAbortBulkOutStatus(uint8_t endpointNumber, unsigned int timeout)
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA2, static_cast<uint8_t>(USBTMCbRequest::CHECK_ABORT_BULK_OUT_STATUS), 0, endpointNumber, 8,
               timeout);
}

void mdtUsbtmcControlTransfer::setupInitiateAbortBulkIn ( uint8_t bTag, uint8_t endpointNumber, unsigned int timeout )
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA2, static_cast<uint8_t>(USBTMCbRequest::INITIATE_ABORT_BULK_IN), bTag, (0b10000000 | endpointNumber), 2,
               timeout);
}


void mdtUsbtmcControlTransfer::setupCheckAbortBulkInStatus ( uint8_t endpointNumber, unsigned int timeout )
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA2, static_cast<uint8_t>(USBTMCbRequest::CHECK_ABORT_BULK_IN_STATUS), 0, (0b10000000 | endpointNumber), 8,
               timeout);
}

void mdtUsbtmcControlTransfer::setupInitiateClear ( uint8_t interfaceNumber, unsigned int timeout )
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA1, static_cast<uint8_t>(USBTMCbRequest::INITIATE_CLEAR), 0, interfaceNumber, 1,
               timeout);
}

void mdtUsbtmcControlTransfer::setupCheckClearStatus ( uint8_t interfaceNumber, unsigned int timeout )
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA1, static_cast<uint8_t>(USBTMCbRequest::CHECK_CLEAR_STATUS), 0, interfaceNumber, 2,
               timeout);
}

void mdtUsbtmcControlTransfer::setupGetCapabilities ( uint8_t interfaceNumber, unsigned int timeout )
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA1, static_cast<uint8_t>(USBTMCbRequest::GET_CAPABILITIES), 0, interfaceNumber, 0x0018,
               timeout);
}

void mdtUsbtmcControlTransfer::setupIndicatorPulse ( uint8_t interfaceNumber, unsigned int timeout )
{
  fillTransfer(pvTransferHandler.controlTransferCallback,
               0xA1, static_cast<uint8_t>(USBTMCbRequest::INDICATOR_PULSE), 0, interfaceNumber, 1,
               timeout);
}

QString mdtUsbtmcControlTransfer::usbtmcRequestText() const
{
  QString str;

  switch(usbtmcRequest()){
    case USBTMCbRequest::INITIATE_ABORT_BULK_OUT:
      str = "INITIATE_ABORT_BULK_OUT";
      break;
    case USBTMCbRequest::CHECK_ABORT_BULK_OUT_STATUS:
      str = "CHECK_ABORT_BULK_OUT_STATUS";
      break;
    case USBTMCbRequest::INITIATE_ABORT_BULK_IN:
      str = "INITIATE_ABORT_BULK_IN";
      break;
    case USBTMCbRequest::CHECK_ABORT_BULK_IN_STATUS:
      str = "CHECK_ABORT_BULK_IN_STATUS";
      break;
    case USBTMCbRequest::INITIATE_CLEAR:
      str = "INITIATE_CLEAR";
      break;
    case USBTMCbRequest::CHECK_CLEAR_STATUS:
      str = "CHECK_CLEAR_STATUS";
      break;
    case USBTMCbRequest::GET_CAPABILITIES:
      str = "GET_CAPABILITIES";
      break;
    // No default section, so compiler can tell us if we forgot to implement a status
  }
  if(str.isEmpty()){
    str = "Unknown";
  }
  // Add code
  str += QString(" (0x%1)").arg(static_cast<int>(usbtmcRequest()), 2, 16, QChar('0'));

  return str;
}

QString mdtUsbtmcControlTransfer::usbtmcStatusText() const
{
  QString str;

  switch(usbtmcStatus()){
    case USBTMCstatus::STATUS_SUCCESS:
      str = "STATUS_SUCCESS";
      break;
    case USBTMCstatus::STATUS_PENDING:
      str = "STATUS_PENDING";
      break;
    case USBTMCstatus::STATUS_FAILED:
      str = "STATUS_FAILED";
      break;
    case USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS:
      str = "STATUS_TRANSFER_NOT_IN_PROGRESS";
      break;
    case USBTMCstatus::STATUS_SPLIT_NOT_IN_PROGRESS:
      str = "STATUS_SPLIT_NOT_IN_PROGRESS";
      break;
    case USBTMCstatus::STATUS_SPLIT_IN_PROGRESS:
      str = "STATUS_SPLIT_IN_PROGRESS";
      break;
    case USBTMCstatus::STATUS_INTERRUPT_IN_BUSY:
      str = "STATUS_INTERRUPT_IN_BUSY";
      break;
    // No default section, so compiler can tell us if we forgot to implement a status
  }
  if(str.isEmpty()){
    str = "Unknown";
  }
  // Add code
  str += QString(" (0x%1)").arg(static_cast<int>(usbtmcStatus()), 2, 16, QChar('0'));

  return str;
}
