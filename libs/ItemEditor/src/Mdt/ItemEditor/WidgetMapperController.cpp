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
#include "WidgetMapperController.h"
#include "DataWidgetMapper.h"

#include "AbstractControllerStatePermission.h"

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

WidgetMapperController::WidgetMapperController(QObject* parent)
 : AbstractItemModelController(parent),
   mWidgetMapper(new DataWidgetMapper(this))
{
  setControllerStatePermission( ControllerStatePermission::make<AbstractControllerStatePermission>() );
  connect(this, &WidgetMapperController::currentRowChanged, mWidgetMapper, &DataWidgetMapper::setCurrentRow);
  connect(mWidgetMapper, &DataWidgetMapper::dataEditionStarted, this, &WidgetMapperController::onDataEditionStarted);
  connect(mWidgetMapper, &DataWidgetMapper::dataEditionDone, this, &WidgetMapperController::onDataEditionDone);
}

void WidgetMapperController::addMapping(QWidget* widget, int column)
{
  Q_ASSERT(widget != nullptr);
//   Q_ASSERT_X(model() != nullptr, "WidgetMapperController::addMapping()", "model must be set before mapping widgets");

  mWidgetMapper->addMapping(widget, column);
}

void WidgetMapperController::clearMapping()
{
  mWidgetMapper->clearMapping();
}

void WidgetMapperController::setModelToView(QAbstractItemModel* _model)
{
  Q_ASSERT(_model != nullptr);

  mWidgetMapper->setModel(_model);
  modelSetToView();
//   Q_ASSERT(model() == mWidgetMapper->model());
}

bool WidgetMapperController::submitDataToModel()
{
  return mWidgetMapper->submit();
}

void WidgetMapperController::revertDataFromModel()
{
  mWidgetMapper->revert();
}

}} // namespace Mdt{ namespace ItemEditor{
