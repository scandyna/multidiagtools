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
#include <QCoreApplication>
#include <QTimer>

#include <QDebug>

using namespace Mdt;

MdtDeployUtilsMain::MdtDeployUtilsMain(QObject* parent)
 : AbstractConsoleApplicationMainFunction(parent)
{
//   auto *app = QCoreApplication::instance();
//   Q_ASSERT(app != nullptr);
// 
// //   connect(app, &QCoreApplication::aboutToQuit, this, &MdtDeployUtilsMain::cleanup);
//   // If we let 0ms, we have a lots of memory leaks about dbus reported by ASAN
//   QTimer::singleShot(10, this, &MdtDeployUtilsMain::run);
}

MdtDeployUtilsMain::~MdtDeployUtilsMain()
{
  qDebug() << "DESTR ~MdtDeployUtilsMain()";
}

int MdtDeployUtilsMain::runMain()
{
  qDebug() << "Main...";
  
  return 3;
}

// int MdtDeployUtilsMain::mainImpl()
// {
//   qDebug() << "Main...";
//   
//   return 3;
// }

// void MdtDeployUtilsMain::run()
// {
//   int retCode = mainImpl();
//   QCoreApplication::exit(retCode);
// }

// void MdtDeployUtilsMain::cleanup()
// {
//   qDebug() << "cleanup";
//   this->~MdtDeployUtilsMain();
// }
