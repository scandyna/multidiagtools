
#include "mdtFrameCodec.h"
#include "mdtError.h"

#include <QDebug>

mdtFrameCodec::mdtFrameCodec()
{
}

mdtFrameCodec::~mdtFrameCodec()
{
}

QList<QVariant> mdtFrameCodec::values()
{
  return pvValues;
}

bool mdtFrameCodec::decodeIEEEdata(QByteArray data)
{
  if((data.size() % 8) != 0){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Data length is not multiple of 8" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodec");
    e.commit();
    return false;
  }

  // Generate a warning because function is not impemented
  mdtError e(MDT_FRAME_DECODE_ERROR, "* Function is not implemented yet, sorry ! *" , mdtError::Warning);
  MDT_ERROR_SET_SRC(e, "mdtFrameCodec");
  e.commit();
  return false;
}
