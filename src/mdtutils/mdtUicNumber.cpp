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
#include "mdtUicNumber.h"
#include "mdtError.h"
#include "mdtCsvFile.h"
#include <QChar>
#include <QStringList>

#include <QDebug>

mdtUicNumber::mdtUicNumber()
{
}

mdtUicNumber::~mdtUicNumber()
{
  // Release used memory
  pvUicNumberDigits.clear();
  unloadDbs();
}

bool mdtUicNumber::setNumber(const QString &uicNumber)
{
  QString number;
  int i, digit;

  // Remove unusable chars
  number = uicNumber.simplified();
  number.remove(" ");
  number.remove("-");
  // Check length:
  //  - We only know about 6 and 11 digits UIC numbers
  //  - If we have 7 or 12 digits, the control key is given
  if((number.size() == 7)||(number.size() == 12)){
    number.remove(number.size()-1, 1);
  }
  if((number.size() != 6)&&(number.size() != 11)){
    auto e = mdtErrorNew("UIC number has invalid digits count (allowed: 6, 7, 11 or 12 digits)", mdtError::Error, "mdtUicNumber");
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    return false;
  }
  // Store digits
  pvUicNumberDigits.clear();
  for(i=0; i<number.size(); ++i){
    digit = number.at(i).digitValue();
    if(digit<0){
      auto e = mdtErrorNew("Invalid UIC number (contains unallowed chars)", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      pvUicNumberDigits.clear();
      return false;
    }
    pvUicNumberDigits.append(digit);
  }

  return true;
}

QString mdtUicNumber::formatedUicNumber()
{
  QString uicNumber;

  if(!isValid()){
    return "<unknow>";
  }
  // Case of 6 digits number
  if(pvUicNumberDigits.size() == 6){
    uicNumber.append(QString::number(pvUicNumberDigits.at(0)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(1)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(2)));
    uicNumber.append(" ");
    uicNumber.append(QString::number(pvUicNumberDigits.at(3)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(4)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(5)));
    uicNumber.append("-");
    uicNumber.append(controlKeyString());
  }else if(pvUicNumberDigits.size() == 11){
    uicNumber.append(QString::number(pvUicNumberDigits.at(0)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(1)));
    uicNumber.append(" ");
    uicNumber.append(QString::number(pvUicNumberDigits.at(2)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(3)));
    uicNumber.append(" ");
    uicNumber.append(QString::number(pvUicNumberDigits.at(4)));
    // We have special cases for UIC numbbers beginning with 9
    if(pvUicNumberDigits.at(0) == 9){
      uicNumber.append(" ");
      uicNumber.append(QString::number(pvUicNumberDigits.at(5)));
      uicNumber.append(QString::number(pvUicNumberDigits.at(6)));
      uicNumber.append(QString::number(pvUicNumberDigits.at(7)));
    }else{
      uicNumber.append(QString::number(pvUicNumberDigits.at(5)));
      uicNumber.append(" ");
      uicNumber.append(QString::number(pvUicNumberDigits.at(6)));
      uicNumber.append(QString::number(pvUicNumberDigits.at(7)));
    }
    uicNumber.append(" ");
    uicNumber.append(QString::number(pvUicNumberDigits.at(8)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(9)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(10)));
    uicNumber.append("-");
    uicNumber.append(controlKeyString());
  }else{
    uicNumber = "<unknow>";
  }

  return uicNumber;
}

QString mdtUicNumber::compact6UicNumber()
{
  QString uicNumber;

  if(!isValid()){
    return uicNumber;
  }

  // Case of 6 digits number
  if(pvUicNumberDigits.size() == 6){
    uicNumber.append(QString::number(pvUicNumberDigits.at(0)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(1)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(2)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(3)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(4)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(5)));
  }else if(pvUicNumberDigits.size() == 11){
    uicNumber.append(QString::number(pvUicNumberDigits.at(5)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(6)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(7)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(8)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(9)));
    uicNumber.append(QString::number(pvUicNumberDigits.at(10)));
  }

  return uicNumber;
}

QString mdtUicNumber::compact11UicNumber()
{
  QString uicNumber;

  if(!isValid()){
    return uicNumber;
  }
  // Must have 11 digits
  if(pvUicNumberDigits.size() != 11){
    return uicNumber;
  }
  uicNumber.append(QString::number(pvUicNumberDigits.at(0)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(1)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(2)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(3)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(4)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(5)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(6)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(7)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(8)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(9)));
  uicNumber.append(QString::number(pvUicNumberDigits.at(10)));

  return uicNumber;
}

bool mdtUicNumber::isValid()
{
  // Check if somethig was set
  if(pvUicNumberDigits.size() < 1){
    auto e = mdtErrorNew("No valid UIC number was set", mdtError::Error, "mdtUicNumber");
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    return false;
  }

  return true;
}

int mdtUicNumber::controlKey()
{
  int sum;

  if(!isValid()){
    return -1;
  }
  // Compute
  if(pvUicNumberDigits.size() > 7){
    sum = controlSum11();
  }else{
    sum = controlSum6();
  }
  // Get diff
  sum = (10 - (sum % 10)) % 10;

  return sum;
}

QString mdtUicNumber::controlKeyString()
{
  int ck;

  ck = controlKey();
  if(ck < 0){
    return "<unknow>";
  }

  return QString::number(ck);
}

QString mdtUicNumber::serialNumber()
{
  QString sn;
  int i;

  if(!isValid()){
    return "<unknow>";
  }
  if(pvUicNumberDigits.size() == 6){
    for(i=3; i<6; i++){
      sn.append(QString::number(pvUicNumberDigits.at(i)));
    }
  }else if(pvUicNumberDigits.size() == 11){
    for(i=8; i<11; i++){
      sn.append(QString::number(pvUicNumberDigits.at(i)));
    }
  }else{
    sn = "<unknow>";
  }

  return sn;
}

bool mdtUicNumber::loadCountriesDb(const QString &dbPath)
{
  mdtCsvFile csv;
  QList<QStringList> db;
  mdtUicNumberItem *item;
  int i;

  // Open file
  csv.setFileName(dbPath);
  if(!csv.open(QIODevice::ReadOnly | QIODevice::Text)){
    auto e = mdtErrorNew("Cannot open file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    return false;
  }
  // Read
  if(!csv.readLines(";", "\"")){
    auto e = mdtErrorNew("Cannot parse CSV file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    csv.close();
    return false;
  }
  db = csv.lines();
  csv.close();
  qDeleteAll(pvCountries);
  pvCountries.clear();
  for(i=1; i<db.size(); i++){
    // Check line size
    if(db.at(i).size() != 5){
      auto e = mdtErrorNew("CSV file coloumn count false (expected: 5)", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item = new mdtUicNumberItem;
    Q_ASSERT(item != 0);
    // Store column data
    if(!item->setNumber(db.at(i).at(0))){
      delete item;
      qDeleteAll(pvCountries);
      pvCountries.clear();
      auto e = mdtErrorNew("CSV file: first coloumn is not a number", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item->setCode(db.at(i).at(1));
    item->setNameFr(db.at(i).at(2));
    item->setNameEn(db.at(i).at(3));
    item->setNameDe(db.at(i).at(4));
    // Add to the list
    pvCountries.insert(item->number(), item);
  }

  return true;
}

bool mdtUicNumber::countriesDbLoaded()
{
  return (pvCountries.size() > 0);
}

bool mdtUicNumber::loadUsagesDb(const QString &dbPath)
{
  mdtCsvFile csv;
  QList<QStringList> db;
  mdtUicNumberItem *item;
  int i;

  // Open file
  csv.setFileName(dbPath);
  if(!csv.open(QIODevice::ReadOnly | QIODevice::Text)){
    auto e = mdtErrorNew("Cannot open file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    return false;
  }
  // Read
  if(!csv.readLines(";", "\"")){
    auto e = mdtErrorNew("Cannot parse CSV file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    csv.close();
    return false;
  }
  db = csv.lines();
  csv.close();
  qDeleteAll(pvUsages);
  pvUsages.clear();
  for(i=1; i<db.size(); i++){
    // Check line size
    if(db.at(i).size() != 4){
      auto e = mdtErrorNew("CSV file coloumn count false (expected: 4)", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item = new mdtUicNumberItem;
    Q_ASSERT(item != 0);
    // Store column data
    if(!item->setNumber(db.at(i).at(0))){
      delete item;
      qDeleteAll(pvUsages);
      pvUsages.clear();
      auto e = mdtErrorNew("CSV file: first coloumn is not a number", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item->setNameFr(db.at(i).at(1));
    item->setNameEn(db.at(i).at(2));
    item->setNameDe(db.at(i).at(3));
    // Add to the list
    pvUsages.insert(item->number(), item);
  }

  return true;
}

bool mdtUicNumber::usagesDbLoaded()
{
  return (pvUsages.size() > 0);
}

bool mdtUicNumber::loadTypesDb(const QString &dbPath)
{
  mdtCsvFile csv;
  QList<QStringList> db;
  mdtUicNumberItem *item;
  int i;

  // Open file
  csv.setFileName(dbPath);
  if(!csv.open(QIODevice::ReadOnly | QIODevice::Text)){
    auto e = mdtErrorNew("Cannot open file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    return false;
  }
  // Read
  if(!csv.readLines(";", "\"")){
    auto e = mdtErrorNew("Cannot parse CSV file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    csv.close();
    return false;
  }
  db = csv.lines();
  csv.close();
  qDeleteAll(pvTypes);
  pvTypes.clear();
  for(i=1; i<db.size(); i++){
    // Check line size
    if(db.at(i).size() != 4){
      auto e = mdtErrorNew("CSV file coloumn count false (expected: 4)", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item = new mdtUicNumberItem;
    Q_ASSERT(item != 0);
    // Store column data
    if(!item->setNumber(db.at(i).at(0))){
      delete item;
      qDeleteAll(pvTypes);
      pvTypes.clear();
      auto e = mdtErrorNew("CSV file: first coloumn is not a number", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item->setNameFr(db.at(i).at(1));
    item->setNameEn(db.at(i).at(2));
    item->setNameDe(db.at(i).at(3));
    // Add to the list
    pvTypes.insert(item->number(), item);
  }

  return true;
}

bool mdtUicNumber::typesDbLoaded()
{
  return (pvTypes.size() > 0);
}

bool mdtUicNumber::loadSpeedAndHeatsDb(const QString &dbPath)
{
  mdtCsvFile csv;
  QList<QStringList> db;
  mdtUicNumberItem *item;
  int i;

  // Open file
  csv.setFileName(dbPath);
  if(!csv.open(QIODevice::ReadOnly | QIODevice::Text)){
    auto e = mdtErrorNew("Cannot open file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    return false;
  }
  // Read
  if(!csv.readLines(";", "\"")){
    auto e = mdtErrorNew("Cannot parse CSV file " + dbPath, mdtError::Error, "mdtUicNumber");
    e.setSystemError(csv.error(), csv.errorString());
    MDT_ERROR_SET_SRC(e, "mdtUicNumber");
    e.commit();
    csv.close();
    return false;
  }
  db = csv.lines();
  csv.close();
  qDeleteAll(pvSpeedAndHeats);
  pvSpeedAndHeats.clear();
  for(i=1; i<db.size(); i++){
    // Check line size
    if(db.at(i).size() != 4){
      auto e = mdtErrorNew("CSV file coloumn count false (expected: 4)", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item = new mdtUicNumberItem;
    Q_ASSERT(item != 0);
    // Store column data
    if(!item->setNumber(db.at(i).at(0))){
      delete item;
      qDeleteAll(pvSpeedAndHeats);
      pvSpeedAndHeats.clear();
      auto e = mdtErrorNew("CSV file: first coloumn is not a number", mdtError::Error, "mdtUicNumber");
      MDT_ERROR_SET_SRC(e, "mdtUicNumber");
      e.commit();
      return false;
    }
    item->setNameFr(db.at(i).at(1));
    item->setNameEn(db.at(i).at(2));
    item->setNameDe(db.at(i).at(3));
    // Add to the list
    pvSpeedAndHeats.insert(item->number(), item);
  }

  return true;
}

bool mdtUicNumber::speedAndHeatDbLoaded()
{
  return (pvSpeedAndHeats.size() > 0);
}

bool mdtUicNumber::loadDbs(const QString &dir)
{
  if(!loadCountriesDb(dir + "/uic_countries_db.csv")){
    return false;
  }
  if(!loadUsagesDb(dir + "/uic_usages_db.csv")){
    return false;
  }
  if(!loadTypesDb(dir + "/uic_types_db.csv")){
    return false;
  }
  if(!loadSpeedAndHeatsDb(dir + "/uic_speedandheat_db.csv")){
    return false;
  }

  return true;
}

bool mdtUicNumber::dbsLoaded()
{
  if(!countriesDbLoaded()){
    return false;
  }
  if(!usagesDbLoaded()){
    return false;
  }
  if(!typesDbLoaded()){
    return false;
  }
  if(!speedAndHeatDbLoaded()){
    return false;
  }

  return true;
}

void mdtUicNumber::unloadDbs()
{
  qDeleteAll(pvCountries);
  pvCountries.clear();
  qDeleteAll(pvUsages);
  pvUsages.clear();
  qDeleteAll(pvTypes);
  pvTypes.clear();
  qDeleteAll(pvSpeedAndHeats);
  pvSpeedAndHeats.clear();
}

mdtUicNumberItem mdtUicNumber::country()
{
  mdtUicNumberItem *item;
  int index;

  // Check about UIC number
  if(pvUicNumberDigits.size() < 4){
    return mdtUicNumberItem();
  }
  // Contruct index
  index = 10*pvUicNumberDigits.at(2) + pvUicNumberDigits.at(3);
  // Get item
  item = pvCountries.value(index);
  if(item == 0){
    return mdtUicNumberItem();
  }

  return *item;
}

mdtUicNumberItem mdtUicNumber::usage()
{
  mdtUicNumberItem *item;
  int index;

  // Check about UIC number
  if(pvUicNumberDigits.size() < 2){
    return mdtUicNumberItem();
  }
  // Contruct index
  index = 10*pvUicNumberDigits.at(0) + pvUicNumberDigits.at(1);
  // Get item
  item = pvUsages.value(index);
  if(item == 0){
    return mdtUicNumberItem();
  }

  return *item;
}

mdtUicNumberItem mdtUicNumber::type()
{
  mdtUicNumberItem *item;
  int index;

  // Check about UIC number
  if(pvUicNumberDigits.size() == 6){
    index = 100*pvUicNumberDigits.at(0) + 10*pvUicNumberDigits.at(1) + pvUicNumberDigits.at(2);
  }else if(pvUicNumberDigits.size() == 11){
    // We have special cases for UIC numbbers beginning with 9
    if(pvUicNumberDigits.at(0) == 9){
      index = 100*pvUicNumberDigits.at(5) + 10*pvUicNumberDigits.at(6) + pvUicNumberDigits.at(7);
    }else{
      index = 10*pvUicNumberDigits.at(4) + pvUicNumberDigits.at(5);
    }
  }else{
    return mdtUicNumberItem();
  }
  // Get item
  item = pvTypes.value(index);
  if(item == 0){
    return mdtUicNumberItem();
  }

  return *item;
}

mdtUicNumberItem mdtUicNumber::speedAndHeat()
{
  mdtUicNumberItem *item;
  int index;

  // Check about UIC number
  if(pvUicNumberDigits.size() < 8){
    return mdtUicNumberItem();
  }
  // Contruct index
  index = 10*pvUicNumberDigits.at(6) + pvUicNumberDigits.at(7);
  // Get item
  item = pvSpeedAndHeats.value(index);
  if(item == 0){
    return mdtUicNumberItem();
  }

  return *item;
}

int mdtUicNumber::controlSum6()
{
  Q_ASSERT(pvUicNumberDigits.size() >= 6);

  int i, n;
  int sum;

  sum = 0;
  for(i=0; i<5; i+=2){
    sum += pvUicNumberDigits.at(i);
  }
  for(i=1; i<7; i+=2){
    n = 2*pvUicNumberDigits.at(i);
    if(n >= 10){
      n -= 9;
    }
    sum += n;
  }

  return sum;
}

int mdtUicNumber::controlSum11()
{
  Q_ASSERT(pvUicNumberDigits.size() >= 11);

  int i, n;
  int sum;

  sum = 0;
  for(i=0; i<11; i+=2){
    n = 2*pvUicNumberDigits.at(i);
    if(n >= 10){
      n -= 9;
    }
    sum += n;
  }
  for(i=1; i<10; i+=2){
    sum += pvUicNumberDigits.at(i);
  }

  return sum;
}
