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
/*
 * This is a minimal application,
 *  usefull, for exemple, ti create
 *  suppressions files with valgrind.
 */
#include <QApplication>
#include <QTimer>
#include <QObject>
#include <QDialog>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCryptographicHash>
#include <QLineEdit>
#include <QDebug>

/*
 * Minimal thread
 */
class minimalThread : public QThread
{
 public:
  minimalThread(QObject *parent = 0);
  void run();
};
minimalThread::minimalThread(QObject *parent)
 : QThread(parent)
{
}
void minimalThread::run()
{
  msleep(10);
  qDebug() << "Running...";
}

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QTimer timer;
  QDialog dlg;
  QList<int> lst;
  QByteArray ba;
  QString str;
  minimalThread thd;
  QFileInfo fileInfo;
  QDir dir;
  QLabel label;
  QMessageBox msgbox;
  QProgressDialog pd;
  QCryptographicHash hash(QCryptographicHash::Sha1);
  QLineEdit lineEdit;

  // Basic containers usage
  lst << 1 << 2 << 3;
  ba = "abcd";
  str = "ABCD";
  // Basic hash usage
  hash.addData("zhhuhuu");
  hash.result();
  // File info
  fileInfo.setFile(QDir::tempPath());
  // Label
  label.setText(QObject::tr("Test"));
  // Thread
  thd.start();
  // Progress dialog
  pd.setMinimumDuration(0);
  pd.setRange(0, 10);
  pd.setLabelText("Test");
  pd.setValue(0);
  pd.setLabelText("Test with a longer string ...........................................");
  pd.setValue(5);
  pd.reset();

  // We just want to run for a short time, but app.exec() blocks until
  //  event loop is ended with quit. So, we use a timer to send this event.
  QObject::connect(&timer, SIGNAL(timeout()), qApp, SLOT(quit()));
  timer.start(2000);
  
  lineEdit.show();
  
  // Thread
  thd.wait();

  return app.exec();
}
