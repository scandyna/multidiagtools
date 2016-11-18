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
#include "FieldList.h"

namespace Mdt{ namespace Sql{ namespace Schema{

int FieldList::fieldIndex(const QString& fieldName) const
{
  const auto name = fieldName.toUpper();
  for(int i = 0; i < pvFieldList.size(); ++i){
    if(pvFieldList.at(i).name().toUpper() == name){
      return i;
    }
  }
  return -1;
}

QStringList FieldList::toFieldNameList() const
{
  QStringList list;

  list.reserve(size());
  for(const auto & field : pvFieldList){
    list.append(field.name());
  }

  return list;
}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{