/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "VehicleTypeClassDataList.h"
#include <algorithm>
#include <iterator>

int Mdt::Railway::VehicleTypeClassDataList::rowOfVehicleTypeName(const QString& name) const
{
  const auto pred = [name](const VehicleTypeClassData & vtc){
    return compareVehicleTypeName(vtc.name(), name);
  };
  const auto it = std::find_if(cbegin(), cend(), pred);
  if(it == cend()){
    return -1;
  }

  return std::distance(cbegin(), it);
}

bool Mdt::Railway::VehicleTypeClassDataList::compareVehicleTypeName(const QString& a, const QString& b)
{
  return (QString::compare(a, b, Qt::CaseInsensitive) == 0);
}
