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
#include "FieldTypeName.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{

FieldType FieldTypeName::typeFromName(const QString & name)
{
  if(name.isEmpty()){
    return FieldType::UnknownType;
  }else if(name == QLatin1String("BOOLEAN")){
    return FieldType::Boolean;
  }else if(name == QLatin1String("INTEGER")){
    return FieldType::Integer;
  }else if(name == QLatin1String("FLOAT")){
    return FieldType::Float;
  }else if(name == QLatin1String("DOUBLE")){
    return FieldType::Double;
  }else if(name == QLatin1String("VARCHAR")){
    return FieldType::Varchar;
  }else if(name == QLatin1String("DATE")){
    return FieldType::Date;
  }else if(name == QLatin1String("TIME")){
    return FieldType::Time;
  }else if(name == QLatin1String("DATETIME")){
    return FieldType::DateTime;
  }else{
    return FieldType::UnknownType;
  }
}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
