/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include <QApplication>
#include <QTextCodec>
#include <QDir>
#include "mdtError.h"
#include "mdtErrorOut.h"
#include "mdtUicNumber.h"
#include "mdtUicNumberWidget.h"

#include <QDebug>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int retVal;
  mdtUicNumber *uic;
  mdtUicNumberWidget uicw;
  QDir dataDir;

#ifdef Q_OS_UNIX
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

  // Init error system
  if(!mdtErrorOut::init("uicnumber.log")){
    qDebug() << "main(): unable to init the error system";
    return 1;
  }

  uic = new mdtUicNumber;
  // Find data path
  dataDir = QDir::currentPath() + "/data/uic";
  if(!dataDir.exists()){
    dataDir = "/usr/share/mdt/data/uic";
    if(!dataDir.exists()){
      qDebug() << "Cannot find data dir !";
      return 1;
    }
  }
  qDebug() << "Foud data in " << dataDir.path();
  uicw.setUicDbsDir(dataDir.path());
  uicw.setUicNumber(uic);
  uicw.show();
  
  retVal = app.exec();

  // Free the error system
  mdtErrorOut::destroy();
  delete uic;

  return retVal;
}
