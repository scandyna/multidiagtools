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
#include "mdtParentChildTableModel.h"
#include <QStringList>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlTableModel>

#include <QTextCodec>
#include "mdtErrorOut.h"
#include "mdtError.h"

#include "linux/mdtDeviceU3606AWidget.h"
#include "mdtLed.h"
#include "mdtBlinkLed.h"
#include <QGridLayout>
#include <QWidget>

#include "mdtSerialPortSetupDialog.h"
#include "mdtSerialPortManager.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QStringList lst;
  int retVal;

#ifdef Q_OS_UNIX
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

  // Init error system
  if(!mdtErrorOut::init("essais.log")){
    qDebug() << "main(): unable to init the error system";
    return 1;
  }
  mdtErrorOut::setDialogLevelsMask(mdtError::Info | mdtError::Warning | mdtError::Error);

  // Essais serial port dialog
  mdtSerialPortSetupDialog dlg;
  dlg.setPortManager(new mdtSerialPortManager);
  dlg.show();
  
  //mdtDeviceU3606AWidget dw;
  //dw.show();

  /* Essais LED */
/*
  QWidget *w = new QWidget;
  QGridLayout *layout = new QGridLayout;
  mdtBlinkLed *led;

  for(int x=0; x<20; x++){
    for(int y=0; y<2; y++){
      led = new mdtBlinkLed;
      led->setFixedSize(30, 30);
      led->setTextMode();
      led->setBlinking(true);
      layout->addWidget(led, y, x);
    }
    for(int y=2; y<4; y++){
      led = new mdtBlinkLed;
      led->setFixedSize(30, 30);
      //led->setTextMode();
      led->setBlinking(true);
      layout->addWidget(led, y, x);
    }
  }
  w->setLayout(layout);
  w->resize(400,200);
  w->show();
*/
  
  retVal = app.exec();

  // Free the error system
  mdtErrorOut::destroy();
  
  return retVal;

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("essais.db");
  if(!db.open()){
    qDebug() << "Could not open database 'essais.db";
  }
  qDebug() << "Found following tables: " << db.tables();

  // Modèles
  mdtParentChildTableModel m;
  QSqlTableModel cm1, cm2;
  
  cm1.setTable("Contacts");
  cm1.select();
  cm2.setTable("address");
  cm2.select();

  // Vues
  //QListView lw;
  QTableView tw;
  QTreeView trw;

  //lw.setModel(&cm1);
  //lw.show();
  tw.setModel(&cm1);
  tw.resize(400, 300);
  tw.show();
  trw.setModel(&cm2);
  trw.resize(400, 300);
  trw.show();

  //return app.exec();
}
