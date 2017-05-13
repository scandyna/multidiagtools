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
#include "FileInputIteratorSharedData.h"
#include <QObject>
#include <QTextCodec>
#include <QTextDecoder>
#include <utility>

namespace Mdt{ namespace PlainText{

FileInputIteratorSharedData::FileInputIteratorSharedData(int rawDataBufferCapacity)
 : mDecoder(nullptr)
{
  Q_ASSERT(rawDataBufferCapacity > 0);
  mCurrentPos = mUnicodeBuffer.cbegin();
  mEnd = mUnicodeBuffer.cend();
  mRawDataBuffer.reserve(rawDataBufferCapacity);
}

FileInputIteratorSharedData::~FileInputIteratorSharedData()
{
  delete mDecoder;
}

bool FileInputIteratorSharedData::setSource(QIODevice* device, const QByteArray& encoding)
{
  Q_ASSERT(device != nullptr);

  // Delete possibly allocated decoder (it is a nullptr or a valid pointer)
  delete mDecoder;
  mDecoder = nullptr;
  // Set possibly referenced divice to nullptr
  mDevice = nullptr;
  // Clear buffer
  clearUnicodeBuffer();
  /*
    * Find a codec for requested encodeing
    * Note: we not have to manage codec lifetime,
    *       see QTextCodec::~QTextCodec()
    */
  auto *codec = QTextCodec::codecForName(encoding);
  if(codec == nullptr){
    const QString msg = tr("Could not find a codec for requested encoding '%1'").arg( QString::fromLatin1(encoding) );
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "FileInputIteratorSharedData");
    mLastError.commit();
    return false;
  }
  // Allocate our decoder
  Q_ASSERT(codec != nullptr);
  mDecoder = codec->makeDecoder();
  /*
   * Check that device is open
   * We must do it at runtime,
   * because it can be closed at any time
   * (f.ex. a file on a removable device)
   */
  if(!device->isOpen()){
    QString msg = tr("Device is not open.");
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "FileInputIteratorSharedData");
    msg = device->errorString();
    if(!msg.isEmpty()){
      const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "FileInputIteratorSharedData");
      mLastError.stackError(error);
    }
    mLastError.commit();
    return false;
  }
  Q_ASSERT(device->isReadable());
  mDevice = device;
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
  while(mCurrentPos == mEnd){
    if(!readMore()){
      return false;
    }
  }
  Q_ASSERT(mCurrentPos != mEnd);

  return true;
}

bool FileInputIteratorSharedData::advance()
{
  Q_ASSERT(mDevice);
  Q_ASSERT(mDecoder);

  // Check if we have cached data
  if(mCurrentPos != mEnd){
    Q_ASSERT(mCurrentPos < mEnd);
    ++mCurrentPos;
  }
  // Check if we have to read more data
  while(mCurrentPos == mEnd){
    if(!readMore()){
      Q_ASSERT(mCurrentPos == mEnd);
      return false;
    }
    if(mDevice->atEnd()){
      return true;
    }
  }

  return true;
}

bool FileInputIteratorSharedData::readMore()
{
  Q_ASSERT(mDevice);
  Q_ASSERT(mDecoder);
  Q_ASSERT(mRawDataBuffer.capacity() > 0);

  // Clear unicode buffer (also enshure that iterators are valid)
  clearUnicodeBuffer();
  /*
    * Check that device is open
    * We must do it at runtime,
    * because it can be closed at any time
    * (f.ex. a file on a removable device)
    */
  if(!mDevice->isOpen()){
    QString msg = tr("Device was closed.");
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "FileInputIteratorSharedData");
    msg = mDevice->errorString();
    if(!msg.isEmpty()){
      auto error = mdtErrorNew(msg, Mdt::Error::Critical, "FileInputIteratorSharedData");
      mLastError.stackError(error);
    }
    mLastError.commit();
    return false;
  }
  Q_ASSERT(mDevice->isReadable());
  // Read a chunk of data from device
  auto n = mDevice->read(mRawDataBuffer.data(), mRawDataBuffer.capacity());
  if(n < 0){
    QString msg = tr("Read from device failed.");
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "FileInputIteratorSharedData");
    msg = mDevice->errorString();
    if(!msg.isEmpty()){
      auto error = mdtErrorNew(msg, Mdt::Error::Critical, "FileInputIteratorSharedData");
      mLastError.stackError(error);
    }
    mLastError.commit();
    return false;
  }
  // Decode readen data into unicode buffer and update iterators
  mDecoder->toUnicode(&mUnicodeBuffer, mRawDataBuffer.data(), n);
  mCurrentPos = mUnicodeBuffer.cbegin();
  mEnd = mUnicodeBuffer.cend();

  return true;
}

void FileInputIteratorSharedData::clearUnicodeBuffer()
{
  mUnicodeBuffer.clear();
  mCurrentPos = mUnicodeBuffer.cbegin();
  mEnd = mUnicodeBuffer.cend();
}

QString FileInputIteratorSharedData::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}

}} // namespace Mdt{ namespace PlainText{
