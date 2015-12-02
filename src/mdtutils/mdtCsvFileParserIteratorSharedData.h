/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CSV_FILE_PARSER_ITERATOR_SHARED_DATA_H
#define MDT_CSV_FILE_PARSER_ITERATOR_SHARED_DATA_H

#include "mdtError.h"
#include <QIODevice>
#include <QString>
#include <QByteArray>
#include <QTextCodec>
#include <QTextDecoder>
#include <QObject>
#include <QPointer>
#include <utility>
#include <vector>

//#include <QDebug>

/*! \brief Contains shared part of mdtCsvFileParserIterator
 */
class mdtCsvFileParserIteratorSharedData
{
 public:

  /*! \brief Default constructor
   */
  mdtCsvFileParserIteratorSharedData()
   : pvDecoder(nullptr)
  {
    pvCurrentPos = pvUnicodeBuffer.cbegin();
    pvEnd = pvUnicodeBuffer.cend();
    pvRawDataBuffer.reserve(1024);
  }

  /*! \brief Copy is disabled
   */
  mdtCsvFileParserIteratorSharedData(const mdtCsvFileParserIteratorSharedData &) = delete;

  /*! \brief Assignement is disabled
   */
  mdtCsvFileParserIteratorSharedData & operator=(const mdtCsvFileParserIteratorSharedData &) = delete;

  /*! \brief Move is disabled
   */
  mdtCsvFileParserIteratorSharedData(const mdtCsvFileParserIteratorSharedData &&) = delete;

  /*! \brief Assignement is disabled
   */
  mdtCsvFileParserIteratorSharedData & operator=(mdtCsvFileParserIteratorSharedData &&) = delete;

  /*! \brief Destructor
   */
  ~mdtCsvFileParserIteratorSharedData()
  {
    delete pvDecoder;
  }

