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

  mdtDeviceU3606AWidget w;
  
  w.show();
  
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
