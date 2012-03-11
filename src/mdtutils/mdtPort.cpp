
#include "mdtPort.h"
#include "mdtFrame.h"
#include "mdtFrameAscii.h"

mdtPort::mdtPort(QObject *parent)
 : mdtPortSys(parent)
{
  // Set default timeouts
  setReadTimeout(500);
  setWriteTimeout(500);
}

mdtPort::~mdtPort()
{
  this->close();
}

bool mdtPort::open(mdtPortConfig &cfg)
{
  mdtFrame *frame;

  lockMutex();
  if(!mdtPortSys::open(cfg)){
    unlockMutex();
    return false;
  }
  // Create the read frames pools with requested type
  for(int i=0; i<cfg.readQueueSize(); i++){
    if(cfg.frameType() == mdtFrame::mdtFrameTypeAscii){
      frame = new mdtFrameAscii;
      static_cast<mdtFrameAscii*>(frame)->setEofSeq(cfg.endOfFrameSeq());
    }else{
      frame = new mdtFrame;
    }
    Q_ASSERT(frame != 0);
    frame->reserve(cfg.readFrameSize());
    pvReadFramesPool.enqueue(frame);
  }
  // Create the write frames pools
  for(int i=0; i<cfg.writeQueueSize(); i++){
    frame = new mdtFrame;
    Q_ASSERT(frame != 0);
    frame->reserve(cfg.writeFrameSize());
    pvWriteFramesPool.enqueue(frame);
  }
  unlockMutex();
  pvConfig = cfg;

  return true;
}

void mdtPort::close()
{
  // Close device
  mdtPortSys::close();
  // Delete queues
  qDeleteAll(pvReadFramesPool);
  pvReadFramesPool.clear();
  qDeleteAll(pvReadenFrames);
  pvReadenFrames.clear();
  qDeleteAll(pvWriteFramesPool);
  pvWriteFramesPool.clear();
  qDeleteAll(pvWriteFrames);
  pvWriteFrames.clear();
}
