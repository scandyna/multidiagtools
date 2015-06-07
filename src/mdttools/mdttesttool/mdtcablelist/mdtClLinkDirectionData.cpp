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
#include "mdtClLinkDirectionData.h"

mdtClLinkDirection_t mdtClLinkDirectionKeyData::direction() const
{
  QString _code = code.toString();

  if(_code == "BID"){
    return mdtClLinkDirection_t::Bidirectional;
  }else if(_code == "STE"){
    return mdtClLinkDirection_t::StartToEnd;
  }else if(_code == "ETS"){
    return mdtClLinkDirection_t::EndToStart;
  }else{
    return mdtClLinkDirection_t::Undefined;
  }
}

void mdtClLinkDirectionKeyData::setDirection(mdtClLinkDirection_t d)
{
  switch(d){
    case mdtClLinkDirection_t::Bidirectional:
      code = "BID";
      break;
    case mdtClLinkDirection_t::StartToEnd:
      code = "STE";
      break;
    case mdtClLinkDirection_t::EndToStart:
      code = "ETS";
      break;
    case mdtClLinkDirection_t::Undefined:
      code.clear();
      break;
  }
}
