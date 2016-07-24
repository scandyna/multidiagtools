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
#include "DataWidgetMapper.h"
#include <QModelIndex>
#include <QAbstractItemDelegate>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QVariant>

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractWidgetMapperController::AbstractWidgetMapperController(DataWidgetMapper *mapper, QObject* parent)
 : AbstractController(parent),
   pvWidgetMapper(mapper)
{
  Q_ASSERT(pvWidgetMapper != nullptr);
  pvWidgetMapper->setParent(this);
  /*
   * Replace delegate with our proxy delegate.
   * DataWidgetMapper uses QItemDelegate,
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

  connect(this, &AbstractWidgetMapperController::currentRowChanged, pvWidgetMapper, &DataWidgetMapper::setCurrentRow);
  connect(pvWidgetMapper, &DataWidgetMapper::dataEditionStarted, this, &AbstractWidgetMapperController::onDataEditionStarted);
  connect(pvWidgetMapper, &DataWidgetMapper::dataEditionDone, this, &AbstractWidgetMapperController::onDataEditionDone);
}

void AbstractWidgetMapperController::setModel(QAbstractItemModel* _model)
{
  Q_ASSERT(_model != nullptr);
//   Q_ASSERT_X(pvMappedWidgetList->isEmpty(), "AbstractWidgetMapperController::setModel()", "setting model while widgets are allready mapped is not allowed");

  referenceItemModel(_model);
  pvWidgetMapper->setModel(_model);
  registerItemModel();
  Q_ASSERT(model() == pvWidgetMapper->model());
}

void AbstractWidgetMapperController::addMapping(QWidget* widget, int column)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT_X(model() != nullptr, "AbstractWidgetMapperController::addMapping()", "model must be set before mapping widgets");

  pvWidgetMapper->addMapping(widget, column);
}

void AbstractWidgetMapperController::clearMapping()
{
  pvWidgetMapper->clearMapping();
}

bool AbstractWidgetMapperController::submitDataToModel()
{
  return pvWidgetMapper->submit();
}

void AbstractWidgetMapperController::revertDataFromModel()
{
  pvWidgetMapper->revert();
}

}} // namespace Mdt{ namespace ItemEditor{
