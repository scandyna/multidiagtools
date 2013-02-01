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
#ifndef MDT_USBTMC_PORT_THREAD_H
#define MDT_USBTMC_PORT_THREAD_H

#include <QObject>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <QString>
#include "mdtUsbPortThread.h"
#include "mdtFrame.h"
#include "mdtFrameUsbControl.h"

class mdtUsbPort;

class mdtUsbtmcPortThread : public mdtUsbPortThread
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtUsbtmcPortThread(QObject *parent = 0);

  /*! \brief Returns true
   */
  bool isReader() const;

  /*! \brief Returns true
   */
  bool isWriter() const;

 private:

  /*! \brief USBTMC write process
   *
   * \param writeFrame Pointer that points to frame (if null, this method will check if a new frame is to write).
   *                    Note: when this method returns, it's possible that
   *                          pointed frame pointer is null (no more data to write).
   * \param waitAnAnswer If a frame is completly written, and an answer is expected (ex. read request), this flag
   *                      will be updated.
   * \param expectedBulkInbTags If a answer is expected, the bTag will be appended to this list.
   * \return NoError or a fatal error (handled erros are handled internally and notifications are done).
   */
  mdtAbstractPort::error_t usbtmcWrite(mdtFrame **writeFrame, bool *waitAnAnswer, QList<quint8> &expectedBulkInbTags);

  /*! \brief Abort bulk IN
   *
   * Mutext must be locked when calling this method.
   *  It will be unlocked during wait.
   *
   * Note that pending control transfers will be cancelled.
   *
   * Once the abort process is done, port manager will be notified
   *  with a mdtAbstractPort::ReadCanceled (only if thread has running flag true).
   *  Port manager then knows that it can continue working.
   *
   * \return NoError or a error of type mdtAbstractPort::error_t.
   * \pre port must be set before call of this method
   */
  mdtAbstractPort::error_t abortBulkIn(quint8 bTag, mdtFrame **writeFrame, mdtFrame **readFrame);

  /*! \brief Send a INITIATE_ABORT_BULK_IN request thru the control endpoint
   *
   * \see abortBulkIn()
   * \return NoError or a error of type mdtAbstractPort::error_t.
   */
  mdtAbstractPort::error_t sendInitiateAbortBulkInRequest(quint8 bTag, mdtFrameUsbControl *ctlFrame);

  /*! \brief Send a CHECK_ABORT_BULK_IN_STATUS request thru the control endpoint
   *
   * \see abortBulkIn()
   * \return NoError or a error of type mdtAbstractPort::error_t.
   */
  mdtAbstractPort::error_t sendCheckAbortBulkInStatusRequest(quint8 bTag, mdtFrameUsbControl *ctlFrame);

  /*! \brief Working thread method
   */
  void run();

  Q_DISABLE_COPY(mdtUsbtmcPortThread);
};

#endif  // #ifndef MDT_USBTMC_PORT_THREAD_H
