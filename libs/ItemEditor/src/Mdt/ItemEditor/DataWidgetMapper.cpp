/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

DataWidgetMapper::DataWidgetMapper(QObject* parent)
 : QObject(parent),
   mCurrentRow(-1),
   mUpdatingMappedWidget(false),
   mEditingState(false)
{
  setItemDelegate(new QStyledItemDelegate(this));
}

void DataWidgetMapper::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT_X(mMappedWidgetList.isEmpty(), "DataWidgetMapper::setModel()", "setting model while widgets are allready mapped is not allowed");

  if(model == mModel){
    return;
  }
  if(!mModel.isNull()){
    disconnect(mModel, &QAbstractItemModel::dataChanged, this, &DataWidgetMapper::onModelDataChanged);
    disconnect(mModel, &QAbstractItemModel::modelReset, this, &DataWidgetMapper::onModelReset);
  }
  mModel = model;
  connect(mModel, &QAbstractItemModel::dataChanged, this, &DataWidgetMapper::onModelDataChanged);
  connect(mModel, &QAbstractItemModel::modelReset, this, &DataWidgetMapper::onModelReset);
  setCurrentRow(-1);
}

QAbstractItemModel* DataWidgetMapper::model() const
{
  return mModel;
}

void DataWidgetMapper::setItemDelegate(QAbstractItemDelegate* delegate)
{
  Q_ASSERT(delegate != nullptr);

//   if(!pvDelegate.isNull()){
//     disconnect(pvDelegate, &QAbstractItemDelegate::commitData, this, &DataWidgetMapper::onEditorCommitData);
//     disconnect(pvDelegate, &QAbstractItemDelegate::closeEditor, this, &DataWidgetMapper::onCloseEditor);
//   }
  mDelegate = delegate;
//   connect(pvDelegate, &QAbstractItemDelegate::commitData, this, &DataWidgetMapper::onEditorCommitData);
//   connect(pvDelegate, &QAbstractItemDelegate::closeEditor, this, &DataWidgetMapper::onCloseEditor);
}

QAbstractItemDelegate* DataWidgetMapper::itemDelegate() const
{
  return mDelegate;
}

void DataWidgetMapper::addMapping(QWidget* widget, int column)
{
  Q_ASSERT_X(!mModel.isNull(), "DataWidgetMapper::addMapping()", "model must be set before mapping widgets");
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(column >= 0);
  Q_ASSERT(widget->metaObject() != nullptr);

  mMappedWidgetList.addWidget(widget, column);
  connectUserPropertyNotifySignal(widget, ConnectAction::Connect);
  widget->installEventFilter(mDelegate);
  updateMappedWidget(widget, column);
}

void DataWidgetMapper::clearMapping()
{
  if(!mDelegate.isNull()){
    for(const auto & mw : mMappedWidgetList){
      auto *widget = mw.widget();
      if(widget != nullptr){
        connectUserPropertyNotifySignal(widget, ConnectAction::Disctonnect);
        updateMappedWidget(widget, -1);
        widget->setEnabled(true);
        widget->removeEventFilter(mDelegate);
      }
    }
  }
  mMappedWidgetList.clear();
}

void DataWidgetMapper::setCurrentRow(int row)
{
  bool rowHasChanged = (row != mCurrentRow);

  mCurrentRow = row;
  if(!mEditingState){
    updateAllMappedWidgets();
  }
  if(rowHasChanged){
    emit currentRowChanged(row);
  }
}

bool DataWidgetMapper::submit()
{
  Q_ASSERT(!mModel.isNull());

  if(!commitAllMappedWidgetsData()){
    return false;
  }
  if(!mModel->submit()){
    return false;
  }
  mEditingState = false;
  emit dataEditionDone();

  return true;
}

void DataWidgetMapper::revert()
{
  updateAllMappedWidgets();
  mEditingState = false;
  emit dataEditionDone();
}

void DataWidgetMapper::onDataEditionStarted()
{
  if(mEditingState || mUpdatingMappedWidget){
    return;
  }
  mEditingState = true;
  emit dataEditionStarted();
}

void DataWidgetMapper::onModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &)
{
  if(topLeft.parent().isValid()){
    return;
  }
  /// \todo Range checking seems wrong
  if( (topLeft.row() != mCurrentRow) && (bottomRight.row() != mCurrentRow) ){
    return;
  }
  for(const auto & mw : mMappedWidgetList){
    // Update current widget if its column is in range topLeft, bottomRight
    int column = mw.column();
    if( (column >= topLeft.column()) && (column <= bottomRight.column()) ){
      updateMappedWidget(mw.widget(), column);
    }
  }
}

void DataWidgetMapper::onModelReset()
{
  setCurrentRow(-1);
}

void DataWidgetMapper::connectUserPropertyNotifySignal(QWidget*const widget, DataWidgetMapper::ConnectAction ca)
{
  Q_ASSERT(metaObject() != nullptr);
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(widget->metaObject() != nullptr);

  // Find widget's user property notify signal
  QMetaMethod notifySignal = widget->metaObject()->userProperty().notifySignal();
  // Get QMetaMethod of onDataEditionStarted()
  int slotIndex = metaObject()->indexOfSlot("onDataEditionStarted()");
  Q_ASSERT(slotIndex >= 0);
  QMetaMethod widgetMapperSlot = metaObject()->method(slotIndex);
  // (dis)connect
  if(ca == ConnectAction::Connect){
    connect(widget, notifySignal, this, widgetMapperSlot);
  }else{
    disconnect(widget, notifySignal, this, widgetMapperSlot);
  }
}

void DataWidgetMapper::updateMappedWidget(QWidget*const widget, int column)
{
  Q_ASSERT(!mModel.isNull());
  Q_ASSERT(!mDelegate.isNull());

  if(widget == nullptr){
    return;
  }
  mUpdatingMappedWidget = true;
  auto index = mModel->index(mCurrentRow, column);
  mDelegate->setEditorData(widget, index);
  /*
   * On invalid index, widget must allways be disabled,
   * else, if editable flag is not set, and widget has not editable property,
   * it is also disabled
   */
  if(index.isValid()){
    const bool editable = (mModel->flags(index) & Qt::ItemIsEditable);
    if(mEditablePropertyMap.setWidgetEditable(widget, editable)){
      widget->setEnabled(true);
    }else{
      widget->setEnabled(editable);
    }
  }else{
    widget->setEnabled(false);
  }
  mUpdatingMappedWidget = false;
}

void DataWidgetMapper::updateAllMappedWidgets()
{
  for(const auto & mw : mMappedWidgetList){
    updateMappedWidget(mw.widget(), mw.column());
  }
}

bool DataWidgetMapper::commitData(QWidget*const widget, int column)
{
  Q_ASSERT(!mModel.isNull());
  Q_ASSERT(!mDelegate.isNull());

  if(widget == nullptr){
    return true;
  }
  auto index = mModel->index(mCurrentRow, column);
  if(index.isValid()){
    mDelegate->setModelData(widget, mModel, index);
  }

  return true;
}

bool DataWidgetMapper::commitAllMappedWidgetsData()
{
  for(const auto & mw : mMappedWidgetList){
    if( !commitData(mw.widget(), mw.column()) ){
      return false;
    }
  }
  return true;
}

}} // namespace Mdt{ namespace ItemEditor{