  /*! \brief Set source
   *
   * First, device will be open (if not allready open).
   *  Then, a text decoder is created, based on given encoding.
   *  If above operations succeeds, and device is not at end, a first chunk of data
   *  is read and decoded into unicode buffer.
   *
   * \param device I/O device on witch data will be read.
   *               If device is not allready open,
   *                it will be open with QIODevice::ReadOnly.
   *               If device is allready open, it will be kept as is.
   *               Note: device is not owned here (will not be deleted)
   * \param encoding Encoding name of the data that device will read.
   *               Will use QTextCodec::codecForName() to get apropriate codec.
   * \return false if device could not be open, or no codec could be found for given encoding,
   *         true if all goes well.
   *
   * \pre device must be a valid pointer
   * \pre device must not be open with QIODevice::WriteOnly (QIODevice::isReadable() must return true)
   */
  bool setSource(QIODevice *device, const QByteArray & encoding)
  {
    Q_ASSERT(device != nullptr);
    Q_ASSERT((!device->isOpen()) || (device->isReadable()));

    // Delete possibly allocated decoder (it is a nullptr or a valid pointer)
    delete pvDecoder;
    pvDecoder = nullptr;
    // Set possibly referenced divice to nullptr
    pvDevice = nullptr;
    // Clear buffer(s??)
    clearUnicodeBuffer();
    /*
     * Find a codec for requested encodeing
     * Note: we not have to manage codec lifetime,
     *       see QTextCodec::~QTextCodec()
     */
    auto *codec = QTextCodec::codecForName(encoding);
    if(codec == nullptr){
      QString msg = tr("Could not find a codec for requested encodeing '") + encoding + tr("'");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileParserIteratorSharedData");
      pvLastError.commit();
      return false;
    }
    // Allocate our decoder
    Q_ASSERT(codec != nullptr);
    pvDecoder = codec->makeDecoder();
    /*
     * Open device if it is not allready open
     */
    if(!device->isOpen()){
      if(!device->open(QIODevice::ReadOnly)){
        QString msg = tr("Could not open device:\n") \
                    + device->errorString();
        pvLastError.setError(msg, mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileParserIteratorSharedData");
        pvLastError.commit();
        return false;
      }
    }
    pvDevice = device;
    /*
     * If device is allready at end, its not a error,
     * but we do not try to read from it.
     */
    if(device->atEnd()){
      return true;
    }
    /*
     * Try to read and decode the first chunck of data from device
     */
    while(pvCurrentPos == pvEnd){
      if(!readMore()){
        return false;
      }
    }
    Q_ASSERT(pvCurrentPos != pvEnd);

    return true;
  }

  /*! \brief Check if end was reached
   *
   * Returns true when I/O device
   *  is at end, and no more data is available
   *  in internal unicode buffer.
   *
   * If no device was set or was closed,
   *  this function also returns true.
   */
  bool atEnd() const
  {
    if(!pvDevice){
      return true;
    }
    if(!pvDevice->isOpen()){
      return true;
    }
    return ( (pvCurrentPos == pvEnd) && (pvDevice->atEnd()) );
  }

  /*! \brief Advance by one unicode char in buffer
   *
   * If internal unicode buffer has data available,
   *  current position is simply incremented by 1.
   *  If no more data is available any more,
   *  a chunk is readen frome device,
   *  decoded into unicode buffer and current position
   *  set to its beginning.
   *
   * Retuns false if a error occured.
   *
   * \pre This function can only be called once setSource() successfully returned.
   * \sa lastError()
   * \sa get()
   */
  bool advance()
  {
    Q_ASSERT(pvDevice);
    Q_ASSERT(pvDevice->isReadable());
    Q_ASSERT(pvDecoder);

    qDebug() << "advance() ...";
    // Check if we have to read more data
    while(pvCurrentPos == pvEnd){
      if(!readMore()){
        return false;
      }
    }
    // Update result and current position
    Q_ASSERT(pvCurrentPos != pvEnd);
    ++pvCurrentPos;
    qDebug() << " -> Ok, current: " << *pvCurrentPos;

    return true;
  }

  /*! \brief Get current char in unicode buffer
   *
   * Does nothing else than return the char
   *  referenced by current position.
   *  Calling this function multiple
   *  times also returns allways the same char.
   *
   * \pre This function must not be called after atEnd() returns true.
   */
  QChar get() const
  {
    Q_ASSERT(pvCurrentPos != pvEnd);
    return *pvCurrentPos;
  }

  /*! \brief Get a char from unicode buffer
   *
   * If internal unicode buffer has data available,
   *  one char is simply returned.
   *  If no more data is available any more,
   *  a chunk is readen frome device,
   *  decoded into unicode buffer
   *  and one element is returned.
   *
   * Because data can possibly been readen
   *  from device, caller must check that second of retruned pair
   *  is true (false means a failure).
   *
   * Note that this function will not advance
   *  in the stream. In other words, calling it multiple time
   *  will allways return the same char.
   *
   * \pre This function can only be called once setSource() successfully returned.
   * \sa takeOne()
   */
  std::pair<QChar, bool> getOne()
  {
    Q_ASSERT(pvDevice);
    Q_ASSERT(pvDevice->isReadable());
    Q_ASSERT(pvDecoder);

    std::pair<QChar, bool> result;

    // Check if we have to read more data
    while(pvCurrentPos == pvEnd){
      result.second = readMore();
      if(!result.second){
        return result;
      }
    }
    // Update result and current position
    Q_ASSERT(pvCurrentPos != pvEnd);
    result.first = *pvCurrentPos;
    result.second = true;

    return result;
  }

  /*! \brief Take a char from unicode buffer
   *
   * Does the same operation than getOne() ,
   *  but also advances in stream.
   *
   * \pre This function can only be called once setSource() successfully returned.
   * \sa getOne()
   */
  std::pair<QChar, bool> takeOne()
  {
    Q_ASSERT(pvDevice);
    Q_ASSERT(pvDevice->isReadable());
    Q_ASSERT(pvDecoder);

    std::pair<QChar, bool> result = getOne();
    if(result.second){
      ++pvCurrentPos;
    }
    return result;
  }

  /*! \brief Set internal raw data buffer size
   *
   * This function is mainly used for unit testing.
   */
  void setInternalRawDataBufferSize(int size)
  {
    Q_ASSERT(size > 0);
    pvRawDataBuffer.resize(size);
    pvRawDataBuffer.shrink_to_fit();
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Read a chunck of data from I/O device
   */
  bool readMore()
  {
    Q_ASSERT(pvDevice);
    Q_ASSERT(pvDevice->isReadable());
    Q_ASSERT(pvDecoder);
    Q_ASSERT(pvRawDataBuffer.capacity() > 0);

    // Clear unicode buffer (also enshure that iterators are valid)
    clearUnicodeBuffer();
    // Read a chunk of data from device
    auto n = pvDevice->read(pvRawDataBuffer.data(), pvRawDataBuffer.capacity());
    if(n < 0){
      QString msg = tr("Read from device failed:\n") \
                  + pvDevice->errorString();
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileParserIteratorSharedData");
      pvLastError.commit();
      return false;
    }
    // Decode readen data into unicode buffer and update iterators
    pvDecoder->toUnicode(&pvUnicodeBuffer, pvRawDataBuffer.data(), n);
    pvCurrentPos = pvUnicodeBuffer.cbegin();
    pvEnd = pvUnicodeBuffer.cend();

    return true;
  }

  /*! \brief Clear unicode buffer
   *
   * Will also reset iterators
   */
  void clearUnicodeBuffer()
  {
    pvUnicodeBuffer.clear();
    pvCurrentPos = pvUnicodeBuffer.cbegin();
    pvEnd = pvUnicodeBuffer.cend();
  }

  /*! \brief Translate (calls QObject::tr() )
   */
  QString tr(const char *sourceText)
  {
    return QObject::tr(sourceText);
  }

  QString::const_iterator pvCurrentPos, pvEnd;
  std::vector<char> pvRawDataBuffer;
  QString pvUnicodeBuffer;
  QTextDecoder *pvDecoder;
  QPointer<QIODevice> pvDevice;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CSV_FILE_PARSER_ITERATOR_SHARED_DATA_H
