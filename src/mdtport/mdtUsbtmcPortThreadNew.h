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
#ifndef MDT_USBTMC_PORT_THREAD_NEW_H
#define MDT_USBTMC_PORT_THREAD_NEW_H

#include "mdtUsbPortThreadNew.h"

class mdtUsbtmcTransferHandler;

/*! \brief USBTMC port thread
 */
class mdtUsbtmcPortThreadNew : public mdtUsbPortThreadNew
{
 public:

  /*! \brief Constructor
   */
  mdtUsbtmcPortThreadNew(mdtUsbtmcTransferHandler & th, libusb_context *usbContext, QObject *parent = 0);

  // Disable copy
  mdtUsbtmcPortThreadNew(const mdtUsbtmcPortThreadNew & rhs) = delete;
  mdtUsbtmcPortThreadNew & operator=(const mdtUsbtmcPortThreadNew & rhs) = delete;

 private:

  /*! \brief Things to do before closing
   */
  void aboutToClose();

  /*! \brief Thread implementation
   */
  void run();

  mdtUsbtmcTransferHandler & pvTransferHandler;
};

#endif // #ifndef MDT_USBTMC_PORT_THREAD_NEW_H
