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
#include "MdtDeployUtilsMain.h"
#include "Mdt/DeployUtils/SettingsCommandLineParser.h"
#include <QCoreApplication>

#include "Mdt/DeployUtils/Library.h"
#include <QDebug>

using namespace Mdt;
using namespace Mdt::DeployUtils;

MdtDeployUtilsMain::MdtDeployUtilsMain(QObject* parent)
 : AbstractConsoleApplicationMainFunction(parent)
{
}

int MdtDeployUtilsMain::runMain()
{
  qDebug() << "Main...";
  
  SettingsCommandLineParser parser;
  parser.process(getArguments());
  
  Library lib;
  lib.findLibrary("/home/philippe/opt/build/cross/mxe/usr/i686-w64-mingw32.shared.posix/qt5/bin/Qt5Core.dll");

  return 0;
}
