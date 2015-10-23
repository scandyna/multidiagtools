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
#include "mdtSqlApplicationWidgetsTest.h"
#include "mdtApplication.h"

#include <memory>

#include <QPushButton>
#include <QPointer>

#include <QDebug>

void mdtSqlApplicationWidgetsTest::initTestCase()
{
  /*
   * Init and open database
   */
  QVERIFY(pvTempFile.open());
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());
}

void mdtSqlApplicationWidgetsTest::cleanupTestCase()
{
}

template<typename T> class sandboxSingletonDeleter;

template<typename T>
class sandboxSingleton
{
 public:

  ~sandboxSingleton()
  {
    qDebug() << "~sandboxSingleton() , this: " << this << " , pvInstance: " << pvInstance;
    pvInstance = nullptr;
  }

  static std::shared_ptr<T> instance()
  {
    if(pvInstance == nullptr){
      pvInstance = new T;
    }
    ///return std::shared_ptr<T, sandboxSingletonDeleter>(pvInstance, sandboxSingletonDeleter<T>());
    return std::shared_ptr<T>(pvInstance);
  }

  static void destruct()
  {
    delete pvInstance;
    pvInstance = nullptr;
  }

 protected:

  sandboxSingleton()
  {
    qDebug() << "sandboxSingleton() , this: " << this << " , pvInstance: " << pvInstance;
  }


 private:

  sandboxSingleton(const sandboxSingleton &) = delete;
  sandboxSingleton(sandboxSingleton &&) = delete;
  sandboxSingleton & operator=(const sandboxSingleton &) = delete;

 private:

  static T *pvInstance;
};

template<typename T>
T *sandboxSingleton<T>::pvInstance = nullptr;


template<typename T>
class sandboxSingletonDeleter
{
 public:

//   sandboxSingletonDeleter(const sandboxSingletonDeleter &) = delete;
//   sandboxSingletonDeleter(sandboxSingletonDeleter &&) = delete;

  sandboxSingletonDeleter()
  {
  }

  ~sandboxSingletonDeleter()
  {
    sandboxSingleton<T>::destruct();
  }
};


class mySingleton : public sandboxSingleton<mySingleton>
{
  friend class sandboxSingleton<mySingleton>;

 public:

  static QPushButton *getPushButton(QWidget *parent)
  {
    if(instance()->pb == nullptr){
      instance()->pb = new QPushButton(parent);
    }
    return instance()->pb;
  }

  ~mySingleton()
  {
    qDebug() << "~mySingleton() destruct";
    ///delete pb;
  }

 private:


  mySingleton()
  {
    qDebug() << "mySingleton() construct , this: " << this;
    pb = nullptr;
  }


  QPushButton *pb;
};

void mdtSqlApplicationWidgetsTest::sandbox()
{
  ///sandboxSingletonDeleter<mySingleton> stg;
  ///sandboxSingletonDeleter<int> stg;
  QWidget *parentWidget = new QWidget;

  ///mySingleton::instance();
  
  auto *pb = mySingleton::getPushButton(parentWidget);

  parentWidget->show();

  /*
  while(parentWidget->isVisible()){
    QTest::qWait(500);
  }
  */
  
  delete parentWidget;


// 
//   TestApplicationWidgets::setDatabase(pvDatabase);
//   QVERIFY(TestApplicationWidgets::database().tables() == pvDatabase.tables());
//   
//   mdtError e("Test", mdtError::Warning);
//   TestApplicationWidgets::doSomeThing();
  
  // Cleanup
  ///TestApplicationWidgets::clear();
}

/**
 * \todo In a sandbox, do some tests with QPointer
 */

class MyWidget : public QWidget
{
public:
  MyWidget(QWidget* parent = nullptr)
  : QWidget(parent) {}
  
  ~MyWidget()
  {
    qDebug() << "MyWidget::~MyWidget()";
  }
};

void mdtSqlApplicationWidgetsTest::ptrSandbox()
{
  QWidget *parent = new QWidget;
  ///MyWidget *mw = new MyWidget(parent);
  QPointer<MyWidget> mw(new MyWidget);
  ///QPointer<MyWidget> mw(new MyWidget(parent));

  qDebug() << mw;
  delete mw;
  qDebug() << mw;
  
  delete parent;
  
  
  
}


void mdtSqlApplicationWidgetsTest::simpleTest()
{
  // Following line must not compile
  // TestApplicationWidgets twa;

  mdtSqlApplicationWidgetsGuard<TestApplicationWidgets> testAppWidgetsGuard;

  /*
   * First calls
   */
  QVERIFY(!TestApplicationWidgets::database().isValid());
  QVERIFY(!TestApplicationWidgets::database().isOpen());
  TestApplicationWidgets::setDatabase(pvDatabase);
  QVERIFY(TestApplicationWidgets::database().isValid());
  QVERIFY(TestApplicationWidgets::database().isOpen());
  /*
   * Destruct and repeat
   */
  TestApplicationWidgets::destruct();
  QVERIFY(!TestApplicationWidgets::database().isValid());
  QVERIFY(!TestApplicationWidgets::database().isOpen());
  TestApplicationWidgets::setDatabase(pvDatabase);
  QVERIFY(TestApplicationWidgets::database().isValid());
  QVERIFY(TestApplicationWidgets::database().isOpen());

}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSqlApplicationWidgetsTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
