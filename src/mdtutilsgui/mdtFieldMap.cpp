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

  if(!pvItems.contains(item)){
    pvItems.append(item);
  }
  updateItem(item);
  /**
  pvItemsByFieldIndex.insert(item->fieldIndex(), item);
  pvItemsByFieldName.insert(item->fieldName(), item);
  pvItemsByDisplayText.insert(item->fieldDisplayText(), item);
  pvItemsBySourceFieldIndex.insert(item->sourceFieldIndex(), item);
  pvItemsBySourceFieldName.insert(item->sourceFieldName(), item);
  */
}

void mdtFieldMap::clear()
{
  ///qDebug() << "clear() , deleting: " << pvItems;
  qDeleteAll(pvItems);
  pvItems.clear();
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
  int i;
  
  QList<int> keysByIndex;
  QList<QString> keysByName;

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
    ///qDebug() << "Edit by SRC FLD IDX, items (0): " << pvItemsBySourceFieldIndex;
    // We have a multi hash here, erase all corresponding items first
    keysByIndex = pvItemsBySourceFieldIndex.keys(item);
    Q_ASSERT(keysByIndex.size() <= 1);
    for(i=0; i<keysByIndex.size(); i++){
      pvItemsBySourceFieldIndex.remove(keysByIndex.at(i));
    }
    ///qDebug() << "Edit by SRC FLD IDX, items (1): " << pvItemsBySourceFieldIndex;
    pvItemsBySourceFieldIndex.insert(item->sourceFieldIndex(), item);
    ///qDebug() << "Edit by SRC FLD IDX, items (2): " << pvItemsBySourceFieldIndex;
    /**
    items = pvItemsBySourceFieldIndex.values(item->sourceFieldIndex());
    if(items.size() > 0){
      pvItemsBySourceFieldIndex.remove(item->sourceFieldIndex());
      for(i=0; i<items.size(); i++){
        Q_ASSERT(items.at(i) != 0);
        items.at(i)->setSourceFieldIndex(item->sourceFieldIndex());
        pvItemsBySourceFieldIndex.insert(item->sourceFieldIndex(), items.at(i));
      }
    }
    pvItemsBySourceFieldIndex.insert(item->sourceFieldIndex(), item);
    */
  }
  if(!item->sourceFieldName().isEmpty()){
    ///qDebug() << "Edit by SRC FLD NAME, items (0): " << pvItemsBySourceFieldName;
    keysByName = pvItemsBySourceFieldName.keys(item);
    Q_ASSERT(keysByName.size() <= 1);
    for(i=0; i<keysByName.size(); i++){
      pvItemsBySourceFieldName.remove(keysByName.at(i));
    }
    ///qDebug() << "Edit by SRC FLD NAME, items (1): " << pvItemsBySourceFieldName;
    pvItemsBySourceFieldName.insert(item->sourceFieldName(), item);
    ///qDebug() << "Edit by SRC FLD NAME, items (2): " << pvItemsBySourceFieldName;
    /**
    items = pvItemsBySourceFieldName.values(item->sourceFieldName());
    if(items.size() > 0){
      pvItemsBySourceFieldName.remove(item->sourceFieldName());
      for(i=0; i<items.size(); i++){
        Q_ASSERT(items.at(i) != 0);
        items.at(i)->setSourceFieldName(item->sourceFieldName());
        pvItemsBySourceFieldName.insert(item->sourceFieldName(), items.at(i));
      }
    }
    pvItemsBySourceFieldName.insert(item->sourceFieldName(), item);
    */
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
QVariant mdtFieldMap::dataForFieldIndex(const QStringList &sourceData, int fieldIndex) const
{
  QString src;
  QVariant data;

  ///qDebug() << "REQ data for index " << fieldIndex << " , sourceData: " << sourceData;
  mdtFieldMapItem *item = pvItemsByFieldIndex.value(fieldIndex, 0);
  if(item == 0){
    return QVariant();
  }
  if(item->sourceFieldIndex() < 0){
    return QVariant();
  }
  if(item->sourceFieldIndex() >= sourceData.size()){
    return QVariant();
  }
  if((item->sourceFieldDataStartOffset()<0)&&(item->sourceFieldDataEndOffset()<0)){
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
