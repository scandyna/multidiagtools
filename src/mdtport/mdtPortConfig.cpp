
#include "mdtPortConfig.h"

mdtPortConfig::mdtPortConfig()
{
  setDefault();
}

mdtPortConfig::~mdtPortConfig() {
}

void mdtPortConfig::setDefault()
{
  pvReadFrameSize = 1024;
  pvReadQueueSize = 10;
  //pvReadMinWaitTime = 0;
  pvUseReadTimeoutProtocol = false;
  pvReadTimeout = 500;
  pvWriteFrameSize = 1024;
  pvWriteQueueSize = 10;
  pvWriteMinWaitTime = 0;
  pvWriteTimeout = 500;
  pvFrameType = mdtFrame::FT_RAW;
  pvEndOfFrameSeq.clear();
  pvEndOfFrameSeq.append('\n');
  pvBytePerByteWrite = false;
}

void mdtPortConfig::setReadFrameSize(int size)
{
  Q_ASSERT(size >= 0);

  pvReadFrameSize = size;
}

int mdtPortConfig::readFrameSize()
{
  return pvReadFrameSize;
}

void mdtPortConfig::setReadQueueSize(int size)
{
  Q_ASSERT(size >= 0);

  pvReadQueueSize = size;
}

int mdtPortConfig::readQueueSize()
{
  return pvReadQueueSize;
}
/*
void mdtPortConfig::setReadMinWaitTime(int minWaitTime)
{
  pvReadMinWaitTime = minWaitTime;
}
*/
/*
int mdtPortConfig::readMinWaitTime()
{
  return pvReadMinWaitTime;
}
*/
void mdtPortConfig::setUseReadTimeoutProtocol(bool use)
{
  pvUseReadTimeoutProtocol = use;
}

bool mdtPortConfig::useReadTimeoutProtocol()
{
  return pvUseReadTimeoutProtocol;
}

void mdtPortConfig::setReadTimeout(int timeout)
{
  pvReadTimeout = timeout;
}

int mdtPortConfig::readTimeout()
{
  return pvReadTimeout;
}

void mdtPortConfig::setWriteMinWaitTime(int minWaitTime)
{
  pvWriteMinWaitTime = minWaitTime;
}

int mdtPortConfig::writeMinWaitTime()
{
  return pvWriteMinWaitTime;
}

void mdtPortConfig::setWriteTimeout(int timeout)
{
  pvWriteTimeout = timeout;
}

int mdtPortConfig::writeTimeout()
{
  return pvWriteTimeout;
}

void mdtPortConfig::setWriteFrameSize(int size)
{
  Q_ASSERT(size >= 0);

  pvWriteFrameSize = size;
}

int mdtPortConfig::writeFrameSize()
{
  return pvWriteFrameSize;
}

void mdtPortConfig::setWriteQueueSize(int size)
{
  Q_ASSERT(size >= 0);

  pvWriteQueueSize = size;
}

int mdtPortConfig::writeQueueSize()
{
  return pvWriteQueueSize;
}

void mdtPortConfig::setFrameType(const mdtFrame::type_t type)
{
  pvFrameType = type;
}

mdtFrame::type_t mdtPortConfig::frameType()
{
  return pvFrameType;
}

void mdtPortConfig::setEndOfFrameSeq(const QByteArray &seq)
{
  pvEndOfFrameSeq = seq;
}

void mdtPortConfig::setEndOfFrameSeq(char c)
{
  pvEndOfFrameSeq.clear();
  pvEndOfFrameSeq.append(c);
}

QByteArray mdtPortConfig::endOfFrameSeq()
{
  return pvEndOfFrameSeq;
}

void mdtPortConfig::setBytePerByteWrite(bool on, int waitTime)
{
  pvBytePerByteWrite = on;
  pvWriteMinWaitTime = waitTime;
}

bool mdtPortConfig::bytePerByteWrite()
{
  return pvBytePerByteWrite;
}

bool mdtPortConfig::operator==(const mdtPortConfig &other)
{
  return matches(other);
}

bool mdtPortConfig::operator!=(const mdtPortConfig &other)
{
  return !matches(other);
}

bool mdtPortConfig::matches(const mdtPortConfig &other)
{
  if(pvReadFrameSize != other.pvReadFrameSize){
    return false;
  }
  if(pvReadQueueSize != other.pvReadQueueSize){
    return false;
  }
  if(pvWriteFrameSize != other.pvWriteFrameSize){
    return false;
  }
  if(pvWriteQueueSize != other.pvWriteQueueSize){
    return false;
  }
  if(pvFrameType != other.pvFrameType){
    return false;
  }
  if(pvEndOfFrameSeq != other.pvEndOfFrameSeq){
    return false;
  }
  if(pvReadTimeout != other.pvReadTimeout){
    return false;
  }
  if(pvWriteTimeout != other.pvWriteTimeout){
    return false;
  }
  return true;
}
