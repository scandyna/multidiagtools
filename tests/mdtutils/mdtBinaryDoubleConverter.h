/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_BINARY_DOUBLE_CONVERTER_H
#define MDT_BINARY_DOUBLE_CONVERTER_H

///#include "mdtError.h"
#include <QtGlobal>
#include <limits>

/*! \brief Helper class for binary <-> double conversions
 *
 * T is the type of binary format (int, int16_t, ...)
 */
template <typename T>
class mdtBinaryDoubleConverter
{
 public:

  /*! \brief Constructor
   *
   * Will set following parameters:
   *  - minimum: -2^(N-1) with N the bits count of T
   *  - maximum: 2^(N-1)-1 with N the bits count of T
   *  - quantization step: 1
   *  - lsbIndex: 0
   *
   * At this state, this class does nothing helpful,
   *  and standard conversions from C/C++ should be used instead.
   *
   *  To use the converter, set conversion parameters with setRange() first.
   */
  mdtBinaryDoubleConverter(bool valueIsSigned = true)
  {
    if(valueIsSigned){
      setRange(-pow(2.0, 8*sizeof(T)-1), pow(2.0, 8*sizeof(T)-1)-1.0, 8*sizeof(T), 0, valueIsSigned, false, 1.0);
    }else{
      setRange(0.0, pow(2.0, 8*sizeof(T))-1.0, 8*sizeof(T), 0, valueIsSigned, false, 1.0);
    }
  }

  /*! \brief Specify the range of values and conversion parameters
   *
   * For some application, binary data can contain a value and some additionnal flags.
   *  In such case, we must tell the converter witch bits contains the value itself.
   *
   * Take example of Wago 750-457 analog module:
   *  - Value + flags are encoded in a 16 bits word
   *  - Value is encoded in 12 bits, from b3 to b14 + 1 sign bit, b15
   *  - Value is scaled from -10 V to 10 V
   *
   * In this example, this method must be called with
   *  min = -10.0 , max = 10.0 , valueBitsCount = 13 , valueLsbIndex = 3 , valueIsSigned = true , scaleFromMinToMax = true , conversionFactor = 1.0 .
   *
   * Here is another example with Wago 750-464 with Pt100 sensor and normal format:
   *  - Value is encoded in 16 bits
   *  - Scaling is allready done by device
   *  - Value represents 0.1°C per digit -> we have to apply a conversion factor of 0.1
   *
   * In this example, this method must be called with
   *  min = -200.0 , max = 850.0 , valueBitsCount = 16 , valueLsbIndex = 0 , valueIsSigned = true , scaleFromMinToMax = false , conversionFactor = 0.1 .
   *
   *
   * \param min Minimum value in double format (f.ex. 0V, or 4mA)
   * \param max Maximum value in double format (f.ex. 10V, or 20mA)
   * \param valueBitsCount Number of bits used for the value, including sign bit.
   * \param valueLsbIndex Index of first bit to use for value extraction.
   * \param valueIsSigned If true, binary value is considered as signed value, i.e. the most significant bit is the sign.
   * \param scaleFromMinToMax If true, binary value will be scaled from min to max regarding LSB.
   *                           LSB is determined from valueBitsCount .
   * \param conversionFactor This factor is applied as follow (min, max scaling is ignored in this example):
   *                          - ValueDouble = binaryValue * LSB * conversionFactor
   *                          - binaryValue = valueDouble / (LSB * conversionFactor)<br>
   *                          LSB is determined from valueBitsCount .
   *
   * \pre max must be > min
   * \pre valueBitsCount must be >= 0
   * \pre valueLsbIndex must be >= 0
   * \pre valueLsbIndex + valueBitsCount must be <= bits count of type T
   */
  void setRange(double min, double max, int valueBitsCount, int valueLsbIndex = 0, bool valueIsSigned = false, bool scaleFromMinToMax = true, double conversionFactor = 1.0)
  {
    Q_ASSERT(max > min);
    Q_ASSERT((qAbs(conversionFactor) - std::numeric_limits<double>::min()) > 0.0);
    Q_ASSERT(valueBitsCount >= 0);
    Q_ASSERT(valueLsbIndex >= 0);
    Q_ASSERT((unsigned int)(valueLsbIndex + valueBitsCount) <= (unsigned int)(8*sizeof(T)));

    int i;

    // Store lsbIndex and setup value mask
    pvValueLsbIndex = valueLsbIndex;
    pvValueMask = 0;
    for(i = 0; i < valueBitsCount; ++i){
      pvValueMask += (1<<i);
    }
    // Set factors + sign mask
    pvValueIsSigned = valueIsSigned;
    pvScaleFromMinToMax = scaleFromMinToMax;
    if(pvScaleFromMinToMax){
      pvQuantizationStep = conversionFactor * (max-min) / (pow(2.0, valueBitsCount)/**-1.0*/);
    }else{
      pvQuantizationStep = conversionFactor;
    }
    pvSignMask = (1 << (valueBitsCount-1) );
    pvInvQuantizationStep = 1.0/pvQuantizationStep;

    // Store min and max
    pvMinimum = min;
    pvMaximum = max;
    
    qDebug() << "min: " << min << ", max: " << max;
    qDebug() << "Q: " << pvQuantizationStep << " , 1/Q: " << pvInvQuantizationStep;
    qDebug() << "Value mask: " << hex << pvValueMask << " , sign mask: " << hex << pvSignMask;
  }

