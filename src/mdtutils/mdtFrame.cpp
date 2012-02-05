
#include "mdtFrame.h"
#include <cstring>

mdtFrame::mdtFrame()
{
  pvEOFcondition = false;
}

mdtFrame::~mdtFrame()
{
}

void mdtFrame::putUntil(const char *data, char token, int maxLen, bool ignoreNullValues)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(data != QByteArray::data());

  char *cursor = (char*)data;
  char *end = cursor + maxLen;

  // If EOF condition was reached, or frame is full, we do nothing
  if(isComplete()||isFull()){
    return;
  }

  // We not want to check ignoreNullValues each byte, so we make 2 loops
  if(ignoreNullValues){
    while(cursor < end){
      if(*cursor == token){
        pvEOFcondition = true;
        return;
      }
      if(*cursor != 0){
        append(*cursor);
      }
      ++cursor;
    }
  }else{
    while(cursor < end){
      if(*cursor == token){
        pvEOFcondition = true;
        return;
      }
      append(*cursor);
      ++cursor;
    }
  }
}

void mdtFrame::putUntil(const char *data, QByteArray &token, int maxLen, bool ignoreNullValues)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(data != QByteArray::data());

  char *cursor = (char*)data;
  char *end = cursor + maxLen;
  int cmpLen = token.size();

  // If EOF condition was reached, or frame is full, we do nothing
  if(isComplete()||isFull()){
    return;
  }

  // We not want to check ignoreNullValues each byte, so we make 2 loops
  if(ignoreNullValues){
    while(cursor < end){
      if((end-cursor) >= cmpLen){
        if(memcmp(cursor, token.data(), cmpLen) == 0){
          pvEOFcondition = true;
          return;
        }
      }
      if(*cursor != 0){
        append(*cursor);
      }
      ++cursor;
    }
  }else{
    while(cursor < end){
      if((end-cursor) >= cmpLen){
        if(memcmp(cursor, token.data(), cmpLen) == 0){
          pvEOFcondition = true;
          return;
        }
      }
      append(*cursor);
      ++cursor;
    }
  }
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
