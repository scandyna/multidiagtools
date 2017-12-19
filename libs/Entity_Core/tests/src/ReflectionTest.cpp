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
#include <array>

void ReflectionTest::initTestCase()
{
}

void ReflectionTest::cleanupTestCase()
{
}

class Field
{
 public:

  explicit Field(const QString & name)
   : mFieldName(name)
  {
  }

  QString fieldName() const
  {
    return mFieldName;
  }

 private:

  QString mFieldName;
};

struct ClientDataStruct
{
  int id;
  QString firstName;
};

struct ClientStaticDefinition
{
  static const QString entityName()
  {
    return "Client";
  }

  static const Field id()
  {
    return Field("id");
  }

  static const Field firstName()
  {
    return Field("firstName");
  }

  static std::array<Field, 2> fieldList()
  {
    return {{Field("id"), Field("firstName")}};
  }
};

class ClientData
{
 public:

  static ClientStaticDefinition def()
  {
    return ClientStaticDefinition{};
  }

 private:

  ClientDataStruct mData;
};


template<typename T>
void debugEntityDef(const T & entity)
{
  qDebug() << "Entity: " << entity.def().entityName();
  for(const auto & field : entity.def().fieldList()){
    qDebug() << " field: " << field.fieldName();
  }
}

auto reflectMembers(const ClientDataStruct & s)
{
  struct MetaData
  {
  };
}


void ReflectionTest::sandbox()
{
  qDebug() << "sizeof(ClientStaticDefinition): " << sizeof(ClientStaticDefinition);

  ClientData client;
  debugEntityDef(client);
  qDebug() << client.def().id().fieldName();
  qDebug() << client.def().firstName().fieldName();
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
