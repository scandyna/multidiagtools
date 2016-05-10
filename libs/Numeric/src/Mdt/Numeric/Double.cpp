/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "Double.h"
#include <QDebug>

namespace Mdt{ namespace Numeric{

}} // namespace Mdt{ namespace Numeric{

QDebug operator<<(QDebug dbg, const Mdt::Numeric::Double & x)
{
  if(x.isNull()){
    return dbg.nospace() << "Double(Null)";
  }
  if(x.isMinusInfinity()){
    return dbg.nospace() << "Double(-inf)";
  }
  if(x.isPlusInfinity()){
    return dbg.nospace() << "Double(+inf)";
  }
  return dbg.nospace() << "Double(" << x.toDouble() << ")";
}

