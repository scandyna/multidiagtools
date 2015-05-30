/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
  pvCopyParentToChildOnInsertion = false;
  pvRelationOperatorWithPreviousItem = " AND ";
}

mdtSqlRelationItem::~mdtSqlRelationItem()
{
}

void mdtSqlRelationItem::setParentField(const QSqlField &field)
{
  pvParentField = field;
  // Setup data protection + value for no data filter regarding type
  switch(pvParentField.type()){
    case QVariant::Char:
      pvDataProtection = "'";
      pvValueForNoDataFilter = "";
      break;
    case QVariant::String:
      pvDataProtection = "'";
      pvValueForNoDataFilter = "";
      break;
    case QVariant::Int:
      pvDataProtection = "";
      pvValueForNoDataFilter = -1;
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

void mdtSqlRelationItem::setCopyParentToChildOnInsertion(bool enable)
{
  pvCopyParentToChildOnInsertion = enable;
}

void mdtSqlRelationItem::setRelationOperatorWithPreviousItem(const QString &operatorWithPreviousItem)
{
  pvRelationOperatorWithPreviousItem = operatorWithPreviousItem;
}
