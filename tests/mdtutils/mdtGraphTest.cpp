/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtGraphTest.h"
#include "mdtGraphVertexData.h"
#include "mdtApplication.h"
#include <QVariant>
#include <QString>

void mdtGraphTest::vertexDataTest()
{
  mdtGraphVertexData d;

  // Check defaults
  QCOMPARE(d.key(), QVariant());
  QCOMPARE(d.data(), QVariant());
  // Check simple data access
  d.setData("Data 01");
  QCOMPARE(d.data(), QVariant("Data 01"));
  d.setData("Data 02");
  QCOMPARE(d.data(), QVariant("Data 02"));
  // Check multiple data access
  d.setData("Data A", "FldA");
  QCOMPARE(d.data("FldA"), QVariant("Data A"));
  d.setData("Data B", "FldB");
  QCOMPARE(d.data("FldA"), QVariant("Data A"));
  QCOMPARE(d.data("FldB"), QVariant("Data B"));
  QCOMPARE(d.data(), QVariant("Data 02"));
  // Unavailable field
  QCOMPARE(d.data("UnknownFld"), QVariant());
}



int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtGraphTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
