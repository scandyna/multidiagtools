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
#include "Mdt/Application.h"

// #include <QTimer>
// #include <QApplication>

#include <QDebug>

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  // All code is exectued here
  MdtDeployUtilsMain mainImpl;

//   QTimer::singleShot(
//     10, // If we let 0, we have a lots ov memory leaks about dbus reported by ASAN
//     [](){
//       auto *app = Mdt::Application::instance();
//       qDebug() << "The function..";
//       app->exit(25);
//     }
//   );

  qDebug() << "main: enter eventloop";
  int ret = app.exec();
  qDebug() << "main: eventloop finished";

  return ret;
}
