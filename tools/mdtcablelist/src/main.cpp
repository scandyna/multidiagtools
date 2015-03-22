/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtApplication.h"

#include <QObject>
#include <QLocale>
#include <QSqlDatabase>

///#include <QStringList>
///#include <QDir>
///#include <QFileInfo>

#include "mdtClMainWindow.h"
#include "mdtDeviceContainer.h"
#include "mdtDeviceAddress.h"
#include "mdtModbusHwNodeId.h"
#include "mdtDeviceModbusWago.h"

#include <QDebug>

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  ///QSqlDatabase db;
  QString password;
  int retVal;
  // Terminal block editor
  ///mdtSqlWindow terminalBlockEditorWindow;
  ///mdtClTerminalBlockEditor *terminalBlockEditor;

  // Init app, we allow multiple instances
  if(!app.init()){
    return 1;
  }

  ///QObject::connect(&app, SIGNAL(languageChanged()), &modbusIoTool, SLOT(retranslate()));
  ///modbusIoTool.setAvailableTranslations(app.availableTranslations(), app.currentTranslationKey());

  // Add instruments to the global device container
  mdtGlobalDeviceContainer gdc;
  mdtDeviceAddress da;
  da.setModbusTcpIdentification("", 502, mdtModbusHwNodeId(0, 8, 0));
  da.setAlias("MainCoupler");
  gdc->addDevice<mdtDeviceModbusWago>(da);

  mdtClMainWindow window;

  window.show();

  retVal = app.exec();
  ///db.close();
  gdc->clear();

  return retVal;
}
