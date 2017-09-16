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
#include "SingleApplication.h"

namespace Mdt{

SingleApplication::SingleApplication(int& argc, char** argv)
 : QtSingleApplication(argc, argv),
   mImpl( std::make_unique<CoreApplicationImpl>() )
{
}

SingleApplication::~SingleApplication()
{
}

void SingleApplication::enableFileLogging()
{
  mImpl->enableFileLogging(CoreApplicationImpl::ApplicationName);
}

QString SingleApplication::logFilePath()
{
  return mImpl->logFilePath();
}

QString SingleApplication::cacheDirectoryPath()
{
  return CoreApplicationImpl::cacheDirectoryPath();
}

QString SingleApplication::qtVersion()
{
  return CoreApplicationImpl::qtVersion();
}

QString SingleApplication::mdtVersion()
{
  return CoreApplicationImpl::mdtVersion();
}

void SingleApplication::debugEnvironment()
{
  mImpl->debugEnvironment( mImpl->commonEnvironmentEntries() );
}

} // namespace Mdt{
