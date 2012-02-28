
#include "mdtDeviceFileConfig.h"

mdtDeviceFileConfig::mdtDeviceFileConfig()
{
  setDefault();
}

mdtDeviceFileConfig::~mdtDeviceFileConfig() {
}

void mdtDeviceFileConfig::setDefault()
{
  pvInterface = "";
  pvReadFrameSize = 1024;
  pvReadQueueSize = 10;
  pvReadMinWaitTime = 0;
  pvWriteFrameSize = 1024;
  pvWriteQueueSize = 10;
  pvWriteMinWaitTime = 0;
  pvFrameType = mdtFrame::mdtFrameTypeAscii;
  pvEndOfFrameSeq = '\0';
}

void mdtDeviceFileConfig::setInterface(const QString & interface)
{
  pvInterface = interface;
}

QString mdtDeviceFileConfig::interface()
{
  return pvInterface;
}

void mdtDeviceFileConfig::setReadFrameSize(int size)
{
  Q_ASSERT(size >= 0);

  pvReadFrameSize = size;
}

int mdtDeviceFileConfig::readFrameSize()
{
  return pvReadFrameSize;
}

void mdtDeviceFileConfig::setReadQueueSize(int size)
{
  Q_ASSERT(size >= 0);

  pvReadQueueSize = size;
}

int mdtDeviceFileConfig::readQueueSize()
{
  return pvReadQueueSize;
}

void mdtDeviceFileConfig::setReadMinWaitTime(int minWaitTime)
{
  pvReadMinWaitTime = minWaitTime;
}

int mdtDeviceFileConfig::readMinWaitTime()
{
  return pvReadMinWaitTime;
}

void mdtDeviceFileConfig::setWriteMinWaitTime(int minWaitTime)
{
  pvWriteMinWaitTime = minWaitTime;
}

int mdtDeviceFileConfig::writeMinWaitTime()
{
  return pvWriteMinWaitTime;
}

void mdtDeviceFileConfig::setWriteFrameSize(int size)
{
  Q_ASSERT(size >= 0);

  pvWriteFrameSize = size;
}

int mdtDeviceFileConfig::writeFrameSize()
{
  return pvWriteFrameSize;
}

void mdtDeviceFileConfig::setWriteQueueSize(int size)
{
  Q_ASSERT(size >= 0);

  pvWriteQueueSize = size;
}

int mdtDeviceFileConfig::writeQueueSize()
{
  return pvWriteQueueSize;
}

void mdtDeviceFileConfig::setFrameType(const mdtFrame::type_t type)
{
  pvFrameType = type;
}

mdtFrame::type_t mdtDeviceFileConfig::frameType()
{
  return pvFrameType;
}

void mdtDeviceFileConfig::setEndOfFrameSeq(const QByteArray &seq)
{
  pvEndOfFrameSeq = seq;
}

void mdtDeviceFileConfig::setEndOfFrameSeq(char c)
{
  pvEndOfFrameSeq.clear();
  pvEndOfFrameSeq.append(c);
}

QByteArray mdtDeviceFileConfig::endOfFrameSeq()
{
  return pvEndOfFrameSeq;
}

bool mdtDeviceFileConfig::operator==(const mdtDeviceFileConfig &other)
{
  return matches(other);
}

bool mdtDeviceFileConfig::operator!=(const mdtDeviceFileConfig &other)
{
  return !matches(other);
}

bool mdtDeviceFileConfig::matches(const mdtDeviceFileConfig &other)
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
  return true;
}
