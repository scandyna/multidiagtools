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
#ifndef MDT_FRAME_H
#define MDT_FRAME_H

#include <QByteArray>

/*! \brief Provide an array of bytes
 *
 * mdtFrame is based on Qt's QByteArray, that provide a array of bytes.
 * To store data, the QByteArray's methods append() can be used.<br>
 * For ASCII frames reception, the end is a special char/strting on most case. mdtFrame
 * provide the functions putUntil() for that purpose.<br>
 * During data reception, for exemple from serial port, the best way to check if more data can be stored is to use bytesToStore().
 * It returns the remain capacity of the frame, and 0 if the end of frame (EOF) condition was
 * reached. To know if the frame is complete (i.e. end of frame condition ok), use
 * isComplete()<br>
 * Here are the states of a frame with return values of both functions:<br>
 *  <table border="1" cellpadding="5">
 *   <tr><th>State</th><th>bytesToStore()</th><th>isComplete()</th></tr>
 *   <tr><td>Frame is not full, and EOF condition not reached</td><td>remainCapacity()</td><td>false</td></tr>
 *   <tr><td>Frame is full, and EOF condition not reached</td><td>0</td><td>false</td></tr>
 *   <tr><td>Frame is not full, and EOF condition reached</td><td>0</td><td>true</td></tr>
 *   <tr><td>Frame is full, and EOF condition reached</td><td>0</td><td>true</td></tr>
 *  </table>
 * At this state, whenn bytesToStore() returns 0 and isComplete() is true, the received frame is ok.
 * The next step is to decode it, according to the used protocol (modbus, ...).<br>
 * Note that these states are only set when using putUntil() functions.
 */
class mdtFrame : public QByteArray
{
 public:

  mdtFrame();
  ~mdtFrame();

  enum type_t { mdtFrameTypeAscii = 1 };

  /*! \brief Set the ignore null values flag.
   * 
   * If set true, all null values ('\0' or 0) are not stored by putUntil() methods
   */
  void setIgnoreNullValues(bool ignoreNullValues);

  /*! \brief Put data until EOF token was reached
   *
   * This function is usefull for ASCII frames reception. Whenn token is reached, the EOF condition will become true (the EOF char is not stored).<br>
   * When the EOF condition was reached, this function does not store any more data.<br>
   * Note that this function is slower than QByteArray's append() method, because it compares each byte with the token.
   * \param data Pointer to source data
   * \param token Token that represents the EOF (End Of Frame)
   * \param maxLen Length of the array pointed by data
   * \return The number of bytes really stored
   * \pre The data pointer must be valid, and not be the internal pointer returned by data().
   */
  int putUntil(const char *data, char token, int maxLen);

  /*! \brief Put data until EOF token was reached
   * 
   * This is an overloaded function. The token can be a sequence of bytes.
   * \sa putUntil()
   */
  int putUntil(const char *data, QByteArray &token, int maxLen);

  /*! \brief Returns true if frame is full
   */
  bool isFull();

  /*! \brief Returns the remain capacity
   */
  int remainCapacity();

  /*! \brief Returns the bytes to store into the frame
   * 
   * Bytes to store can be the remainCapacity() value if frame is not full and EOF condition was not reached.
   * Else, it returns 0.
   */
  int bytesToStore();

  /*! \brief Returns true if EOF condition was reached
   */
  bool isComplete();

  /*! \brief Overloaded function
   * 
   * Internally, it calls QByteArray's clear() and reserve() functions.
   * The goal is to keep the same capacity and reset some mdtFrame specific flags
   */
  virtual void clear();

  /*! \brief Put data into the frame
   * 
   * Must be implemented by specific frame subclass.
   * This method is called by the RX thread.
   * Default implementation simply stores the maximum data as possible. No flags ares set.
   * \param data Valid pointer to the data
   * \param len Maximum number of byte to store, must not be more than data size
   * \return Number of bytes that could really be stored
   * \pre The data pointer must be valid, and not be the internal pointer returned by data().
   */
  virtual int putData(char *data, int len);

  /*! \brief Get the length of the end of frame sequence
   *  Used for ASCII frames.
   *  Default implementation returns 0
   */
  virtual int eofSeqLen();

  /*! \brief Take some data
   * 
   * \param data Pointer to data destination
   * \param len Number of bytes to take
   * \pre The data pointer must be valid, and not be the internal pointer returned by data().
   */
  int take(char *data, int len);

  /*! \brief Take some data
   * 
   * This method does not copy any data, but is usefull after a copy using QByteArray::data() .
   * The consumed bytes are removed from frame
   * \param data Pointer to data destination
   * \param len Number of bytes to take
   * \pre The data pointer must be valid, and not be the internal pointer returned by data().
   */
  int take(int len);

 protected:

  bool pvIgnoreNullValues;
  bool pvEOFcondition;  // End Of Frame condition
};
#endif  // ifndef MDT_FRAME_H
