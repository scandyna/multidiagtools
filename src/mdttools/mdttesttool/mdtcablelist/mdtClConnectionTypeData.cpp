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
#include "mdtClConnectionTypeData.h"

/*
 * Some key data implemntations
 */
mdtClConnectionType_t mdtClConnectionTypeKeyData::type() const
{
  QString _code = code.toString();

  if(_code == "T"){
    return mdtClConnectionType_t::Terminal;
  }else if(_code == "P"){
    return mdtClConnectionType_t::Pin;
  }else if(_code == "S"){
    return mdtClConnectionType_t::Socket;
  }else{
    return mdtClConnectionType_t::Undefined;
  }
}

void mdtClConnectionTypeKeyData::setType(mdtClConnectionType_t t)
{
  clear();
  switch(t){
    case mdtClConnectionType_t::Terminal:
      code = "T";
      break;
    case mdtClConnectionType_t::Pin:
      code = "P";
      break;
    case mdtClConnectionType_t::Socket:
      code = "S";
      break;
    case mdtClConnectionType_t::Undefined:
      // Let it null
      break;
  }
}

/*
 * Some connection data implementations
 */

void mdtClConnectionTypeData::setType(mdtClConnectionType_t t)
{
  clear();
  keyData.setType(t);
}

QString mdtClConnectionTypeData::name(const QLocale & locale) const
{
  switch(locale.language()){
    case QLocale::French:
      return nameFR.toString();
    case QLocale::German:
      return nameDE.toString();
    case QLocale::Italian:
      return nameIT.toString();
    default:
      return nameEN.toString();
  }
}

void mdtClConnectionTypeData::clear()
{
  keyData.clear();
  nameEN.clear();
  nameFR.clear();
  nameDE.clear();
  nameIT.clear();
}
