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
#include "ForeignKeyActionSqlTransform.h"
#include <QLatin1String>

namespace Mdt{ namespace Sql{ namespace Schema{

QString ForeignKeyActionSqlTransform::getSql(ForeignKeyAction action)
{
  switch(action){
    case ForeignKeyAction::NoAction:
      return QLatin1String("NO ACTION");
    case ForeignKeyAction::Restrict:
      return QLatin1String("RESTRICT");
    case ForeignKeyAction::Cascade:
      return QLatin1String("CASCADE");
    case ForeignKeyAction::SetNull:
      return QLatin1String("SET NULL");
    case ForeignKeyAction::SetDefault:
      return QLatin1String("SET DEFAULT");
  }
  return QString();
}

ForeignKeyAction ForeignKeyActionSqlTransform::fromActionString(const QString& actionStr)
{
  const QString str = actionStr.simplified().toUpper();

  if(str == QLatin1String("NO ACTION")){
    return ForeignKeyAction::NoAction;
  }else if(str == QLatin1String("RESTRICT")){
    return ForeignKeyAction::Restrict;
  }else if(str == QLatin1String("SET NULL")){
    return ForeignKeyAction::SetNull;
  }else if(str == QLatin1String("SET DEFAULT")){
    return ForeignKeyAction::SetDefault;
  }else if(str == QLatin1String("CASCADE")){
    return ForeignKeyAction::Cascade;
  }else{
    return ForeignKeyAction::NoAction;
  }
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
