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

#include <QDebug>

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

bool mdtFieldMap::updateItem(mdtFieldMapItem *item)
{
  Q_ASSERT(item != 0);

  QList<mdtFieldMapItem*> items;

  if(item->fieldIndex() > -1){
    pvItemsByFieldIndex.insert(item->fieldIndex(), item);
  }
  if(!item->fieldName().isEmpty()){
    pvItemsByFieldName.insert(item->fieldName(), item);
  }
  if(!item->fieldDisplayText().isEmpty()){
    pvItemsByDisplayText.insert(item->fieldDisplayText(), item);
  }
  if(item->sourceFieldIndex() > -1){
    items = pvItemsBySourceFieldIndex.values(item->sourceFieldIndex());
    if(items.size() > 0){
      Q_ASSERT(items.at(0) != 0);
      pvItemsBySourceFieldIndex.remove(items.at(0)->sourceFieldIndex());
    }
    pvItemsBySourceFieldIndex.insert(item->sourceFieldIndex(), item);
  }
  
  if(!item->sourceFieldName().isEmpty()){
    items = pvItemsBySourceFieldName.values(item->sourceFieldName());
    if(items.size() > 0){
      Q_ASSERT(items.at(0) != 0);
      pvItemsBySourceFieldName.remove(items.at(0)->sourceFieldName());
    }
    pvItemsBySourceFieldName.insert(item->sourceFieldName(), item);
  }

  return true;
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

QList<mdtFieldMapItem*> mdtFieldMap::itemsAtSourceFieldIndex(int index)
{
  return pvItemsBySourceFieldIndex.values(index);
}

QList<mdtFieldMapItem*> mdtFieldMap::itemsAtSourceFieldName(const QString &name)
{
  return pvItemsBySourceFieldName.values(name);
}

/// \todo add conversion
QVariant mdtFieldMap::dataForFieldIndex(const QStringList &sourceData, int fieldIndex)
{
  ///qDebug() << "REQ data for index " << fieldIndex << " , sourceData: " << sourceData;
  mdtFieldMapItem *item = pvItemsByFieldIndex.value(fieldIndex, 0);
  if(item == 0){
    return QVariant();
  }
  ///qDebug() << "Found item, source index: " << item->sourceFieldIndex();
  if(item->sourceFieldIndex() < 0){
    return QVariant();
  }
  if(item->sourceFieldIndex() >= sourceData.size()){
    return QVariant();
  }
  if((item->sourceFieldDataStartOffset()<0)&&(item->sourceFieldDataEndOffset()<0)){
    return sourceData.at(item->sourceFieldIndex());
  }
  if(item->sourceFieldDataStartOffset()<0){
    QString src = sourceData.at(item->sourceFieldIndex()).left(item->sourceFieldDataEndOffset()+1);
    return src;
  }
  if(item->sourceFieldDataEndOffset()<0){
    QString src = sourceData.at(item->sourceFieldIndex());
    src = src.right(src.size() - item->sourceFieldDataStartOffset());
    return src;
  }
  QString src = sourceData.at(item->sourceFieldIndex());
  src = src.mid(item->sourceFieldDataStartOffset(), item->sourceFieldDataEndOffset() - item->sourceFieldDataStartOffset() + 1);
  ///return src;
  return QVariant(src).convert(item->dataType());
}

QVariant mdtFieldMap::dataForFieldName(const QStringList &sourceData, const QString &fieldName)
{
  mdtFieldMapItem *item = pvItemsByFieldName.value(fieldName, 0);
  if(item == 0){
    return QVariant();
  }
  return dataForFieldIndex(sourceData, item->fieldIndex());
}

QVariant mdtFieldMap::dataForDisplayText(const QStringList &sourceData, const QString &displayText)
{
  mdtFieldMapItem *item = pvItemsByDisplayText.value(displayText, 0);
  if(item == 0){
    return QVariant();
  }
  return dataForFieldIndex(sourceData, item->fieldIndex());
}

QString mdtFieldMap::dataForSourceFieldIndex(const QList<QVariant> &data, int sourceFieldIndex)
{
  int i;
  QString str;
  QList<mdtFieldMapItem*> items = pvItemsBySourceFieldIndex.values(sourceFieldIndex);
  mdtFieldMapItem *item;

  for(i=0; i<items.size(); i++){
    item = items.at(i);
    Q_ASSERT(item != 0);
    if((item->fieldIndex()>=0)&&(item->fieldIndex()<data.size())){
      insertDataIntoSourceString(str, data.at(item->fieldIndex()), item);
    }
  }

  return str;
}

QString mdtFieldMap::dataForSourceFieldName(const QList<QVariant> &data, const QString &sourceFieldName)
{
  int i;
  QString str;
  QList<mdtFieldMapItem*> items = pvItemsBySourceFieldName.values(sourceFieldName);
  mdtFieldMapItem *item;

  for(i=0; i<items.size(); i++){
    item = items.at(i);
    Q_ASSERT(item != 0);
    if((item->fieldIndex()>=0)&&(item->fieldIndex()<data.size())){
      insertDataIntoSourceString(str, data.at(item->fieldIndex()), item);
    }
  }

  return str;
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
