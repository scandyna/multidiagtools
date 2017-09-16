/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "SingleCoreApplication.h"

namespace Mdt{

SingleCoreApplication::SingleCoreApplication(int& argc, char** argv)
 : QtSingleCoreApplication(argc, argv),
   mImpl( std::make_unique<CoreApplicationImpl>() )
{
}

SingleCoreApplication::~SingleCoreApplication()
{
}

void SingleCoreApplication::enableFileLogging()
{
  mImpl->enableFileLogging(CoreApplicationImpl::ApplicationName);
}

QString SingleCoreApplication::logFilePath()
{
  return mImpl->logFilePath();
}

QString SingleCoreApplication::cacheDirectoryPath()
{
  return CoreApplicationImpl::cacheDirectoryPath();
}

QString SingleCoreApplication::qtVersion()
{
  return CoreApplicationImpl::qtVersion();
}

QString SingleCoreApplication::mdtVersion()
{
  return CoreApplicationImpl::mdtVersion();
}

void SingleCoreApplication::debugEnvironment()
{
  mImpl->debugEnvironment( mImpl->commonEnvironmentEntries() );
}

} // namespace Mdt{
