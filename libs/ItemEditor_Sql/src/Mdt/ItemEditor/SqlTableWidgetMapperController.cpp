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
#include "SqlTableWidgetMapperController.h"
#include "Mdt/ItemEditor/DataWidgetMapper.h"
#include "Mdt/ItemEditor/WidgetMapperControllerStateTable.h"
#include "Mdt/ItemEditor/WidgetMapperControllerStatePermission.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/ItemEditor/ControllerStatePermissionProxyModel.h"

namespace Mdt{ namespace ItemEditor{

SqlTableWidgetMapperController::SqlTableWidgetMapperController(QObject* parent)
 : AbstractSqlTableModelController(parent),
   mWidgetMapper( new DataWidgetMapper(this) )
{
  setControllerStateMachine( ControllerStateMachine::makeNew<WidgetMapperControllerStateTable, WidgetMapperControllerStatePermission>(this) );
  connect( getControllerStatePermissionProxyModel(), &ControllerStatePermissionProxyModel::flagsChanged, mWidgetMapper, &DataWidgetMapper::updateFromModelFlags );
  connect(this, &SqlTableWidgetMapperController::currentRowToBeSet, mWidgetMapper, &DataWidgetMapper::setCurrentRow);
  connect(mWidgetMapper, &DataWidgetMapper::dataEditionStarted, this, &SqlTableWidgetMapperController::onDataEditionStarted);
  connect(mWidgetMapper, &DataWidgetMapper::dataEditionDone, this, &SqlTableWidgetMapperController::onDataEditionDone);
}

void SqlTableWidgetMapperController::addMapping(QWidget* widget, int column)
{
  Q_ASSERT(widget != nullptr);

  mWidgetMapper->addMapping(widget, column);
}

void SqlTableWidgetMapperController::clearMapping()
{
  mWidgetMapper->clearMapping();
}

void SqlTableWidgetMapperController::setModelToView(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mWidgetMapper->setModel(model);
  modelSetToView();
}

bool SqlTableWidgetMapperController::setDataToModel()
{
  return mWidgetMapper->setDataToModel();
}

void SqlTableWidgetMapperController::revertDataFromModel()
{
  mWidgetMapper->revertDataFromModel();
}

}} // namespace Mdt{ namespace ItemEditor{
