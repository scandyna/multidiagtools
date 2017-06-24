/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_PLAIN_TEXT_FILE_INPUT_ITERATOR_SHARED_DATA_H
#define MDT_PLAIN_TEXT_FILE_INPUT_ITERATOR_SHARED_DATA_H

#include "Mdt/Error.h"
#include <QIODevice>
#include <QString>
#include <QByteArray>
#include <QPointer>
#include <vector>

class QTextCodec;
class QTextDecoder;

namespace Mdt{ namespace PlainText{

  /*! \brief Contains shared part of FileInputIterator
   */
  class FileInputIteratorSharedData
  {
   public:

    /*! \brief Default constructor
     */
    FileInputIteratorSharedData(int rawDataBufferCapacity = 1024);

    /*! \brief Copy is disabled
     */
    FileInputIteratorSharedData(const FileInputIteratorSharedData &) = delete;

    /*! \brief Assignement is disabled
     */
    FileInputIteratorSharedData & operator=(const FileInputIteratorSharedData &) = delete;

    /*! \brief Move is disabled
     */
    FileInputIteratorSharedData(const FileInputIteratorSharedData &&) = delete;

    /*! \brief Assignement is disabled
     */
    FileInputIteratorSharedData & operator=(FileInputIteratorSharedData &&) = delete;

    /*! \brief Destructor
     */
    ~FileInputIteratorSharedData();

    /*! \brief Set source
     *
     * First, it is checked that device is open.
     *  Then, a text decoder is created, based on given encoding.
     *  If above operations succeeds, and device is not at end, a first chunk of data
     *  is read and decoded into unicode buffer.
     *
     * \param device I/O device on witch data will be read.
     *               Note: device is not owned here (will not be deleted)
     * \param encoding Encoding name of the data that device will read.
     *               Will use QTextCodec::codecForName() to get apropriate codec.
     * \return false if device is not open, or no codec could be found for given encoding,
     *         true if all goes well.
     *
     * \pre device must be a valid pointer
     * \pre device must readable (QIODevice::isReadable() must return true)
     */
    bool setSource(QIODevice *device, const QByteArray & encoding);

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
      return (mCurrentPos == mEnd);
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
    bool advance();

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
      Q_ASSERT(mCurrentPos != mEnd);
      return *mCurrentPos;
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

 private:

    /*! \brief Read a chunck of data from I/O device
     */
    bool readMore();

    /*! \brief Clear unicode buffer
     *
     * Will also reset iterators
     */
    void clearUnicodeBuffer();

//     /*! \brief Translate (calls QObject::tr() )
//      */
//     QString tr(const char *sourceText);

    QString::const_iterator mCurrentPos, mEnd;
    std::vector<char> mRawDataBuffer;
    QString mUnicodeBuffer;
    QTextDecoder *mDecoder;
    QPointer<QIODevice> mDevice;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_FILE_INPUT_ITERATOR_SHARED_DATA_H
