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
#include "FileInputIterator.h"
#include "FileInputIteratorSharedData.h"

namespace Mdt{ namespace PlainText{

FileInputIterator::FileInputIterator(QIODevice* device, const QByteArray& encoding)
 : mShared( std::make_shared<FileInputIteratorSharedData>() )
{
  init(device, encoding);
}

bool FileInputIterator::setSource(QIODevice* device, const QByteArray& encoding)
{
  Q_ASSERT(device != nullptr);

  mErrorOccured = false;
  if(!mShared){
    mShared = std::make_shared<FileInputIteratorSharedData>();
  }

  return init(device, encoding);
}

void FileInputIterator::clear()
{
  mErrorOccured = false;
  mShared.reset();
}

FileInputIterator& FileInputIterator::operator++()
{
  Q_ASSERT(!isEof());
  Q_ASSERT(mShared);

  mErrorOccured = !mShared->advance();

  return *this;
}

FileInputIterator::reference FileInputIterator::operator*()
{
  Q_ASSERT(!isEof());
  if(mErrorOccured){
    mCurrentChar = 0x2BD1;
  }else{
    mCurrentChar = mShared->get().unicode();
  }
  return mCurrentChar;
}

bool FileInputIterator::isEof() const
{
  if(!mShared){
    return true;
  }
  return mShared->atEnd();
}

Mdt::Error FileInputIterator::lastError() const
{
  if(!mShared){
    return Mdt::Error();
  }
  return mShared->lastError();
}

bool FileInputIterator::init(QIODevice* device, const QByteArray& encoding)
{
  Q_ASSERT(device != nullptr);
  Q_ASSERT(mShared);

  mErrorOccured = !mShared->setSource(device, encoding);
  if(mErrorOccured){
    return false;
  }
  // Check if device is allready at end
  if(mShared->atEnd()){
    mShared.reset();
  }

  return true;
}

}} // namespace Mdt{ namespace PlainText{
