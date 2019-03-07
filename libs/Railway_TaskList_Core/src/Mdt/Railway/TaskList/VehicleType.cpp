/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "VehicleType.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

VehicleType::VehicleType::VehicleType(const QString& name, const QString& alias, const QString& manufacturerSerie)
 : mDataStruct(name.trimmed(), alias.trimmed(), manufacturerSerie.trimmed())
{
  Q_ASSERT(!mDataStruct.name.isEmpty());
  Q_ASSERT(!mDataStruct.alias.isEmpty());
  Q_ASSERT(!mDataStruct.manufacturerSerie.isEmpty());
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{
