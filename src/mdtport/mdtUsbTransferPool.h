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
#ifndef MDT_USB_TRANSFER_POOL_H
#define MDT_USB_TRANSFER_POOL_H

#include <QtGlobal>
#include <vector>
#include <algorithm>
#include <libusb-1.0/libusb.h>

/*! \brief Storage of USB transfers
 */
template<typename T>
class mdtUsbTransferPool
{
 public:

  /*! \brief Constructor
   *
   * \param maxTransfers Maximum allowed number of transfers.
   *           This limit can be usefull for bug detection.
   *           If exceeded, getTransfer() will return a null pointer.
   *           In debug build, a assertion will also fail in this case.
   */
  mdtUsbTransferPool(unsigned int maxTransfers = 50)
   : pvDeviceHandle(0),
     pvMaxTransfers(maxTransfers),
     pvAllocatedTransfers(0)
  {
  }

  /*! \brief Destructor
   */
  ~mdtUsbTransferPool()
  {
    clear();
  }

  /*! \brief Clear pool
   *
   * Will also destroy each transfer.
   */
  void clear()
  {
    Q_ASSERT(pvTransfers.size() == pvAllocatedTransfers);
    for(T *transfer : pvTransfers){
      delete transfer;
    }
    pvTransfers.clear();
    pvDeviceHandle = 0;
  }

  ///mdtUsbtmcControlTransfer(mdtUsbtmcTransferHandler & th, libusb_device_handle *dev_handle, int bufferSize = 1024);

  /*! \brief Alloc transfers
   *
   * \param n Number of transfers
   * \param th Application specific transfer handler
   * \param devHandle Must be a valid pointer to currently open device.
   * \pre n must be <= maxTransfers (given in constructor)
   */
  template<typename TransferHandlerType>
  bool allocTransfers(unsigned int n, TransferHandlerType & th, libusb_device_handle *devHandle)
  {
    Q_ASSERT(devHandle != 0);
    Q_ASSERT(n <= pvMaxTransfers);

    int i;
    T *transfer;

    clear();
    pvDeviceHandle = devHandle;
    pvTransfers.reserve(n);
    for(i = 0; i < n; ++i){
      transfer = new T(th, pvDeviceHandle);
      if(transfer == 0){
        clear();
        return false;
      }
      pvTransfers.push_back(transfer);
    }
    pvAllocatedTransfers = pvTransfers.size();

    return true;
  }

  /*! \brief Get a transfer
   *
   * Will create a new transfer if pool is empty.
   *
   * \return A valid pointer to transfer (or 0 on memory allocation failure)
   */
  template<typename TransferHandlerType>
  T *getTransfer(TransferHandlerType & th)
  {
    Q_ASSERT(pvDeviceHandle != 0);

    T *transfer;

    if(pvTransfers.empty()){
      Q_ASSERT(pvAllocatedTransfers < pvMaxTransfers);
      if(pvAllocatedTransfers < pvMaxTransfers){
        ++pvAllocatedTransfers;
        return new T(th, pvDeviceHandle);
      }else{
        return 0;
      }
    }
    transfer = pvTransfers.back();
    Q_ASSERT(transfer != 0);
    pvTransfers.pop_back();

    return transfer;
  }

  /*! \brief Restore a transfer
   */
  void restoreTransfer(T *transfer)
  {
    Q_ASSERT(transfer != 0);
    Q_ASSERT(std::find(pvTransfers.begin(), pvTransfers.end(), transfer) == pvTransfers.end());
    pvTransfers.push_back(transfer);
  }

 private:

  Q_DISABLE_COPY(mdtUsbTransferPool);

  std::vector<T*> pvTransfers;
  libusb_device_handle *pvDeviceHandle;
  unsigned int pvMaxTransfers;
  unsigned int pvAllocatedTransfers;
};

#endif // #ifndef MDT_USB_TRANSFER_POOL_H
