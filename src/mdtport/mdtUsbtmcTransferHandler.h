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
#ifndef MDT_USBTMC_TRANSFER_HANDLER_H
#define MDT_USBTMC_TRANSFER_HANDLER_H

#include <QtGlobal>
#include <libusb-1.0/libusb.h>
#include <cstdint>

/*! \brief USBTMC transfer handler
 */
class mdtUsbtmcTransferHandler
{
 public:

  /*! \brief Constructor
   */
  mdtUsbtmcTransferHandler();

  /*! \brief Control transfer callback
   */
  static void controlTransferCallback(libusb_transfer *transfer);

 private:

  Q_DISABLE_COPY(mdtUsbtmcTransferHandler);
};

#endif  // #ifndef MDT_USBTMC_TRANSFER_HANDLER_H
