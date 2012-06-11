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
#include "mdtUicNumberTest.h"
#include "mdtUicNumber.h"
#include "mdtUicNumberItem.h"

/// NOTE: add 91 85 4 450 014
void mdtUicNumberTest::controlKeyTest()
{
  mdtUicNumber uic;

  // Initial state
  QVERIFY(!uic.isValid());
  QVERIFY(uic.controlKey() < 0);
  QVERIFY(uic.compact6UicNumber() == "");
  QVERIFY(uic.compact11UicNumber() == "");

  // Tests with some invalid numbers
  QVERIFY(!uic.setNumber("A 560 005"));
  QVERIFY(!uic.isValid());
  QVERIFY(uic.controlKey() < 0);
  QVERIFY(!uic.setNumber("45 560 005"));
  QVERIFY(!uic.isValid());
  QVERIFY(uic.controlKey() < 0);

  // Test with some UIC number - simple format
  QVERIFY(uic.setNumber("21 - 81 - 24 71 217"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 3);
  QVERIFY(uic.setNumber("560 005"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 1);
  QVERIFY(uic.setNumber("560 012"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 7);
  QVERIFY(uic.setNumber("94 85 7 560 256"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.setNumber("50 85 20-43 028"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 3);
  QVERIFY(uic.setNumber("50 85 29 43 028"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 4);
  QVERIFY(uic.setNumber("50 85 39 43 864"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.setNumber("560 044"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.setNumber("560 119"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.setNumber("560 109"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 1);
  QVERIFY(uic.setNumber("460 002"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 9);
  QVERIFY(uic.setNumber("460 006"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.setNumber("460 014"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 4);
  QVERIFY(uic.setNumber("460 040"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 9);
  QVERIFY(uic.setNumber("50 85 20 34 684"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 4);
  QVERIFY(uic.setNumber("50 85 92 33 645"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 9);
  QVERIFY(uic.setNumber("51 85 00-30 949"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 8);
  QVERIFY(uic.setNumber("522 007"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 4);

  // Test the format check
  QVERIFY(uic.setNumber("560 005-1"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 1);
  QVERIFY(uic.compact6UicNumber() == "560005");
  QVERIFY(uic.compact11UicNumber() == "");
  QVERIFY(uic.setNumber("94 85 7 560 256 - 0"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.compact6UicNumber() == "560256");
  QVERIFY(uic.compact11UicNumber() == "94857560256");
  QVERIFY(uic.setNumber("50 85 20-43 028 - 3"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 3);
  QVERIFY(uic.setNumber("50 85 29 43 028 - 4"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 4);
  QVERIFY(uic.compact11UicNumber() == "50852943028");
  QVERIFY(uic.setNumber("50 85 39 43 864 - 0"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.setNumber("560 044-0"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.compact6UicNumber() == "560044");
  QVERIFY(uic.compact11UicNumber() == "");
  QVERIFY(uic.setNumber("460 002-9"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 9);
  QVERIFY(uic.compact6UicNumber() == "460002");
  QVERIFY(uic.setNumber("460 006-0"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 0);
  QVERIFY(uic.compact6UicNumber() == "460006");
  QVERIFY(uic.compact11UicNumber() == "");
  QVERIFY(uic.setNumber("50 85 20-34 684-4"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 4);
  QVERIFY(uic.compact11UicNumber() == "50852034684");
  QVERIFY(uic.setNumber("50 85 92-33 645-9"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 9);
  QVERIFY(uic.compact11UicNumber() == "50859233645");
  QVERIFY(uic.setNumber("522 007-4"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.controlKey() == 4);
  QVERIFY(uic.compact6UicNumber() == "522007");
  QVERIFY(uic.compact11UicNumber() == "");
}

void mdtUicNumberTest::detailsTest()
{
  mdtUicNumber uic;

  // Initial states
  QVERIFY(!uic.isValid());
  QVERIFY(!uic.countriesDbLoaded());
  QVERIFY(!uic.usagesDbLoaded());
  QVERIFY(!uic.typesDbLoaded());
  QVERIFY(!uic.speedAndHeatDbLoaded());
  QVERIFY(!uic.dbsLoaded());

  // Load databases manually
  QVERIFY(uic.loadCountriesDb("data/uic/uic_countries_db.csv"));
  QVERIFY(uic.countriesDbLoaded());
  QVERIFY(!uic.dbsLoaded());
  QVERIFY(uic.loadUsagesDb("data/uic/uic_usages_db.csv"));
  QVERIFY(uic.usagesDbLoaded());
  QVERIFY(!uic.dbsLoaded());
  QVERIFY(uic.loadTypesDb("data/uic/uic_types_db.csv"));
  QVERIFY(uic.typesDbLoaded());
  QVERIFY(!uic.dbsLoaded());
  QVERIFY(uic.loadSpeedAndHeatsDb("data/uic/uic_speedandheat_db.csv"));
  QVERIFY(uic.speedAndHeatDbLoaded());
  QVERIFY(uic.dbsLoaded());
  // Reload DBs
  uic.unloadDbs();
  QVERIFY(!uic.dbsLoaded());
  QVERIFY(uic.loadDbs("data/uic"));
  QVERIFY(uic.dbsLoaded());

  // Check with 11 digits UIC numbers
  QVERIFY(uic.setNumber("50 85 39 43 864"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.usage().number() == 50);
  QVERIFY(uic.country().number() == 85);
  QVERIFY(uic.country().code() == "CH");
  QVERIFY(uic.type().number() == 39);
  QVERIFY(uic.speedAndHeat().number() == 43);
  QVERIFY(uic.serialNumber() == "864");

  // Check with 6 digits UIC numbers
  QVERIFY(uic.setNumber("460 002"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.type().number() == 460);
  QVERIFY(uic.serialNumber() == "002");

  // Check with 11 digits "other usage" UIC numbers
  QVERIFY(uic.setNumber("94 85 7 560 256"));
  QVERIFY(uic.isValid());
  QVERIFY(uic.usage().number() == 94);
  QVERIFY(uic.country().number() == 85);
  QVERIFY(uic.country().code() == "CH");
  QVERIFY(uic.serialNumber() == "256");

  // Check with 11 digits "other usage" UIC numbers
  QVERIFY(uic.setNumber("91 85 4 450 014"));

  qDebug() << "UIC: " << uic.formatedUicNumber();
  qDebug() << "UIC 6: " << uic.compact6UicNumber();
  qDebug() << "UIC 11: " << uic.compact11UicNumber();
  qDebug() << uic.country().nameFr();
  qDebug() << uic.usage().nameFr();
  qDebug() << uic.type().nameFr();
  qDebug() << uic.speedAndHeat().nameFr();

  QVERIFY(uic.isValid());
  QVERIFY(uic.usage().number() == 91);
  QVERIFY(uic.country().number() == 85);
  QVERIFY(uic.country().code() == "CH");
  QVERIFY(uic.serialNumber() == "014");
}

