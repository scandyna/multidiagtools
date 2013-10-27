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

//#include <QDebug>

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

  if(!pvItems.contains(item)){
    pvItems.append(item);
  }
}

void mdtFieldMap::clear()
{
  qDeleteAll(pvItems);
  pvItems.clear();
}

mdtFieldMapItem *mdtFieldMap::itemAtFieldIndex(int index) const
{
  int i;
  mdtFieldMapItem *item;

  for(i = 0; i < pvItems.size(); ++i){
    item = pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->fieldIndex() == index){
      return item;
    }
  }

  return 0;
}

mdtFieldMapItem *mdtFieldMap::itemAtFieldName(const QString &name) const
{
  int i;
  mdtFieldMapItem *item;

  for(i = 0; i < pvItems.size(); ++i){
    item = pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->fieldName() == name){
      return item;
    }
  }

  return 0;
}

mdtFieldMapItem *mdtFieldMap::itemAtDisplayText(const QString &text)
{
  int i;
  mdtFieldMapItem *item;

  for(i = 0; i < pvItems.size(); ++i){
    item = pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->fieldDisplayText() == text){
      return item;
    }
  }

  return 0;
}

QString mdtFieldMap::sourceFieldNameAtFieldIndex(int index) const
{
  QString name;
  mdtFieldMapItem *item;

  item = itemAtFieldIndex(index);
  if(item != 0){
    name = item->sourceFieldName();
  }

  return name;
}

QList<mdtFieldMapItem*> mdtFieldMap::itemsAtSourceFieldIndex(int index) const
{
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;
  int i;

  for(i = 0; i < pvItems.size(); ++i){
    item = pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->sourceFieldIndex() == index){
      items.append(item);
    }
  }

  return items;
}

QList<mdtFieldMapItem*> mdtFieldMap::itemsAtSourceFieldName(const QString &name)
{
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;
  int i;

  for(i = 0; i < pvItems.size(); ++i){
    item = pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->sourceFieldName() == name){
      items.append(item);
    }
  }

  return items;
}

QVariant mdtFieldMap::dataForFieldIndex(const QStringList &sourceData, int fieldIndex) const
{
  ///QString src;
  ///QVariant data;

  mdtFieldMapItem *item = itemAtFieldIndex(fieldIndex);
  if(item == 0){
    return QVariant();
  }

  return item->destinationData(sourceData);
  /**
  if(item->sourceFieldIndex() < 0){
    return QVariant();
  }
  if(item->sourceFieldIndex() >= sourceData.size()){
    return QVariant();
  }
  if((item->sourceFieldDataStartOffset() < 0)&&(item->sourceFieldDataEndOffset() < 0)){
    src = sourceData.at(item->sourceFieldIndex());
    data = src;
    data.convert(item->dataType());
    return data;
  }
  if(item->sourceFieldDataStartOffset()<0){
    src = sourceData.at(item->sourceFieldIndex()).left(item->sourceFieldDataEndOffset()+1);
    data = src;
    data.convert(item->dataType());
    return data;
  }
  if(item->sourceFieldDataEndOffset()<0){
    src = sourceData.at(item->sourceFieldIndex());
    src = src.right(src.size() - item->sourceFieldDataStartOffset());
    data = src;
    data.convert(item->dataType());
    return data;
  }
  src = sourceData.at(item->sourceFieldIndex());
  src = src.mid(item->sourceFieldDataStartOffset(), item->sourceFieldDataEndOffset() - item->sourceFieldDataStartOffset() + 1);
  data = src;
  data.convert(item->dataType());

  return data;
  */
}

QVariant mdtFieldMap::dataForFieldName(const QStringList &sourceData, const QString &fieldName) const
{
  mdtFieldMapItem *item = itemAtFieldName(fieldName);
  if(item == 0){
    return QVariant();
  }
  return dataForFieldIndex(sourceData, item->fieldIndex());
}

QVariant mdtFieldMap::dataForDisplayText(const QStringList &sourceData, const QString &displayText)
{
  mdtFieldMapItem *item = itemAtDisplayText(displayText);
  if(item == 0){
    return QVariant();
  }
  return dataForFieldIndex(sourceData, item->fieldIndex());
}

QString mdtFieldMap::dataForSourceFieldIndex(const QList<QVariant> &data, int sourceFieldIndex)
{
  int i;
  QString str;
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;

  items = itemsAtSourceFieldIndex(sourceFieldIndex);
  for(i=0; i<items.size(); i++){
    item = items.at(i);
    Q_ASSERT(item != 0);
    if((item->fieldIndex() >= 0)&&(item->fieldIndex() < data.size())){
      insertDataIntoSourceString(str, data.at(item->fieldIndex()), item);
    }
  }

  return str;
}

QString mdtFieldMap::dataForSourceFieldName(const QList<QVariant> &data, const QString &sourceFieldName)
{
  int i;
  QString str;
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;

  items = itemsAtSourceFieldName(sourceFieldName);
  for(i=0; i<items.size(); i++){
    item = items.at(i);
    Q_ASSERT(item != 0);
    if((item->fieldIndex()>=0)&&(item->fieldIndex()<data.size())){
      insertDataIntoSourceString(str, data.at(item->fieldIndex()), item);
    }
  }

  return str;
}

QHash<QString, QString> mdtFieldMap::displayTextsByFieldNames() const
{
  QHash<QString, QString> result;
  mdtFieldMapItem *item;
  int i;

  for(i=0; i<pvItems.size(); ++i){
    item = pvItems.at(i);
    Q_ASSERT(item != 0);
    result.insert(item->fieldName(), item->fieldDisplayText());
  }

  return result;
}

void mdtFieldMap::insertDataIntoSourceString(QString &str, const QVariant &data, mdtFieldMapItem *item)
{
  Q_ASSERT(item != 0);

  int start, end;
  QString strData = data.toString();

  if((item->sourceFieldDataStartOffset()<0)&&(item->sourceFieldDataEndOffset()<0)){
    str = strData;
    return;
  }
  start = item->sourceFieldDataStartOffset();
  if(start<0){
    start = 0;
  }
  end = item->sourceFieldDataEndOffset();
  if(end<0){
    end = start + strData.size()-1;
  }
  if(str.size() < (end+1)){
    str.resize(end+1);
  }
  str.replace(start, end-start+1, strData);
}
