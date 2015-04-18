/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtTtValueLimits.h"
#include <limits>

mdtTtValueLimits::mdtTtValueLimits()
/// : pvResult(Result_t::Undefined)
{
}

void mdtTtValueLimits::clear()
{
  pvLeftBottomLimit.clear();
  pvLeftTopLimit.clear();
  pvRightBottomLimit.clear();
  pvRightTopLimit.clear();
}

void mdtTtValueLimits::setLimits(const mdtValueDouble & leftBottomLimit, const mdtValueDouble & leftTopLimit, const mdtValueDouble & rightBottomLimit, const mdtValueDouble & rightTopLimit)
{
  clear();
  setLeftBottomLimit(leftBottomLimit);
  setLeftTopLimit(leftTopLimit);
  setRightBottomLimit(rightBottomLimit);
  setRightTopLimit(rightTopLimit);
}

void mdtTtValueLimits::setLeftBottomLimit(const mdtValueDouble & x)
{
  Q_ASSERT( (x.isNull()) || (pvLeftTopLimit.isNull()) || (x <= pvLeftTopLimit) );
  pvLeftBottomLimit = x;
}

void mdtTtValueLimits::setLeftBottomLimitVar(const QVariant & v)
{
  if(v.isNull()){
    setLeftBottomLimit(mdtValueDouble());
  }else{
    setLeftBottomLimit(v.toDouble());
  }
}

void mdtTtValueLimits::setLeftBottomLimitToMinusInfinity()
{
  setLeftBottomLimit(-std::numeric_limits<double>::infinity());
}

void mdtTtValueLimits::setLeftTopLimit(const mdtValueDouble & x)
{
  Q_ASSERT( (x.isNull()) || (pvLeftBottomLimit.isNull()) ||  (x >= pvLeftBottomLimit) );
  Q_ASSERT( (x.isNull()) || (pvRightBottomLimit.isNull()) ||  (x <= pvRightBottomLimit) );
  pvLeftTopLimit = x;
}

void mdtTtValueLimits::setLeftTopLimitVar(const QVariant & v)
{
  if(v.isNull()){
    setLeftTopLimit(mdtValueDouble());
  }else{
    setLeftTopLimit(v.toDouble());
  }
}

void mdtTtValueLimits::setLeftTopLimitToMinusInfinity()
{
  Q_ASSERT( (pvLeftBottomLimit.isNull()) || (pvLeftBottomLimit.isMinusInfinity()));
  setLeftTopLimit(-std::numeric_limits<double>::infinity());
}

void mdtTtValueLimits::setRightBottomLimit(const mdtValueDouble & x)
{
  Q_ASSERT( (x.isNull()) || (pvLeftTopLimit.isNull()) ||  (x >= pvLeftTopLimit) );
  Q_ASSERT( (x.isNull()) || (pvRightTopLimit.isNull()) ||  (x <= pvRightTopLimit) );
  pvRightBottomLimit = x;
}

void mdtTtValueLimits::setRightBottomLimitVar(const QVariant & v)
{
  if(v.isNull()){
    setRightBottomLimit(mdtValueDouble());
  }else{
    setRightBottomLimit(v.toDouble());
  }
}

void mdtTtValueLimits::setRightBottomLimitToPlusInfinity()
{
  Q_ASSERT( (pvRightTopLimit.isNull()) || (pvRightTopLimit.isPlusInfinity()) );
  setRightBottomLimit(std::numeric_limits<double>::infinity());
}

void mdtTtValueLimits::setRightTopLimit(const mdtValueDouble & x)
{
  Q_ASSERT( (x.isNull()) || (pvRightBottomLimit.isNull()) ||  (x >= pvRightBottomLimit) );
  pvRightTopLimit = x;
}

void mdtTtValueLimits::setRightTopLimitVar(const QVariant & v)
{
  if(v.isNull()){
    setRightTopLimit(mdtValueDouble());
  }else{
    setRightTopLimit(v.toDouble());
  }
}

void mdtTtValueLimits::setRightTopLimitToPlusInfinity()
{
  setRightTopLimit(std::numeric_limits<double>::infinity());
}

bool mdtTtValueLimits::isValid(const mdtValueDouble & x) const
{
  Q_ASSERT(!x.isNull());

  // If one limit is null, limits are not valid
  if(!isSet()){
    return false;
  }
  // For left and right limits, if bottom > top, limits are not valid
  if(pvLeftBottomLimit > pvLeftTopLimit){
    return false;
  }
  if(pvRightBottomLimit > pvRightTopLimit){
    return false;
  }
  // If a right limit is <= a left limit, limits are not valid
  if(pvRightBottomLimit <= pvLeftTopLimit){
    return false;
  }
  // If one of the left limit > x, limits are not valid
  if( (pvLeftBottomLimit > x) || (pvLeftTopLimit > x) ){
    return false;
  }
  // If one of the right limit < x, limits are not valid
  if( (pvRightBottomLimit < x) || (pvRightTopLimit < x) ){
    return false;
  }

  return true;
}

  /*! \brief Get result for given value x
   *
   * If limits are not completely set ( see isValid()  ),
   *  result will be Undefined.
   *
   * If x is in Ok zone, i.e. x in [#leftTopLimit;#rightBottomLimit],
   *  result will be Ok.
   *  If x is in left limit range, i.e. x in [#leftBottomLimit;#leftTopLimit[,
   *  or x is in right limit range, i.e. x in ]#rightBottomLimit;#rightTopLimit],
   *  result will be Limit.
   *
   * For all other caeses, result will be Fail.
   *
   * \pre x must not be null
   */

mdtTtValueLimits::Result_t mdtTtValueLimits::getResult(const mdtValueDouble& x) const
{
  Q_ASSERT(!x.isNull());

  // If limits are not completely set, result is Undefined
  if(!isSet()){
    return Result_t::Undefined;
  }
  // Check about Ok zone
  if( (x >= pvLeftTopLimit) && (x <= pvRightBottomLimit) ){
    return Result_t::Ok;
  }
  // Check about limit ranges
  if( ( (x >= pvLeftBottomLimit)&&(x < pvLeftTopLimit) ) || ( (x > pvRightBottomLimit)&&(x <= pvRightTopLimit) ) ){
    return Result_t::Limit;
  }

  return Result_t::Fail;
}
