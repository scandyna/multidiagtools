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
#include "mdtUicNumberItem.h"

#include <QDebug>

mdtUicNumberItem::mdtUicNumberItem()
{
  pvNumber = -1;
  pvCode = "<-->";
  pvNameFr = "<unknow>";
  pvNameEn = "<unknow>";
  pvNameDe = "<unknow>";
}

mdtUicNumberItem::~mdtUicNumberItem()
{
}

void mdtUicNumberItem::setNumber(int number)
{
  pvNumber = number;
}

bool mdtUicNumberItem::setNumber(const QString &number)
{
  bool ok = false;

  pvNumber = number.toInt(&ok);
  if(!ok){
    pvNumber = -1;
    return false;
  }

  return true;
}

int mdtUicNumberItem::number()
{
  return pvNumber;
}

void mdtUicNumberItem::setCode(const QString &code)
{
  if(!code.isEmpty()){
    pvCode = code;
  }
}

QString &mdtUicNumberItem::code()
{
  return pvCode;
}

void mdtUicNumberItem::setNameFr(const QString &name)
{
  if(!name.isEmpty()){
    pvNameFr = name;
  }
}

QString &mdtUicNumberItem::nameFr()
{
  return pvNameFr;
}

void mdtUicNumberItem::setNameEn(const QString &name)
{
  if(!name.isEmpty()){
    pvNameEn = name;
  }
}

QString &mdtUicNumberItem::nameEn()
{
  return pvNameEn;
}

void mdtUicNumberItem::setNameDe(const QString &name)
{
  if(!name.isEmpty()){
    pvNameDe = name;
  }
}

QString &mdtUicNumberItem::nameDe()
{
  return pvNameDe;
}
