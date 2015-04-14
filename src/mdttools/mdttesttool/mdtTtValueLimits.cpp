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
#include "mdtValue.h"

mdtTtValueLimits::mdtTtValueLimits()
 : pvResult(Result_t::Undefined)
{

}

void mdtTtValueLimits::setLeftBottomLimit(double x)
{
  Q_ASSERT(mdtValueDouble(x) <= mdtValueDouble(pvLeftLimitRange.top));
  pvLeftLimitRange.bottom = x;
}

void mdtTtValueLimits::setLeftTopLimit(double x)
{
  Q_ASSERT(mdtValueDouble(x) >= mdtValueDouble(pvLeftLimitRange.bottom));
  Q_ASSERT(mdtValueDouble(x) <= mdtValueDouble(pvRightLimitRange.bottom));
  pvLeftLimitRange.top = x;
}

void mdtTtValueLimits::setRightBottomLimit(double x)
{
  Q_ASSERT(mdtValueDouble(x) >= mdtValueDouble(pvLeftLimitRange.top));
  Q_ASSERT(mdtValueDouble(x) <= mdtValueDouble(pvRightLimitRange.top));
  pvRightLimitRange.bottom = x;
}

void mdtTtValueLimits::setRightTopLimit(double x)
{
  Q_ASSERT(mdtValueDouble(x) >= mdtValueDouble(pvRightLimitRange.bottom));
  pvRightLimitRange.top = x;
}
