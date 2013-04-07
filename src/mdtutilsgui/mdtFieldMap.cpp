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
#include "mdtFieldMap.h"

mdtFieldMap::mdtFieldMap()
{
}

mdtFieldMap::~mdtFieldMap()
{
  clear();
}

void mdtFieldMap::addItem(mdtFieldMapItem *item)
{
  Q_ASSERT(item != 0);

  pvItemsByFieldIndex.insert(item->fieldIndex(), item);
  pvItemsByFieldName.insert(item->fieldName(), item);
  pvItemsByDisplayText.insert(item->fieldDisplayText(), item);
  pvItemsBySourceFieldIndex.insert(item->sourceFieldIndex(), item);
  pvItemsBySourceFieldName.insert(item->sourceFieldName(), item);
}

void mdtFieldMap::clear()
{
  qDeleteAll(pvItemsByFieldIndex);
  pvItemsByFieldIndex.clear();
  pvItemsByFieldName.clear();
  pvItemsByDisplayText.clear();
  pvItemsBySourceFieldIndex.clear();
  pvItemsBySourceFieldName.clear();
}

mdtFieldMapItem *mdtFieldMap::itemAtFieldIndex(int index)
{
  return pvItemsByFieldIndex.value(index, 0);
}

mdtFieldMapItem *mdtFieldMap::itemAtFieldName(const QString &name)
{
  return pvItemsByFieldName.value(name, 0);
}

mdtFieldMapItem *mdtFieldMap::itemAtDisplayText(const QString &text)
{
  return pvItemsByDisplayText.value(text, 0);
}

mdtFieldMapItem *mdtFieldMap::itemAtSourceFieldIndex(int index)
{
  return pvItemsBySourceFieldIndex.value(index, 0);
}

mdtFieldMapItem *mdtFieldMap::itemAtSourceFieldName(const QString &name)
{
  return pvItemsBySourceFieldName.value(name, 0);
}

/**
mdtFieldMapItem *mdtFieldMap::itemAtSourceFieldDisplayText(const QString &text)
{
}
*/

QVariant mdtFieldMap::dataForFieldIndex(const QStringList &sourceData, int fieldIndex)
{
}

QVariant mdtFieldMap::dataForFieldName(const QStringList &sourceData, const QString &fieldName)
{
}

QVariant mdtFieldMap::dataForDisplayText(const QStringList &sourceData, const QString &displayText)
{
}

QString mdtFieldMap::dataForSourceFieldIndex(const QList<QVariant> &data, int sourceFieldIndex)
{
}

QString mdtFieldMap::dataForSourceFieldName(const QList<QVariant> &data, const QString &sourceFieldName)
{
}

/**
QString mdtFieldMap::dataForSourceFieldDisplayText(const QList<QVariant> &data, const QString &sourceFieldDisplayText)
{
}
*/
