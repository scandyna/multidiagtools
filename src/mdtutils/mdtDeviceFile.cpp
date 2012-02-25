
#include "mdtDeviceFile.h"
#include "mdtFrame.h"
#include "mdtFrameAscii.h"

mdtDeviceFile::mdtDeviceFile(QObject *parent)
 : mdtDeviceFileSys(parent)
{
  // Set default timeouts
  setReadTimeout(500);
  setWriteTimeout(500);
}

mdtDeviceFile::~mdtDeviceFile()
{
  this->close();
}

bool mdtDeviceFile::open(mdtDeviceFileConfig &cfg)
{
  mdtFrame *frame;

  lockMutex();
  if(!mdtDeviceFileSys::open(cfg)){
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

  return true;
}

void mdtDeviceFile::close()
{
  // Close device
  mdtDeviceFileSys::close();
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
