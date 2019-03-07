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
#include "VehicleTypeClassCommonError.h"
#include "Mdt/ErrorCode.h"
#include <QString>

namespace Mdt{ namespace Railway{

bool VehicleTypeClassCommonError::isKnownError(const Mdt::Error & error) const noexcept
{
  if(!error.isErrorType<Mdt::ErrorCode>()){
    return false;
  }
  const auto errorCode = error.error<Mdt::ErrorCode>();
  switch(errorCode){
    case Mdt::ErrorCode::UniqueConstraintError:
      return true;
    default:
      break;
  }

  return false;
}

Mdt::Error VehicleTypeClassCommonError::createError(const VehicleTypeClassData & data, const Mdt::Error & error)
{
  Q_ASSERT(isKnownError(error));
  Q_ASSERT(error.isErrorType<Mdt::ErrorCode>());

  const auto errorCode = error.error<Mdt::ErrorCode>();
  switch(errorCode){
    case Mdt::ErrorCode::UniqueConstraintError:
      return createUniqueConstraintError(data, error);
    default:
      break;
  }

  return Mdt::Error();
}

Mdt::Error VehicleTypeClassCommonError::createUniqueConstraintError(const VehicleTypeClassData & data, const Mdt::Error & error)
{
  const QString msg = tr("A vehicle type class '%1' ('%2') allready exists.")
                      .arg(data.name(), data.alias());
  auto retError = mdtErrorNewQ(msg, Mdt::Error::Warning, this);
  retError.stackError(error);

  return retError;
}

}} // namespace Mdt{ namespace Railway{
