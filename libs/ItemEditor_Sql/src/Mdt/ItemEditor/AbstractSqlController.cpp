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
#include "AbstractSqlController.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Error.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"

namespace Mdt{ namespace ItemEditor{

AbstractSqlController::AbstractSqlController(QObject* parent)
 : AbstractController(parent)
{
}

// AbstractSqlController::AbstractSqlController(QObject* parent, const QSqlDatabase& db)
//  : AbstractController(parent),
//    mOwningModel(new QSqlTableModel(this, db))
// {
//   mModel = mOwningModel;
// }

// AbstractSqlController::AbstractSqlController(const QSqlDatabase& db)
//  : AbstractSqlController(nullptr, db)
// {
// }

void AbstractSqlController::setDefaultModel(const QSqlDatabase & db)
{
  // If we have to set default model on different database, we must re-alloc
  if(!mOwningModel.isNull()){
    delete mOwningModel;
  }
  Q_ASSERT(mOwningModel.isNull());
  mOwningModel = new QSqlTableModel(this, db);
  mModel = mOwningModel;
  registerModel(mModel);
}

void AbstractSqlController::setModel(QSqlTableModel* model)
{
  Q_ASSERT(model != nullptr);

  if(!mOwningModel.isNull()){
    delete mOwningModel;
  }
  Q_ASSERT(mOwningModel.isNull());
  mModel = model;
  registerModel(model);
}

void AbstractSqlController::setTableName(const QString& name)
{
  Q_ASSERT(!mModel.isNull());
  mModel->setTable(name);
}

void AbstractSqlController::setTable(const Sql::Schema::Table & table)
{
  setTableName(table.tableName());
}

bool AbstractSqlController::select()
{
  Q_ASSERT(!mModel.isNull());

  if(!ControllerStatePermission::canSelect(controllerState())){
    /// \todo Update last error ?
    return false;
  }
  if(mModel->isDirty()){
    /// \todo Update last error ?
    return false;
  }
  if(!mModel->select()){
    QString msg = tr("For table '%1': select failed.").arg(mModel->tableName());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryModelQ(*mModel, this) );
    setLastError(error);
    return false;
  }

  return true;
}

// void AbstractSqlController::registerOwningModelIfApplicable()
// {
//   if(mOwningModel.isNull()){
//     return;
//   }
//   if(registeredModel() == mOwningModel){
//     return;
//   }
//   mModel = mOwningModel;
//   registerModel(mModel);
// }

}} // namespace Mdt{ namespace ItemEditor{
