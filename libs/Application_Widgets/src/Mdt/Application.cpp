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
#include "Application.h"
#include "ApplicationImpl.h"

namespace Mdt{

Application::Application(int & argc, char **argv)
 : QApplication(argc, argv),
   mImpl( std::make_unique<ApplicationImpl>() )
{
}

Application::~Application()
{
}

void Application::enableFileLogging()
{
  mImpl->enableFileLogging(ApplicationImpl::ApplicationNameAndPid);
}

QString Application::logFilePath()
{
  return mImpl->logFilePath();
}

QString Application::cacheDirectoryPath()
{
  return ApplicationImpl::cacheDirectoryPath();
}

QString Application::qtVersion()
{
  return ApplicationImpl::qtVersion();
}

QString Application::mdtVersion()
{
  return ApplicationImpl::mdtVersion();
}

void Application::debugEnvironment()
{
  mImpl->debugEnvironment( mImpl->environmentEntries() );
}

} // namespace Mdt{
