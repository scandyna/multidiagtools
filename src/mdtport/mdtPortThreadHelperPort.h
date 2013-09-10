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
#ifndef MDT_PORT_THREAD_HELPER_PORT_H
#define MDT_PORT_THREAD_HELPER_PORT_H

#include "mdtPortThreadHelper.h"

/*! \brief Helper class for mdtPortThread and subclasses
 *
 * This class is helpfull for blocking API
 *  based only on mdtAbstractPort.
 */
class mdtPortThreadHelperPort : public mdtPortThreadHelper
{
 public:

  /*! \brief Constructor
   */
  mdtPortThreadHelperPort(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtPortThreadHelperPort();

  /*! \brief Reconnect
   *
   * This implementation calls mdtAbstractPort::reconnect() .
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   */
  mdtAbstractPort::error_t reconnect(bool notify);

  /*! \brief Read data from port
   *
   * This is a helper method to read data from port and
   *  to store chunk of data into a frame.
   *
   * Note about port mutex handling:<br>
   *  The port mutex must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \param emitNewFrameReaden If true, newFrameReaden() will be emitted each time a complete frame was generated.
   * \return The number of frames completed during the process.
   *          This can be helpful for query/reply protocols in witch the standard
   *          reply should be one frame.
   *         On error, one of the mdtAbstractPort::error_t is returned and thread must handle error itself.
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   */
  int readFromPort(bool emitNewFrameReaden);

  /*! \brief Write data to port
   *
   * This is a helper method
   *  to write some data from a frame to the port.
   *
   * Signal writeProcessBegin() is emitted.
   *
   * \param maxSize If >= 0, maxSize will be written, else the frame size will be considered.
   * \return Amount of bytes written or a mdtAbstractPort::error_t error.
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   */
  int writeDataToPort(int maxSize);

  /*! \brief Write a complete frame to port
   *
   * This is a helper method
   *  to write a frame to the port.
   *  Internally, it will call mdtAbstractPort::waitEventWriteReady()
   *   before writing.
   *
   * Note that frame will be put back to write pool after complete write, cancel, flush or error.
   *  That says that frame will not be valid after call of this method.
   *
   * Note about port mutex handling:<br>
   *  The port mutex must be locked before calling this method.
   *  Internally, it will be unlocked during wait, and will be
   *  locked again. So, the port mutex is allways locked when this
   *  method returns.
   *
   * \param bytePerByteWrite If true, one byte will be written once.
   * \param interByteTime Time between each byte write [ms] (has only effect if bytePerByteWrite is true)
   * \param maxWriteTry Some port can return 0 byte or a timeout error if busy. If this happens,
   *                     this method will sleep some time an try a write call again until
   *                     write call works successfull or maxWriteTry is reached.
   *
   * \return NoError on success or a mdtAbstractPort::error_t error (See also mdtPortThreadHelper::handleCommonWriteErrors() ).
   *
   * \pre Port must be set with mdtPortThreadHelper::setPort().
   * \pre Thread must be set with mdtPortThreadHelper::setThread().
   */
  mdtAbstractPort::error_t writeToPort(bool bytePerByteWrite, int interByteTime, int maxWriteTry = 10);

 private:

  char *pvReadBuffer;
  int pvReadBufferSize;

  Q_DISABLE_COPY(mdtPortThreadHelperPort);
};

#endif // #ifndef MDT_PORT_THREAD_HELPER_PORT_H
