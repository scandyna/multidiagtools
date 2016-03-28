/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "RecordFormatSetupWidget.h"
#include "RecordFormatSetupWidgetItem.h"
#include <QHBoxLayout>
#include <QSignalMapper>

#include <QDebug>

namespace mdt{ namespace csv{

RecordFormatSetupWidget::RecordFormatSetupWidget(QWidget *parent)
 : QWidget(parent),
   pvDefaultFieldType(QMetaType::QString),
   pvLayout(new QHBoxLayout),
   pvSignalMapper(new QSignalMapper(this))
{
  pvLayout->setSpacing(0);
  setLayout(pvLayout);
  connect(pvSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped),
          this, &RecordFormatSetupWidget::onFieldTypeChanged);
}

void RecordFormatSetupWidget::setDefaultFieldType(QMetaType::Type type)
{
  pvDefaultFieldType = type;
}

void RecordFormatSetupWidget::setHeader(const mdtCsvRecord & header)
{
  setItemsCount(header.count());
  for(int i = 0; i < header.count(); ++i){
    setFieldNameAt(i, header.columnDataList.at(i).toString());
    setFieldTypeAt(i, pvDefaultFieldType);
  }
}

void RecordFormatSetupWidget::onFieldTypeChanged(int index)
{
  qDebug() << "Field type changed for index " << index;
}

void RecordFormatSetupWidget::setItemsCount(int n)
{
  int toAddCount = n - pvItems.size();
  int toRemoveCount = pvItems.size() - n;

  qDebug() << "Layout count: " << pvLayout->count();
  qDebug() << "to add: " << toAddCount << " , to remove: " << toRemoveCount;
  if(toAddCount > 0){
    Q_ASSERT(toRemoveCount < 1);
    addItems(toAddCount);
  }
  if(toRemoveCount > 0){
    Q_ASSERT(toAddCount < 1);
    removeItems(toRemoveCount);
  }
  qDebug() << "Layout count: " << pvLayout->count();
}

void RecordFormatSetupWidget::addItems(int n)
{
  Q_ASSERT(n > 0);

  for(int i = 0; i < n; ++i){
    auto *item = new RecordFormatSetupWidgetItem;
    pvItems.push_back(item);
    pvLayout->addWidget(item);
    connect(item, &RecordFormatSetupWidgetItem::fieldTypeChanged,
            pvSignalMapper,  static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map) );
    pvSignalMapper->setMapping(item, pvItems.size()-1);
  }
}

void RecordFormatSetupWidget::removeItems(int n)
{
  Q_ASSERT(n > 0);
  Q_ASSERT(n <= (int)pvItems.size());

  int i = pvItems.size() - 1;
  Q_ASSERT(i >= 0);
  for( ; n > 0; --n,--i){
    auto *item = pvItems[i];
    Q_ASSERT(item != nullptr);
    pvLayout->removeWidget(item);
    delete item;
    pvItems.pop_back();
  }
}

void RecordFormatSetupWidget::setFieldNameAt(int index, const QString &name)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < (int)pvItems.size());

  qDebug() << "setFieldNameAt(" << index << ", " << name << ")";
  auto *item = pvItems[index];
  Q_ASSERT(item != nullptr);
  item->setFieldName(name);
}

void RecordFormatSetupWidget::setFieldTypeAt(int index, QMetaType::Type type)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < (int)pvItems.size());
  auto *item = pvItems[index];
  Q_ASSERT(item != nullptr);
  item->setFieldType(type);
}

}} // namespace mdt{ namespace csv{
