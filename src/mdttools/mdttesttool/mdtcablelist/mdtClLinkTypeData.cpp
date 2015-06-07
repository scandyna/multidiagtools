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
#include "mdtClLinkTypeData.h"

mdtClLinkType_t mdtClLinkTypeKeyData::type() const
{
  QString _code = code.toString();

  if(_code == "CABLELINK"){
    return mdtClLinkType_t::CableLink;
  }else if(_code == "INTERNLINK"){
    return mdtClLinkType_t::InternalLink;
  }else if(_code == "CONNECTION"){
    return mdtClLinkType_t::Connection;
  }else if(_code == "TESTLINK"){
    return mdtClLinkType_t::TestLink;
  }else{
    return mdtClLinkType_t::Undefined;
  }
}

void mdtClLinkTypeKeyData::setType(mdtClLinkType_t t)
{
  switch(t){
    case mdtClLinkType_t::CableLink:
      code = "CABLELINK";
      break;
    case mdtClLinkType_t::InternalLink:
      code = "INTERNLINK";
      break;
    case mdtClLinkType_t::Connection:
      code = "CONNECTION";
      break;
    case mdtClLinkType_t::TestLink:
      code = "TESTLINK";
      break;
    case mdtClLinkType_t::Undefined:
      code.clear();
      break;
  }
}
