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
#include "AbstractSqlTableModelController.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Error.h"
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemEditor{

AbstractSqlTableModelController::AbstractSqlTableModelController(QObject* parent)
 : AbstractSqlController(parent)
{
}

bool AbstractSqlTableModelController::canSelect() const
{
  if(controllerStateMachine() == nullptr){
    return false;
  }
  return controllerStateMachine()->canSelect();
}

void AbstractSqlTableModelController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT( qobject_cast<QSqlTableModel*>(model) != nullptr );

  if( (!mOwningModel.isNull()) && (model != mOwningModel) ){
    delete mOwningModel;
  }
  reinterpret_cast<QSqlTableModel*>(model)->setEditStrategy(QSqlTableModel::OnManualSubmit);
  AbstractSqlController::setModel(model);
}

void AbstractSqlTableModelController::setDefaultModel(const QSqlDatabase & db)
{
  // If we have to set default model on different database, we must re-alloc
  if(!mOwningModel.isNull()){
    delete mOwningModel;
  }
  Q_ASSERT(mOwningModel.isNull());
  mOwningModel = new QSqlTableModel(this, db);
  setModel(mOwningModel);
}

QSqlTableModel* AbstractSqlTableModelController::model() const
{
  Q_ASSERT( (AbstractSqlController::model() == nullptr) || (qobject_cast<QSqlTableModel*>(AbstractSqlController::model()) != nullptr) );

  return reinterpret_cast<QSqlTableModel*>(AbstractSqlController::model());
}

void AbstractSqlTableModelController::setTableName(const QString & name)
{
  Q_ASSERT(model() != nullptr);
  model()->setTable(name);
}

void AbstractSqlTableModelController::setTable(const Sql::Schema::Table & table)
{
  Q_ASSERT(model() != nullptr);
  setTableName(table.tableName());
}

bool AbstractSqlTableModelController::select()
{
  auto *m = model();
  Q_ASSERT(m != nullptr);

  if(!canSelect()){
    /// \todo Update last error ?
    return false;
  }
  if(m->isDirty()){
    /// \todo Update last error ?
    return false;
  }
  if(!m->select()){
    QString msg = tr("For table '%1': select failed.").arg(m->tableName());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryModelQ(*m, this) );
    setLastError(error);
    return false;
  }

  return true;
}

bool AbstractSqlTableModelController::submitChangesToStorage()
{
  Q_ASSERT(model() != nullptr);
  return model()->submitAll();
}

void AbstractSqlTableModelController::revertChangesFromStorage()
{
  Q_ASSERT(model() != nullptr);
  model()->revertAll();
}

}} // namespace Mdt{ namespace ItemEditor{
