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
#include "mdtFrame.h"
#include <cstring>

mdtFrame::mdtFrame()
{
  pvEOFcondition = false;
  pvIgnoreNullValues = false;
  pvDirectlyComplete = false;
  pvWaitAnAnswer = false;
}

mdtFrame::~mdtFrame()
{
}

void mdtFrame::setIgnoreNullValues(bool ignoreNullValues)
{
  pvIgnoreNullValues = ignoreNullValues;
}

int mdtFrame::putUntil(const char *data, char token, int maxLen)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(data != QByteArray::data());

  // Get max size that can be stored
  if(maxLen > remainCapacity()){
    maxLen = remainCapacity();
  }
  // Init cursors
  char *cursor = (char*)data;
  char *end = cursor + maxLen;
  int stored = 0;

  // If EOF condition was reached, or frame is full, we do nothing
  if(isComplete()||isFull()){
    return 0;
  }

  // We not want to check ignoreNullValues each byte, so we make 2 loops
  if(pvIgnoreNullValues){
    while(cursor < end){
      if(*cursor == token){
        pvEOFcondition = true;
        return stored;
      }
      if(*cursor != 0){
        append(*cursor);
        ++stored;
      }
      ++cursor;
    }
  }else{
    while(cursor < end){
      if(*cursor == token){
        pvEOFcondition = true;
        return (cursor - data);
      }
      append(*cursor);
      ++cursor;
      ++stored;
    }
  }

  return stored;
}

int mdtFrame::putUntil(const char *data, QByteArray &token, int maxLen)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(data != QByteArray::data());

  // Get max size that can be stored
  if(maxLen > remainCapacity()){
    maxLen = remainCapacity();
  }
  // Init cursors
  char *cursor = (char*)data;
  char *end = cursor + maxLen;
  int stored = 0;
  int cmpLen = token.size();

  // If EOF condition was reached, or frame is full, we do nothing
  if(isComplete()||isFull()){
    return 0;
  }

  // We not want to check ignoreNullValues each byte, so we make 2 loops
  if(pvIgnoreNullValues){
    while(cursor < end){
      if((end-cursor) >= cmpLen){
        if(memcmp(cursor, token.data(), cmpLen) == 0){
          pvEOFcondition = true;
          return stored;
        }
      }
      if(*cursor != 0){
        append(*cursor);
        ++stored;
      }
      ++cursor;
    }
  }else{
    while(cursor < end){
      if((end-cursor) >= cmpLen){
        if(memcmp(cursor, token.data(), cmpLen) == 0){
          pvEOFcondition = true;
          return stored;
        }
      }
      append(*cursor);
      ++cursor;
      ++stored;
    }
  }

  return stored;
}

bool mdtFrame::isFull()
{
  return remainCapacity() == 0;
}

int mdtFrame::remainCapacity()
{
  int remain;

  remain = capacity() - size();
  if(remain < 0){
    return 0;
  }
  return remain;
}

int mdtFrame::bytesToStore()
{
  if(pvEOFcondition){
    return 0;
  }
  return remainCapacity();
}

void mdtFrame::setDirectlyComplete(bool dc)
{
  pvDirectlyComplete = dc;
}

void mdtFrame::setComplete()
{
  pvEOFcondition = true;
}

bool mdtFrame::isComplete()
{
  return pvEOFcondition;
}

void mdtFrame::clear()
{
  int capa = capacity();

  QByteArray::clear();
  reserve(capa);
  pvEOFcondition = false;
}

void mdtFrame::clearFlags()
{
}

int mdtFrame::putData(const char *data, int len)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(data != QByteArray::data());

  // Get max size that can be stored and store
  if(len > remainCapacity()){
    len = remainCapacity();
  }
  append(data, len);
  if(pvDirectlyComplete){
    pvEOFcondition = true;
  }

  return len;
}

int mdtFrame::eofSeqLen()
{
  return 0;
}

int mdtFrame::take(char *data, int len)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(data != QByteArray::data());

  int capa = capacity();

  // Set the len that is possible to take
  if(len > size()){
    len = size();
  }
  memcpy(data, QByteArray::data(), len*sizeof(char));
  remove(0, len);
  reserve(capa);

  return len;
}

int mdtFrame::take(int len)
{
  int capa = capacity();

  // Set the len that is possible to take
  if(len > size()){
    len = size();
  }
  remove(0, len);
  reserve(capa);

  return len;
}

void mdtFrame::setWaitAnAnswer(bool wait)
{
  pvWaitAnAnswer = wait;
}

bool mdtFrame::waitAnAnswer() const
{
  return pvWaitAnAnswer;
}
