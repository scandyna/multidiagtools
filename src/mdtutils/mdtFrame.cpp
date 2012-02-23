
#include "mdtFrame.h"
#include <cstring>

mdtFrame::mdtFrame()
{
  pvEOFcondition = false;
  pvIgnoreNullValues = false;
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

int mdtFrame::putData(char *data, int len)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(data != QByteArray::data());

  // Get max size that can be stored and store
  if(len > remainCapacity()){
    len = remainCapacity();
  }
  append(data, len);

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
