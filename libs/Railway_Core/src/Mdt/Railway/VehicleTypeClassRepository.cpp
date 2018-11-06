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
#include "VehicleTypeClassRepository.h"

namespace Mdt{ namespace Railway{

void AbstractVehicleTypeClassRepository::setName(int row, const QString & name)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  recordAt(row).setName(name);
}

QString AbstractVehicleTypeClassRepository::name(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return constRecordAt(row).name();
}

void AbstractVehicleTypeClassRepository::setAlias(int row, const QString & alias)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  recordAt(row).setAlias(alias);
}

QString AbstractVehicleTypeClassRepository::alias(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return constRecordAt(row).alias();
}

}} // namespace Mdt{ namespace Railway{
