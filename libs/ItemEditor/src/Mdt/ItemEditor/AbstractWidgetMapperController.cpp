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
#include "AbstractWidgetMapperController.h"
// #include "MappedWidgetList.h"
#include <QDataWidgetMapper>
#include <QModelIndex>
#include <QAbstractItemDelegate>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QVariant>
#include <QByteArray>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractWidgetMapperController::AbstractWidgetMapperController(QDataWidgetMapper *mapper, QObject* parent)
 : AbstractController(parent),
   pvWidgetMapper(mapper)
{
  Q_ASSERT(pvWidgetMapper != nullptr);
  pvWidgetMapper->setParent(this);
  pvWidgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
  /*
   * Replace delegate with our proxy delegate.
   * QDataWidgetMapper uses QItemDelegate,
   * we replace it with a QStyledItemDelegate.
   */
  /**
  auto *delegate = pvWidgetMapper->itemDelegate();
  auto *proxyDelegate = new EditionStartEventCatchDelegate(pvWidgetMapper);
  delegate = new QStyledItemDelegate(pvWidgetMapper);
  proxyDelegate->setItemDelegate(delegate);
  pvWidgetMapper->setItemDelegate(proxyDelegate);
  */
  ///connect(proxyDelegate, &EditionStartEventCatchDelegate::dataEditionStarted, this, &AbstractWidgetMapperController::onDataEditionStarted);

  connect(this, &AbstractWidgetMapperController::currentRowChanged, pvWidgetMapper, &QDataWidgetMapper::setCurrentIndex);
//   pvMappedWidgetList = new MappedWidgetList(this);
//   connect(this, &AbstractWidgetMapperController::rowStateChanged, pvMappedWidgetList, &MappedWidgetList::setRowState);
  connect(this, &AbstractWidgetMapperController::rowStateChanged, this, &AbstractWidgetMapperController::clearWidgetsDataOnInvalidRowState);
}

void AbstractWidgetMapperController::setModel(QAbstractItemModel* _model)
{
  Q_ASSERT(_model != nullptr);
//   Q_ASSERT_X(pvMappedWidgetList->isEmpty(), "AbstractWidgetMapperController::setModel()", "setting model while widgets are allready mapped is not allowed");

  referenceItemModel(_model);
  pvWidgetMapper->setModel(_model);
  registerItemModel();
//   pvMappedWidgetList->setModel(_model);
  Q_ASSERT(model() == pvWidgetMapper->model());
}

void AbstractWidgetMapperController::addMapping(QWidget* widget, int column)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT_X(model() != nullptr, "AbstractWidgetMapperController::addMapping()", "model must be set before mapping widgets");

//   pvMappedWidgetList->addWidget(widget, column);
  pvWidgetMapper->addMapping(widget, column);
  updateWidgetData(widget, column);
  connectUserPropertyNotifySignal(widget, ConnectAction::Connect);
}

void AbstractWidgetMapperController::addMapping(QWidget* widget, int column, const QByteArray& propertyName)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT_X(model() != nullptr, "AbstractWidgetMapperController::addMapping()", "model must be set before mapping widgets");

//   pvMappedWidgetList->addWidget(widget, column);
  pvWidgetMapper->addMapping(widget, column, propertyName);
  updateWidgetData(widget, column);
//   connectUserPropertyNotifySignal(widget, ConnectAction::Connect);
}

void AbstractWidgetMapperController::clearMapping()
{
  pvWidgetMapper->clearMapping();
  disconnectMappedWidgetsUserPropertyNotifySignal();
//   pvMappedWidgetList->clear();
}

void AbstractWidgetMapperController::clearWidgetsDataOnInvalidRowState(RowState rs)
{
  if(!rs.isNull()){
    return;
  }
//   for(int i = 0; i < pvMappedWidgetList->size(); ++i){
//     updateWidgetData(pvMappedWidgetList->at(i), -1);
//   }
}

void AbstractWidgetMapperController::editorNotify()
{
  qDebug() << "Editor notify";
}

void AbstractWidgetMapperController::connectUserPropertyNotifySignal(QWidget*const widget, ConnectAction ca)
{
  Q_ASSERT(metaObject() != nullptr);
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(widget->metaObject() != nullptr);

  // Find widget's user property notify signal
  QMetaMethod notifySignal = widget->metaObject()->userProperty().notifySignal();
  // Get QMetaMethod of AbstractController::onDataEditionStarted()
  ///int slotIndex = metaObject()->indexOfSlot("onDataEditionStarted()");
  int slotIndex = metaObject()->indexOfSlot("editorNotify()");
  Q_ASSERT(slotIndex >= 0);
  QMetaMethod controllerSlot = metaObject()->method(slotIndex);
  // (dis)connect
  if(ca == ConnectAction::Connect){
    connect(widget, notifySignal, this, controllerSlot);
  }else{
    disconnect(widget, notifySignal, this, controllerSlot);
  }
}

void AbstractWidgetMapperController::disconnectMappedWidgetsUserPropertyNotifySignal()
{
//   for(int i = 0; i < pvMappedWidgetList->size(); ++i){
//     connectUserPropertyNotifySignal(pvMappedWidgetList->at(i), ConnectAction::Disctonnect);
//   }
}

void AbstractWidgetMapperController::updateWidgetData(QWidget*const widget, int column)
{
  Q_ASSERT(widget != nullptr);

  auto *delegate = pvWidgetMapper->itemDelegate();
  Q_ASSERT(delegate != nullptr);
  auto *model = pvWidgetMapper->model();
  Q_ASSERT(model != nullptr);
  auto index = model->index(currentRow(), column);
  delegate->setEditorData(widget, index);
}

}} // namespace Mdt{ namespace ItemEditor{
