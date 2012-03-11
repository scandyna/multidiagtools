#ifndef _MDTFRAMECODECSCPIU3606A_H
#define _MDTFRAMECODECSCPIU3606A_H


#include "mdtFrameCodecScpi.h"

class mdtFrameCodecScpiU3606A : public mdtFrameCodecScpi {
  private:
    mesure_type_t pvMeasureType;

    QVariant pvRange;

    QVariant pvResolution;


  public:
    bool decodeConfFunc(const QByteArray & frame);

    measure_type_t measureType();

    QVariant range();

    QVariant resolution();


  private:
    bool decodeNodeValues(const QString & data);

    QVariant convertToDouble(const QVariant & value);

};
#endif
