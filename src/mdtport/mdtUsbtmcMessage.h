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
#ifndef MDT_USBTMC_MESSAGE_H
#define MDT_USBTMC_MESSAGE_H

#include <QByteArray>
#include <algorithm>
#include <cstring>
#include <cstdint>

/*! \brief USBTMC message
 *
 * For concept of USBTMC message,
 *  see USBTMC 1.0 specifications,
 *  section 3.2.1.1 and 3.3.1
 */
class mdtUsbtmcMessage
{
public:

  /*! \brief Construct a message that acts on given byte array
   */
  mdtUsbtmcMessage(QByteArray & ba)
   : pvByteArray(ba),
     pvCursor(0)
  {
  }

  /*! \brief Reset internal cursor to the beginning of byte array
   */
  void reset()
  {
    pvCursor = 0;
  }

  /*! \brief Get amount of bytes that are to read
   */
  int bytesToRead() const
  {
    if(pvCursor > pvByteArray.size()){
      // Byte array was cleared or resized to less than original size
      return 0;
    }
    return pvByteArray.size() - pvCursor;
  }

  /*! \brief Check if there are bytes to read from byte array
   */
  inline bool hasBytesToRead() const
  {
    return (bytesToRead() > 0);
  }

  /*! \brief Copy data from byte array to buffer
   *
   * \param dest Pointer to destination buffer
   * \param max Maximum count of bytes to copy
   * \return Number of bytes that was copied
   * \pre dest must be valid
   * \post Will never copy more than max bytes
   */
  int read(uint8_t *dest, int max)
  {
    Q_ASSERT(dest != 0);

    // Check if byte array was resized in less size than original (or simply cleared) - If so -> reset
    if(pvCursor > pvByteArray.size()){
      reset();
    }
    // Calculate number of bytes to copy
    int n = std::min(bytesToRead(), max);
    if(n < 1){
      return 0;
    }
    Q_ASSERT(n <= max);
    Q_ASSERT(n >= 0);
    Q_ASSERT((pvCursor + n - 1) < pvByteArray.size());
    // Copy and update cursor
    ::memcpy(dest, pvByteArray.constData() + pvCursor, n);
    pvCursor += n;
    Q_ASSERT(pvCursor <= pvByteArray.size());

    return n;
  }

  /*! \brief Copy data from buffer to byte array
   *
   * Will resize byte array so that all data can be copied.
   *
   * \param src Pointer to source buffer
   * \param n Number of bytes to copy
   * \pre src must be valid
   * \pre n must be >= 0
   */
  void write(const uint8_t *src, int n)
  {
    Q_ASSERT(src != 0);
    Q_ASSERT(n >= 0);

    // Reserve enouth data in byte array
    pvByteArray.resize(pvCursor + n);
    ::memcpy(pvByteArray.data() + pvCursor, src, n);
    pvCursor += n;
  }

 private:

  Q_DISABLE_COPY(mdtUsbtmcMessage);

  QByteArray & pvByteArray;
  int pvCursor;
};

#endif  // #ifndef MDT_USBTMC_MESSAGE_H
