/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
// #include "MultiLanguageMainWindow.h"
#include <QCoreApplication>

#include "Client.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QVariant>
#include <QDebug>

template<typename T>
void debugQObjectProperties(const EntityBase<T> & obj)
{
  Q_ASSERT(obj.metaObject() != nullptr);

  const auto *metaObject = obj.metaObject();
  qDebug() << "Class " << metaObject->className();
  qDebug() << " properties:";
  for(int i=0; i < metaObject->propertyCount(); ++i){
    const auto property = metaObject->property(i);
    qDebug() << "  property " << property.name();
    if(property.isValid()){
      qDebug() << "   type : " << property.typeName();
      qDebug() << "   value: " << property.read(&obj);
      qDebug() << "   is user: " << property.isUser(&obj);
      const auto attributes = obj.propertyAttributes(property.name());
      qDebug() << "   is part of ID: " << attributes.flags().testFlag(PropertyFlag::IsPartOfUniqueIdentifier);
      qDebug() << "   is required: " << attributes.flags().testFlag(PropertyFlag::IsRequired);
      qDebug() << "   max length: " << attributes.maxLength();
    }else{
      qDebug() << "   invalid";
    }
  }
  qDebug() << " methods:";
  for(int i=0; i < metaObject->methodCount(); ++i){
    const auto method = metaObject->method(i);
    qDebug() << "  name: " << method.name();
    if(method.isValid()){
      qDebug() << "   access: " << method.access();
      qDebug() << "   signature: " << method.methodSignature();
      qDebug() << "   type: " << method.typeName();
    }else{
      qDebug() << "   invalid";
    }
  }
}

void sandbox()
{
  Client co;
  co.setId(25);
  qDebug() << "co id: " << co.id();
  qDebug() << "co: " << &co;
  
  ClientData cli1;
  cli1.setId(1);
  cli1.setFirstName("cli1");
//   co = cli1;
  co.setData(cli1);
  qDebug() << "cli1 id: " << cli1.id() << " , fn: " << cli1.firstName();
  qDebug() << "co id: " << co.id() << " , fn: " << co.firstName();
  qDebug() << "co.setId(10)";
  co.setId(10);
  qDebug() << "co.setFirstName(\"CLI 10\")";
  co.setFirstName("CLI 10");
  qDebug() << "cli1 id: " << cli1.id() << " , fn: " << cli1.firstName();
  qDebug() << "co id: " << co.id() << " , fn: " << co.firstName();
  const auto cli = co.data();
  qDebug() << "cli id: " << cli.id() << " , fn: " << cli.firstName();

  debugQObjectProperties(co);
}

int main(/*int argc, char **argv*/)
{
//   QCoreApplication app(argc, argv);

  sandbox();

  SomeDataStruct sdt1;
  sdt1.id = 5;
//   sdt1.name = "Name";

//   DataStruct ds;
//   qDebug() << "ds, " << ds.id.columnName() << ": " << ds.id << " , name: " << ds.name.columnName() << ": " << ds.name;
// 
//   DataStruct2 ds2;
//   qDebug() << "ds2, " << ds2.id.columnName() << ": " << ds2.id << " , name: " << ds2.name.columnName() << ": " << ds2.name;

  return 0;
//   return app.exec();
}
