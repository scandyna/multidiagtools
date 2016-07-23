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
#include "DataWidgetMapper.h"

#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QByteArray>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

DataWidgetMapper::DataWidgetMapper(QObject* parent)
 : QObject(parent),
   pvCurrentRow(-1),
   pvUpdatingMappedWidget(false),
   pvEditionStartNotified(false)
{
  setItemDelegate(new QStyledItemDelegate(this));
}

void DataWidgetMapper::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT_X(pvMappedWidgetList.isEmpty(), "DataWidgetMapper::setModel()", "setting model while widgets are allready mapped is not allowed");

  if(model == pvModel){
    return;
  }
  if(!pvModel.isNull()){
    disconnect(pvModel, &QAbstractItemModel::dataChanged, this, &DataWidgetMapper::onModelDataChaged);
  }
  pvModel = model;
  connect(pvModel, &QAbstractItemModel::dataChanged, this, &DataWidgetMapper::onModelDataChaged);
  pvCurrentRow = -1;
  updateAllMappedWidgets();
}

QAbstractItemModel* DataWidgetMapper::model() const
{
  return pvModel;
}

void DataWidgetMapper::setItemDelegate(QAbstractItemDelegate* delegate)
{
  Q_ASSERT(delegate != nullptr);

//   if(!pvDelegate.isNull()){
//     disconnect(pvDelegate, &QAbstractItemDelegate::commitData, this, &DataWidgetMapper::onEditorCommitData);
//     disconnect(pvDelegate, &QAbstractItemDelegate::closeEditor, this, &DataWidgetMapper::onCloseEditor);
//   }
  pvDelegate = delegate;
//   connect(pvDelegate, &QAbstractItemDelegate::commitData, this, &DataWidgetMapper::onEditorCommitData);
//   connect(pvDelegate, &QAbstractItemDelegate::closeEditor, this, &DataWidgetMapper::onCloseEditor);
}

QAbstractItemDelegate* DataWidgetMapper::itemDelegate() const
{
  return pvDelegate;
}

void DataWidgetMapper::addMapping(QWidget* widget, int column)
{
  Q_ASSERT_X(!pvModel.isNull(), "DataWidgetMapper::addMapping()", "model must be set before mapping widgets");
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(column >= 0);

  pvMappedWidgetList.addWidget(widget, column);
  connectUserPropertyNotifySignal(widget, ConnectAction::Connect);
  widget->installEventFilter(pvDelegate);
  updateMappedWidget(widget, column);
}

void DataWidgetMapper::clearMapping()
{
  if(!pvDelegate.isNull()){
    for(const auto & mw : pvMappedWidgetList){
      auto *widget = mw.widget();
      if(widget != nullptr){
        connectUserPropertyNotifySignal(widget, ConnectAction::Disctonnect);
        updateMappedWidget(widget, -1);
        widget->setEnabled(true);
        widget->removeEventFilter(pvDelegate);
      }
    }
  }
  pvMappedWidgetList.clear();
}

void DataWidgetMapper::setCurrentRow(int row)
{
  bool rowHasChanged = (row != pvCurrentRow);

  pvCurrentRow = row;
  updateAllMappedWidgets();
  if(rowHasChanged){
    emit currentRowChanged(row);
  }
}

bool DataWidgetMapper::submit()
{
  Q_ASSERT(!pvModel.isNull());

  if(!commitAllMappedWidgetsData()){
    return false;
  }
  if(!pvModel->submit()){
    return false;
  }
  pvEditionStartNotified = false;
  emit dataEditionDone();

  return true;
}

void DataWidgetMapper::revert()
{
  updateAllMappedWidgets();
  pvEditionStartNotified = false;
  emit dataEditionDone();
}

void DataWidgetMapper::onDataEditionStarted()
{
  if(pvEditionStartNotified || pvUpdatingMappedWidget){
    return;
  }
  pvEditionStartNotified = true;
  emit dataEditionStarted();
}

void DataWidgetMapper::onModelDataChaged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &)
{
  if(topLeft.parent().isValid()){
    return;
  }
  if( (topLeft.row() != pvCurrentRow) && (bottomRight.row() != pvCurrentRow) ){
    return;
  }
  for(const auto & mw : pvMappedWidgetList){
    // Update current widget if its column is in range topLeft, bottomRight
    int column = mw.column();
    if( (column >= topLeft.column()) && (column <= bottomRight.column()) ){
      updateMappedWidget(mw.widget(), column);
    }
  }
}

void DataWidgetMapper::connectUserPropertyNotifySignal(QWidget*const widget, DataWidgetMapper::ConnectAction ca)
{
  Q_ASSERT(metaObject() != nullptr);
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(widget->metaObject() != nullptr);

  // Find widget's user property notify signal
  QMetaMethod notifySignal = widget->metaObject()->userProperty().notifySignal();
  // Get QMetaMethod of AbstractController::onDataEditionStarted()
  int slotIndex = metaObject()->indexOfSlot("onDataEditionStarted()");
  Q_ASSERT(slotIndex >= 0);
  QMetaMethod controllerSlot = metaObject()->method(slotIndex);
  // (dis)connect
  if(ca == ConnectAction::Connect){
    connect(widget, notifySignal, this, controllerSlot);
  }else{
    disconnect(widget, notifySignal, this, controllerSlot);
  }
}

void DataWidgetMapper::updateMappedWidget(QWidget * const widget, int column)
{
  Q_ASSERT(!pvModel.isNull());
  Q_ASSERT(!pvDelegate.isNull());

  if(widget == nullptr){
    return;
  }
  pvUpdatingMappedWidget = true;
  auto index = pvModel->index(pvCurrentRow, column);
  pvDelegate->setEditorData(widget, index);
  widget->setEnabled(index.isValid());
  pvUpdatingMappedWidget = false;
}

void DataWidgetMapper::updateAllMappedWidgets()
{
  for(const auto & mw : pvMappedWidgetList){
    updateMappedWidget(mw.widget(), mw.column());
  }
}

bool DataWidgetMapper::commitData(QWidget*const widget, int column)
{
  Q_ASSERT(!pvModel.isNull());
  Q_ASSERT(!pvDelegate.isNull());

  if(widget == nullptr){
    return true;
  }
  auto index = pvModel->index(pvCurrentRow, column);
  if(!index.isValid()){
    return false;
  }
  pvDelegate->setModelData(widget, pvModel, index);

  return true;
}

bool DataWidgetMapper::commitAllMappedWidgetsData()
{
  for(const auto & mw : pvMappedWidgetList){
    if( !commitData(mw.widget(), mw.column()) ){
      return false;
    }
  }
  return true;
}

}} // namespace Mdt{ namespace ItemEditor{
