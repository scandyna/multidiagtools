/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "BinaryDependencies.h"
#include "BinaryDependenciesImplementationInterface.h"
#include "BinaryDependenciesLdd.h"
#include "BinaryDependenciesObjdump.h"

namespace Mdt{ namespace DeployUtils{

BinaryDependencies::BinaryDependencies(OperatingSystem targetOperatingSystem, QObject* parent)
 : QObject(parent)
{
//   Platform nativePlatform;

//   if(nativePlatform.operatingSystem() == OperatingSystem::Linux){
//     mImpl.reset(new BinaryDependenciesLdd);
//   }
  if(targetOperatingSystem == OperatingSystem::Linux){
    mImpl.reset(new BinaryDependenciesLdd);
  }else if(targetOperatingSystem == OperatingSystem::Windows){
    mImpl.reset(new BinaryDependenciesObjdump);
  }
  if(!mImpl){
    const QString msg = tr("Could not find a implementation for target operating system.");
    mLastError = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
//     mLastError.commit();
  }
}

BinaryDependencies::~BinaryDependencies()
{
}

bool BinaryDependencies::isValid() const
{
  return (mImpl.get() != nullptr);
}

bool BinaryDependencies::findDependencies(const QString& binaryFilePath)
{
  Q_ASSERT(isValid());

  if( !mImpl->findDependencies(binaryFilePath) ){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

LibraryInfoList BinaryDependencies::dependencies() const
{
  Q_ASSERT(isValid());

  return mImpl->dependencies();
}


}} // namespace Mdt{ namespace DeployUtils{
