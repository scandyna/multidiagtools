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
#ifndef MDT_FRAME_CODEC_K8055_H
#define MDT_FRAME_CODEC_K8055_H

#include "mdtFrameCodec.h"
#include <QByteArray>

/*! \brief Frame codec for Velleman k8055 board
 *
 * The Velleman k8055 is a USB I/O board:<br>
 *  http://www.velleman.eu/products/view/?id=351346<br>
 * Note: I don't know if the new version, called k8055N has the same
 *       format. When looking in demo tool, could say: possibly yes, but unshure.
 *
 * A very usefull help to build this codec was libk8055 library:
 *  http://libk8055.sourceforge.net/<br>
 * Thank to the authors witch "draw" the frames formats.
 */
class mdtFrameCodecK8055 : public mdtFrameCodec
{
 public:

  mdtFrameCodecK8055();
  ~mdtFrameCodecK8055();

  /*! \brief Decode a incoming frame
   *
   * After decoding is done, the values are available
   *  with mdtFrameCodec::values() with following order:
   *  - Indexes from 0 to 4: Input 1 to 5 , type: bool
   *  - Indexes from 5 to 6: Ain 1 to 2 , type: int
   *  - Indexes from 7 to 8: Counter value 1 to 2 , type: int
   */
  bool decode(QByteArray &data);

  /*! \brief Encode a reset frame
   */
  QByteArray encodeReset();

  /*! \brief Encode a frame to set counter debounce value
   *
   * \param counterNumber Counter number, must be 1 or 2
   * \param debounceValue Debounce value, must be a value from 0 to 255
   * \pre Parameters must be valid
   */
  QByteArray encodeSetCounterDebounceValue(int counterNumber, int debounceValue);

  /*! \brief Encode a frame to reset counter
   *
   * \param counterNumber Counter number, must be 1 or 2
   * \pre Parameters must be valid
   */
  QByteArray encodeResetCounter(int counterNumber);

  /*! \brief
   */
  QByteArray encodeSetOutputs();

  /*! \brief Set a digital channel state (On/Off)
   *
   * \param channel The channel, must be from 1 to 8
   * \param state State: On or Off
   * \pre Channel must be in a valid range
   */
  void setDigitalOut(int channel, bool state);

  /*! \brief Set a analog channel value
   *
   * \param channel The channel, must be from 1 to 2
   * \param value Value, must be from 0 to 255
   * \pre Parameters must be valid
   */
  void setAnalogOut(int channel, int value);

 private:

  quint8 pvCmd;
  quint8 pvStatus;
  quint8 pvAnalog1Value;
  quint8 pvAnalog2Value;
  quint8 pvDigitalChannels;
  bool pvResetCounter1;
  bool pvResetCounter2;
  quint16 pvCounter1Value;
  quint16 pvCounter2Value;
  quint8 pvCounter1DebounceValue;
  quint8 pvCounter2DebounceValue;
};

#endif  // #ifndef MDT_FRAME_CODEC_K8055_H
