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
#ifndef MDT_DEVICE_H
#define MDT_DEVICE_H

#include "mdtDeviceIos.h"
#include <QObject>
#include <QString>
#include <QStringList>

/*! \brief Base class for a device connected to a port
 *
 * Give some helper methods for event handling, ....
 *
 * If it is needed to continiusly update the representation
 *  of device's inputs (digital/analog I/O, voltage, temperature, ...),
 *  two things can happen:
 *  - 1) Used port can receive data continiusly (f.ex. RS232) and device send data automatically
 *  - 2) Used port need that a request is sent for each read (TCP, USB, ...) or device need such request
 * Continous update is typical usefull for a GUI representing a real device.
 *
 * A other usage is automated sequence. In this case, the sequence will tell when it need
 *  to set a output and when a input value is reuqierd.
 *
 * A device can accept one or many request befor results must be read. This depends most case
 *  on used port and protocol.
 *
 * It can be useful to find on witch port a device is attached (f.ex. setup of a automated test application).
 *
 * A device can have several states (ready, busy, disconnected, ...). To help the application programmer
 *  to keep coherence, this states are updated in this class. (signal, ...)
 */
class mdtDevice : public QObject
{
 Q_OBJECT

 public:

  mdtDevice(QObject *parent = 0);
  virtual ~mdtDevice();

  /*! \brief Set the I/O's container
   *
   * \pre ios must be a valid pointer
   */
  void setIos(mdtDeviceIos *ios);

  /*! \brief Read one analog input on physical device and update (G)UI representation
   *
   * \param address Depending on device organisation and protocol,
   *                 this can be a relative or absolute address (f.ex. MODBUS queries),
   *                 a input number, etc...
   * \return true on success, false else.
   *
   * Subclass notes:<br>
   *  - This default implementation does nothing and returns allways false.
   *  - If device handled by subclass has analog inputs, this method should be implemented.
   *  - The subclass must handle and document the behaviour of calling this method without any I/O's container set.
   *  - To update (G)UI, the mdtDeviceIos::updateAnalogInputValue() should be used.
   */
  virtual bool readAnalogInput(int address);

  /// \todo Version "blocante" (ret: QVariant, attente avec portmanager, ...)

  /*! \brief Read all analog inputs on physical device and update (G)UI representation
   *
   * \return 0 or a ID on success, value < 0 on error (see mdtPortManager::writeData() for details)
   *
   * Subclass notes:<br>
   *  - This default implementation does nothing and returns allways -1.
   *  - If device handled by subclass has analog inputs, this method should be implemented.
   *  - The subclass must handle and document the behaviour of calling this method without any I/O's container set.
   *  - To update (G)UI, mdtDeviceIos::updateAnalogInputValues() should be used.
   */
  virtual int readAnalogInputs();

  /// \todo Version "blocante" (ret: QList<QVariant>, attente avec portmanager, ...)

 public slots:

  /*! \brief Decode incoming frames
   *
   * Subclass notes:<br>
   *  - This default implementation does nothing.
   *  - This slot should be connected with mdtPortManager::newReadenFrame() signal.
   *  - In this class, this connection is not made, it is the sublcass responsability to do this.
   *  - The incoming frames are available with mdtPortManager::readenFrames().
   */
  virtual void decodeReadenFrames();

 protected:

  mdtDeviceIos *pvIos;    // I/O's container

 private:

  Q_DISABLE_COPY(mdtDevice);
};

#endif  // #ifndef MDT_DEVICE_H
