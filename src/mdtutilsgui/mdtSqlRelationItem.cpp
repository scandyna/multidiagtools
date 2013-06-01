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
#include "mdtSqlRelationItem.h"
#include <QVariant>

mdtSqlRelationItem::mdtSqlRelationItem()
{
  pvParentFieldIndex = -1;
  pvChildFieldIndex = -1;
}

mdtSqlRelationItem::~mdtSqlRelationItem()
{
}

void mdtSqlRelationItem::setParentField(const QSqlField &field)
{
  pvParentField = field;
  // Setup data protection regarding type
  switch(pvParentField.type()){
    case QVariant::Char:
      pvDataProtection = "'";
      break;
    case QVariant::String:
      pvDataProtection = "'";
      break;
    default:
      pvDataProtection = "";
  }
}

void mdtSqlRelationItem::setParentFieldIndex(int index)
{
  pvParentFieldIndex = index;
}

void mdtSqlRelationItem::setChildField(const QSqlField & field)
{
  pvChildField = field;
}

void mdtSqlRelationItem::setChildFieldIndex(int index)
{
  pvChildFieldIndex = index;
}

QString mdtSqlRelationItem::parentFieldName() const
{
  return pvParentField.name();
}

int mdtSqlRelationItem::parentFieldIndex() const
{
  return pvParentFieldIndex;
}

QString mdtSqlRelationItem::childFieldName() const
{
  return pvChildField.name();
}

int mdtSqlRelationItem::childFieldIndex() const
{
  return pvChildFieldIndex;
}

QString mdtSqlRelationItem::dataProtection() const
{
  return pvDataProtection;
}

QVariant::Type mdtSqlRelationItem::dataType() const
{
  return pvParentField.type();
}
