
#include "mdtSerialPort.h"
#include "mdtFrameAscii.h"

mdtSerialPort::mdtSerialPort(QObject *parent)
 : mdtSerialPortSys(parent)
{
  // Set default timeouts
  setRxTimeout(500);
  setTxTimeout(500);
}

mdtSerialPort::~mdtSerialPort()
{
  qDeleteAll(pvRxFramesPool);
  pvRxFramesPool.clear();
  qDeleteAll(pvRxFrames);
  pvRxFrames.clear();
  qDeleteAll(pvTxFramesPool);
  pvTxFramesPool.clear();
  qDeleteAll(pvTxFrames);
  pvTxFrames.clear();
}

bool mdtSerialPort::openPort(mdtSerialPortConfig &cfg)
{
  mdtFrame *frame;

  pvMutex.lock();
  if(!mdtSerialPortSys::openPort(cfg)){
    pvMutex.unlock();
    return false;
  }
  /// NOTE: select correct frame type
  // Create the RX frames pools
  for(int i=0; i<cfg.rxQueueSize(); i++){
    if(cfg.frameType() == mdtFrame::mdtFrameTypeAscii){
      frame = new mdtFrameAscii;
      static_cast<mdtFrameAscii*>(frame)->setEofSeq(cfg.eofSeq());
    }else{
      frame = new mdtFrame;
    }
    Q_ASSERT(frame != 0);
    frame->reserve(cfg.rxFrameSize());
    pvRxFramesPool.enqueue(frame);
  }
  // Create the TX frames pools
  for(int i=0; i<cfg.txQueueSize(); i++){
    frame = new mdtFrame;
    Q_ASSERT(frame != 0);
    frame->reserve(cfg.txFrameSize());
    pvTxFramesPool.enqueue(frame);
  }
  pvMutex.unlock();

  return true;
}

bool mdtSerialPort::rxTimeoutOccured()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvRxTimeoutOccured;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::txTimeoutOccured()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvTxTimeoutOccured;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::carIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCarIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::dsrIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvDsrIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::ctsIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvCtsIsOn;
  pvMutex.unlock();

  return retVal;
}

bool mdtSerialPort::rngIsOn()
{
  bool retVal;

  pvMutex.lock();
  retVal = pvRngIsOn;
  pvMutex.unlock();

  return retVal;
}

void mdtSerialPort::lockMutex()
{
  pvMutex.lock();
}

void mdtSerialPort::unlockMutex()
{
  pvMutex.unlock();
}

void mdtSerialPort::setRts(bool on)
{
  pvMutex.lock();
  mdtSerialPortSys::setRts(on);
  pvMutex.unlock();
}

void mdtSerialPort::setDtr(bool on)
{
  pvMutex.lock();
  mdtSerialPortSys::setDtr(on);
  pvMutex.unlock();
}
