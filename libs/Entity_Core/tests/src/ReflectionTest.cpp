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
#include "ReflectionTest.h"
#include <QString>

#include <QDebug>

#include <QObject>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QVariant>

void ReflectionTest::initTestCase()
{
}

void ReflectionTest::cleanupTestCase()
{
}


struct DataStruct
{
  int id;
  QString firstName;
};

auto reflectMembers(const DataStruct & s)
{
  struct MetaData
  {
  };
}

template<typename T>
void debugStaticQObjectProperties()
{
//   Q_ASSERT(obj.metaObject() != nullptr);

//   const auto *metaObject = obj.metaObject();
  const auto metaObject = T::staticMetaObject;
  qDebug() << "Class " << metaObject.className();
  qDebug() << " properties:";
  for(int i=0; i < metaObject.propertyCount(); ++i){
    const auto property = metaObject.property(i);
    qDebug() << "  property " << property.name();
    if(property.isValid()){
      qDebug() << "   type : " << property.typeName();
//       qDebug() << "   value: " << property.read(&obj);
//       qDebug() << "   is user: " << property.isUser(&obj);
//       const auto attributes = obj.propertyAttributes(property.name());
//       qDebug() << "   is part of ID: " << attributes.flags().testFlag(PropertyFlag::IsPartOfUniqueIdentifier);
//       qDebug() << "   is required: " << attributes.flags().testFlag(PropertyFlag::IsRequired);
//       qDebug() << "   max length: " << attributes.maxLength();
    }else{
      qDebug() << "   invalid";
    }
  }
}

// template<typename T>
void debugQObjectProperties(const QObject & obj)
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
//       const auto attributes = obj.propertyAttributes(property.name());
//       qDebug() << "   is part of ID: " << attributes.flags().testFlag(PropertyFlag::IsPartOfUniqueIdentifier);
//       qDebug() << "   is required: " << attributes.flags().testFlag(PropertyFlag::IsRequired);
//       qDebug() << "   max length: " << attributes.maxLength();
    }else{
      qDebug() << "   invalid";
    }
  }
}

void ReflectionTest::sandbox()
{
  MyEntityData ed;
  debugStaticQObjectProperties<MyEntityData>();

  MyEntity e;
  debugQObjectProperties(e);
}

/*
 * Tests
 */

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ReflectionTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
