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
#include "Mdt/ItemModel/ColumnList.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/Sql/Schema/Table.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Error.h"
#include <QAbstractItemModel>
#include <QStringList>
#include <QSqlError>

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

bool AbstractSqlTableModelController::setTableName(const QString & name)
{
  Q_ASSERT(model() != nullptr);

  model()->setTable(name);
  const auto sqlError = model()->lastError();
  if(sqlError.isValid()){
    const QString msg = tr("Setting table '%1' failed.").arg(name);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryModelQ(model(), this) );
    error.commit();
    setLastError(error);
    return false;
  }
  const Sql::Schema::Driver driver(model()->database());
  if(!setPrimaryKeyFromTable(driver, name)){
    return false;
  }
  if(!setForeignKeysFromTable(driver, name)){
    return false;
  }
  setEntityName(name);

  return true;
}

bool AbstractSqlTableModelController::setTable(const Sql::Schema::Table & table)
{
  Q_ASSERT(model() != nullptr);
  return setTableName(table.tableName());
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
    error.stackError( mdtErrorFromQSqlQueryModelQ(m, this) );
    error.commit();
    setLastError(error);
    return false;
  }

  return true;
}

bool AbstractSqlTableModelController::submitChangesToStorage()
{
  Q_ASSERT(model() != nullptr);

  if(!model()->submitAll()){
    QString msg = tr("For table '%1': submitting changes to database failed.").arg(model()->tableName());
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryModelQ(model(), this) );
    error.commit();
    setLastError(error);
    return false;
  }

  return true;
}

void AbstractSqlTableModelController::revertChangesFromStorage()
{
  Q_ASSERT(model() != nullptr);
  model()->revertAll();
}

bool AbstractSqlTableModelController::removeSelectedRows()
{
  auto *model = modelForView();
  Q_ASSERT(model != nullptr);
  const auto rowList = getSelectedRows();

  /// \todo Transaction
  for(int row : rowList){
    Q_ASSERT(row >= 0);
    if(!model->removeRow(row)){
      return false;
    }
  }

  return true;
}

bool AbstractSqlTableModelController::setPrimaryKeyFromTable(const Sql::Schema::Driver & driver, const QString & tableName)
{
  Q_ASSERT(driver.isValid());

  const auto pk = driver.getTablePrimaryKeyFromDatabase(tableName);
  if(!pk){
    const QString msg = tr("Fetching primary key for table '%1' failed.").arg(tableName);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( pk.error() );
    error.commit();
    setLastError(error);
    return false;
  }
  const auto fieldNameList = pk.value().fieldNameList();
  const auto imPk = ItemModel::PrimaryKey::fromColumnList( getColumnListFromFieldNameList(fieldNameList) );
  setPrimaryKey(imPk);

  return true;
}

bool AbstractSqlTableModelController::setForeignKeysFromTable(const Sql::Schema::Driver& driver, const QString& tableName)
{
  Q_ASSERT(driver.isValid());

  const auto fkList = driver.getTableForeignKeyListFromDatabase(tableName);
  if(!fkList){
    const QString msg = tr("Fetching primary key for table '%1' failed.").arg(tableName);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( fkList.error() );
    error.commit();
    setLastError(error);
    return false;
  }
  removeAllForeignKeys();
  for(const auto & fk : fkList.value()){
    const auto fieldNameList = fk.fieldNameList();
    const auto imFk = ItemModel::ForeignKey::fromColumnList( getColumnListFromFieldNameList(fieldNameList) );
    addForeignKey(fk.foreignTableName(),  imFk);
  }

  return true;
}

ItemModel::ColumnList AbstractSqlTableModelController::getColumnListFromFieldNameList(const QStringList& fieldNameList) const
{
  ItemModel::ColumnList columns;

  const auto *m = model();
  Q_ASSERT(m != nullptr);

  for(const auto & fieldName : fieldNameList){
    Q_ASSERT(m->fieldIndex(fieldName) >= 0);
    columns.append( m->fieldIndex(fieldName) );
  }

  return columns;
}

}} // namespace Mdt{ namespace ItemEditor{
