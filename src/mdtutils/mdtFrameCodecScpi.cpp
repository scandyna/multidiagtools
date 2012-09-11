
#include "mdtFrameCodecScpi.h"
#include "mdtError.h"
#include <cfloat>
#include <QChar>

mdtFrameCodecScpi::mdtFrameCodecScpi()
{
}

mdtFrameCodecScpi::~mdtFrameCodecScpi()
{
}

bool mdtFrameCodecScpi::decodeIdn(const QByteArray &frame)
{
  QVariant value;
  QStringList nodeValues;
  int i;

  // Clear previous results
  pvValues.clear();

  // Store raw data in local QString constainer
  pvAsciiData = frame;

  // Remove white spaces at begin and end
  trim();
  // remove EofSeq
  if(!removeEofSeq()){
    return false;
  }

  // Case of no data
  if(pvAsciiData.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }

  // Build the values list
  nodeValues = pvAsciiData.split(",");
  // Store result in data list
  for(i=0; i<nodeValues.size(); i++){
    pvValues << nodeValues.at(i);
  }

  return true;
}

QVariant mdtFrameCodecScpi::decodeSingleValueInt(QByteArray &frame)
{
  QVariant value;
  bool ok = false;

  // Clear previous results
  pvValues.clear();

  // Store raw data in local QString constainer
  pvAsciiData = frame;

  // Remove white spaces at begin and end
  trim();
  // remove EofSeq
  if(!removeEofSeq()){
    return value;
  }

  // Case of no data
  if(pvAsciiData.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return value;
  }

  // Convert value
  value = pvAsciiData;
  value = value.toInt(&ok);
  if(!ok){
    value.clear();
    return value;
  }
  pvValues << value;

  return value;
}

bool mdtFrameCodecScpi::decodeError(QByteArray &frame)
{
  QVariant value;
  QStringList nodeValues;
  bool ok = false;

  // Clear previous results
  pvValues.clear();

  // Store raw data in local QString constainer
  pvAsciiData = frame;

  // Remove white spaces at begin and end
  trim();
  // remove EofSeq
  if(!removeEofSeq()){
    return false;
  }

  // Case of no data
  if(pvAsciiData.size() < 1){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no data" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }

  // Build the values list
  nodeValues = pvAsciiData.split(",");
  // Store result in data list
  if(nodeValues.size() < 2){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains not an error" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Error code
  value = nodeValues.at(0);
  value = value.toInt(&ok);
  if(!ok){
    value.clear();
  }
  pvValues << value;
  // Error text
  pvValues << nodeValues.at(1);

  return true;
}

bool mdtFrameCodecScpi::decodeIEEEblock(QString data)
{
  int i;
  bool ok = false;
  QString dataLen;

  // If string length is < 2: shure: not an IEEE block
  if(data.size() < 2){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame is not a IEEE block" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // If first char is '#' , simply remove it
  if(data.at(0) == QChar('#')){
    data.remove(0, 1);
  }
  // We must have a number that tells how many info digits that follows
  i = QString(data.at(0)).toInt(&ok); // Note: we need a QString, not a QChar.
  if(!ok){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no block header length" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  data.remove(0, 1);
  // Check that we have enouth chars to decode data length
  if(data.size() < i){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame contains no block header with data length" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Get the data length
  dataLen = data.left(i);
  data.remove(0, i);
  i = dataLen.toInt(&ok);
  if(!ok){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Unable to decode data length in block header" , mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Ok, let's decode the data
  return decodeIEEEdata(data.toAscii());
}