  /*! \brief Get minimum value
   */
  inline double minimum() const { return pvMinimum; }

  /*! \brief Get maximum value
   */
  inline double maximum() const { return pvMaximum; }

  /*! \brief Convert binary data to double value
   */
  double binaryToDouble(T m)
  {
    double x;

    // Extract bits in correct range
    m = m >> pvValueLsbIndex;
    m &= pvValueMask;
    // Apply C2 and negate if sign bit is present
    if((pvValueIsSigned)&&(m & pvSignMask)){
      m = ( (~m) & pvValueMask );  // C1
      ++m;  // C2
      m = -m;
    }
    // Make conversion
    if(pvValueIsSigned){
      x = pvQuantizationStep*(double)m;
    }else{
      if(pvScaleFromMinToMax){
        x = pvQuantizationStep*(double)m + pvMinimum;
      }else{
        x = pvQuantizationStep*(double)m;
      }
    }

    return x;
  }

  /*! \brief Convert double value to binary data
   */
  T doubleToBinary(double x)
  {
    T m;

    qDebug() << "x: " << x;
    
    // We must clip value, else we can have strongly wrong errors (underflow/overflow errors)
    if(x < (pvMinimum + pvQuantizationStep)){
      x = pvMinimum;
    }else if(x > (pvMaximum - pvQuantizationStep)){
      x = pvMaximum - pvQuantizationStep;
    }
    
    qDebug() << "x: " << x;
    
         
    // Make conversion
    if(pvValueIsSigned){
      x = pvInvQuantizationStep * x;
    }else{
      if(pvScaleFromMinToMax){
        x = pvInvQuantizationStep * (x - pvMinimum);
      }else{
        x = pvInvQuantizationStep * x;
      }
    }

     
    qDebug() << "x: " << x;
    
    /**
    if(pvValueIsSigned && (x < 0.0)){
      x = -x;
    }
    qDebug() << "x: " << x << " , 0x" << hex << (T)x;

    x = x * pow(2.0, pvValueLsbIndex);
    
    qDebug() << "x: " << x << " , 0x" << hex << (T)x;
    
    
    return x;
    */
    
    m = x;
    qDebug() << "m: " << m << "  - 0x" << hex << m;
    
    // Apply C2 and negate if value < 0
    if(pvValueIsSigned && (x < 0.0)){
      m = -m;
      m = (~m) & pvValueMask; // C1
      ++m;  // C2
    }
    qDebug() << "m: " << m << "  - 0x" << hex << m;
    m &= pvValueMask;
    m = m << pvValueLsbIndex;
    qDebug() << "m: " << m << "  - 0x" << hex << m;

    return m;
  }

  /*! \brief Get last error
   */
  ///mdtError lastError() const { return pvLastError; }

 private:

  double pvMinimum;
  double pvMaximum;
  double pvQuantizationStep;    // Factor used for binary -> double conversion (D/A conversion)
  double pvInvQuantizationStep; // Factor used for double -> binary conversion (A/D conversion)
  int pvValueLsbIndex;          // LSB index of value part in binary data (because binary data can contain some meta informations)
  T pvValueMask;                // Mask to extract the correct bits that contain value in binary data
  T pvSignMask;                 // Mask to extract the sign bit in binary data
  bool pvValueIsSigned;         // True if binary data represents a signed value
  bool pvScaleFromMinToMax;     
  ///mdtError pvLastError;
};

#endif
