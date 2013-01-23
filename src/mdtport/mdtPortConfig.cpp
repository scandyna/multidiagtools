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
  pvUseReadTimeoutProtocol = false;
  pvReadTimeout = 5000;
  pvWriteFrameSize = 1024;
  pvWriteQueueSize = 10;
  pvWriteInterframeTime = 0;
  pvWriteMinWaitTime = 0;
  pvWriteTimeout = -1;
  pvFrameType = mdtFrame::FT_RAW;
  pvEndOfFrameSeq.clear();
  pvEndOfFrameSeq.append('\n');
  pvBytePerByteWrite = false;
  pvConnectTimeout = 5000;
  pvConnectMaxTry = 10;
}

void mdtPortConfig::setReadFrameSize(int size)
{
  Q_ASSERT(size >= 0);

  pvReadFrameSize = size;
}

int mdtPortConfig::readFrameSize() const
{
  return pvReadFrameSize;
}

void mdtPortConfig::setReadQueueSize(int size)
{
  Q_ASSERT(size >= 0);

  pvReadQueueSize = size;
}

int mdtPortConfig::readQueueSize() const
{
  return pvReadQueueSize;
}

void mdtPortConfig::setUseReadTimeoutProtocol(bool use)
{
  pvUseReadTimeoutProtocol = use;
}

bool mdtPortConfig::useReadTimeoutProtocol()const
{
  return pvUseReadTimeoutProtocol;
}

void mdtPortConfig::setReadTimeout(int timeout)
{
  pvReadTimeout = timeout;
}

int mdtPortConfig::readTimeout() const
{
  return pvReadTimeout;
}

void mdtPortConfig::setWriteMinWaitTime(int minWaitTime)
{
  pvWriteMinWaitTime = minWaitTime;
}

int mdtPortConfig::writeMinWaitTime() const
{
  return pvWriteMinWaitTime;
}

void mdtPortConfig::setWriteInterframeTime(int time)
{
  pvWriteInterframeTime = time;
}

int mdtPortConfig::writeInterframeTime() const
{
  return pvWriteInterframeTime;
}

void mdtPortConfig::setWriteTimeout(int timeout)
{
  pvWriteTimeout = timeout;
}

int mdtPortConfig::writeTimeout() const
{
  return pvWriteTimeout;
}

void mdtPortConfig::setWriteFrameSize(int size)
{
  Q_ASSERT(size >= 0);

  pvWriteFrameSize = size;
}

int mdtPortConfig::writeFrameSize() const
{
  return pvWriteFrameSize;
}

void mdtPortConfig::setWriteQueueSize(int size)
{
  Q_ASSERT(size >= 0);

  pvWriteQueueSize = size;
}

int mdtPortConfig::writeQueueSize() const
{
  return pvWriteQueueSize;
}

void mdtPortConfig::setFrameType(const mdtFrame::type_t type)
{
  pvFrameType = type;
}

mdtFrame::type_t mdtPortConfig::frameType() const
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

QByteArray mdtPortConfig::endOfFrameSeq() const
{
  return pvEndOfFrameSeq;
}

void mdtPortConfig::setBytePerByteWrite(bool on, int waitTime)
{
  pvBytePerByteWrite = on;
  pvWriteMinWaitTime = waitTime;
}

bool mdtPortConfig::bytePerByteWrite() const
{
  return pvBytePerByteWrite;
}


void mdtPortConfig::setConnectTimeout(int timeout)
{
  pvConnectTimeout = timeout;
}

int mdtPortConfig::connectTimeout() const
{
  return pvConnectTimeout;
}

void mdtPortConfig::setConnectMaxTry(int maxTry)
{
  pvConnectMaxTry = maxTry;
}

int mdtPortConfig::connectMaxTry() const
{
  return pvConnectMaxTry;
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
  if(pvConnectTimeout != other.pvConnectTimeout){
    return false;
  }
  if(pvConnectMaxTry != other.pvConnectMaxTry){
    return false;
  }

  return true;
}
