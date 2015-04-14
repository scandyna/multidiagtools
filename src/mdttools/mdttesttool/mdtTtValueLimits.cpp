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
///#include "mdtValue.h"

mdtTtValueLimits::mdtTtValueLimits()
 : pvResult(Result_t::Undefined)
{

}

void mdtTtValueLimits::setLeftBottomLimit(const mdtValueDouble & x)
{
  ///Q_ASSERT(mdtValueDouble(x) <= mdtValueDouble(pvLeftLimitRange.top));
  Q_ASSERT( (x.isNull()) || (pvLeftBottomLimit.isNull()) || (x <= pvLeftBottomLimit) );
  ///pvLeftLimitRange.bottom = x;
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

void mdtTtValueLimits::setLeftTopLimit(const mdtValueDouble & x)
{
  ///Q_ASSERT(mdtValueDouble(x) >= mdtValueDouble(pvLeftLimitRange.bottom));
  ///Q_ASSERT(mdtValueDouble(x) <= mdtValueDouble(pvRightLimitRange.bottom));
  Q_ASSERT( (x.isNull()) || (pvLeftBottomLimit.isNull()) ||  (x >= pvLeftBottomLimit) );
  Q_ASSERT( (x.isNull()) || (pvRightBottomLimit.isNull()) ||  (x <= pvRightBottomLimit) );
  ///pvLeftLimitRange.top = x;
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

void mdtTtValueLimits::setRightBottomLimit(const mdtValueDouble & x)
{
  ///Q_ASSERT(mdtValueDouble(x) >= mdtValueDouble(pvLeftLimitRange.top));
  ///Q_ASSERT(mdtValueDouble(x) <= mdtValueDouble(pvRightLimitRange.top));
  Q_ASSERT( (x.isNull()) || (pvLeftTopLimit.isNull()) ||  (x >= pvLeftTopLimit) );
  Q_ASSERT( (x.isNull()) || (pvRightTopLimit.isNull()) ||  (x <= pvRightTopLimit) );
  ///pvRightLimitRange.bottom = x;
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

void mdtTtValueLimits::setRightTopLimit(const mdtValueDouble & x)
{
  ///Q_ASSERT(mdtValueDouble(x) >= mdtValueDouble(pvRightLimitRange.bottom));
  Q_ASSERT( (x.isNull()) || (pvRightBottomLimit.isNull()) ||  (x >= pvRightBottomLimit) );
  ///pvRightLimitRange.top = x;
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
