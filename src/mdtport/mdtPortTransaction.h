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
#ifndef MDT_PORT_TRANSACTION_H
#define MDT_PORT_TRANSACTION_H

#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include <QByteArray>

/*! \brief Store port transaction identifications data
 *
 * After a query was sent to a device, a bit later
 *  a response will come back.
 *
 * It's not allways easy, as decoder method, to know
 *  what type of query was sent in the past, witch I/O
 *  the answer will concern, ...
 *
 * This class helps mdtPortManager and mdtDevice for that.
 */
class mdtPortTransaction
{
 public:

  /*! \brief Construct a empty transaction
   *
   * See clear() for details about empty transaction.
   */
  mdtPortTransaction();

  /*! \brief Destructor
   */
  ~mdtPortTransaction();

  /*! \brief Clear transaction
   *
   * A empty transaction has following values:
   *  - id: 0
   *  - type: 0
   *  - analogIo: Null
   *  - digitalIo: Null
   *  - forMultipleIos: false
   *  - isInput: false
   *  - isOutput: false
   *  - data: empty
   *  - isQueryReplyMode: false
   *  - address: 0
   */
  void clear();

  /*! \brief Set transaction identifer
   *
   * Depending used protocol, id
   *  can be a transaction ID as defined in MODBUS/TCP,
   *  a bTag used in USBTMC or what else.
   */
  void setId(int id);

  /*! \brief Get id
   *
   * See setId() for details.
   */
  int id() const;

  /*! \brief Set type
   *
   * A typical usage is SCPI query/request type.
   *  See mdtFrameCodecScpi for details.
   */
  void setType(int type);

  /*! \brief Get type
   *
   * See setType() for details
   */
  int type() const;

  /*! \brief Set the concerned analog I/O
   *
   * If a query concerns a specific I/O,
   *  it's possible to reference the I/O object
   *  here.
   */
  void setIo(mdtAnalogIo *io, bool isInput);

  /*! \brief Set the concerned digital I/O
   *
   * If a query concerns a specific I/O,
   *  it's possible to reference the I/O object
   *  here.
   */
  void setIo(mdtDigitalIo *io, bool isInput);

  /*! \brief Get the concerned analog I/O
   *
   * See setIo(mdtAnalogIo*) for details.
   */
  mdtAnalogIo *analogIo();

  /*! \brief Set the concerned digital I/O
   *
   * See setIo(mdtDigitalIo*) for details.
   */
  mdtDigitalIo *digitalIo();

  /*! \brief Set if the transaction concerns multiple I/O's
   *
   * A example using this member is MODBUS.
   *  See mdtDeviceModbus for details.
   */
  void setForMultipleIos(bool forMultipleIos);

  /*! \brief Check if the transaction concerns multiple I/O's
   *
   * See setForMultipleIos() for details.
   */
  bool forMultipleIos() const;

  /*! \brief Set the isInput flag
   */
  void setIsInput(bool isInput);

  /*! \brief Get the isInput flag
   */
  bool isInput() const;

  /*! \brief Get the isOutput flag
   */
  bool isOutput() const;

  /*! \brief Set data by copy
   */
  void setData(const QByteArray &data);

  /*! \brief Get stored data
   */
  QByteArray data() const;

  /*! \brief Set query/reply mode
   *
   * In query/reply mode, port manager
   *  will not remove frame from done
   *  transactions queue just after signal was emited,
   *  but keep it until a read was made.
   *
   * See mdtPortManager for details.
   */
  void setQueryReplyMode(bool mode);

  /*! \brief Get the query/reply mode flag
   *
   * See setQueryReplyMode() for details.
   */
  bool isQueryReplyMode() const;

  /*! \brief Set address
   */
  void setAddress(int address);

  /*! \brief Get address
   */
  int address() const;

 private:

  int pvId;
  mdtAnalogIo *pvAnalogIo;
  mdtDigitalIo *pvDigitalIo;
  int pvType;
  bool pvForMultipleIos;
  bool pvIsInput;
  bool pvIsOutput;
  QByteArray pvData;
  bool pvIsQueryReplyMode;
  int pvAddress;
};

Q_DECLARE_METATYPE(mdtPortTransaction);

#endif  // #ifndef MDT_PORT_TRANSACTION_H
