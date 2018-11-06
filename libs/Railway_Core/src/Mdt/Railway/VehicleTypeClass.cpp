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
#include "VehicleTypeClass.h"
#include "EditVehicleTypeClass.h"

#include <QDebug>

namespace Mdt{ namespace Railway{

VehicleTypeClass::VehicleTypeClass(QObject* parent)
 : QObject(parent)
{
}

void VehicleTypeClass::setRepository(const VehicleTypeClassRepositoryHandle& repositoryHandle)
{
  mRepositoryHandle = repositoryHandle;
}

bool VehicleTypeClass::validateChanges()
{
  Q_ASSERT(!mRepositoryHandle.isNull());

  EditVehicleTypeClass edit;
  auto & repository = mRepositoryHandle.repository();
  const auto rowList = repository.getRowsForUpdatedRecords();
  for(const auto row : rowList){
    qDebug() << "Validating name " << repository.name(row) << " - alias " << repository.alias(row);
    edit.setName(repository.name(row));
    edit.setAlias(repository.alias(row));
    if(!edit.validate()){
      setLastError(edit.lastError());
      return false;
    }
    repository.setName(row, edit.name());
    repository.setAlias(row, edit.alias());
//     repository.recordAt(row) = edit.data();
//     if( !edit.setName(repository.name(row)) ){
//       setLastError(edit.lastError());
//       return false;
//     }
//     if( !edit.setAlias(repository.alias(row)) ){
//       setLastError(edit.lastError());
//       return false;
//     }
  }

  return true;
}

bool VehicleTypeClass::submitChanges()
{
  Q_ASSERT(!mRepositoryHandle.isNull());

  if(!validateChanges()){
    return false;
  }
  auto & repository = mRepositoryHandle.repository();
  if(!repository.submitChanges()){
    setLastError(repository.lastError());
    return false;
  }

  return true;
}

void VehicleTypeClass::setLastError(const Error& error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace Railway{
