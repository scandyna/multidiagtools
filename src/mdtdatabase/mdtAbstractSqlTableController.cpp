/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtAbstractSqlTableController.h"
#include "mdtState.h"
#include "mdtSqlRelation.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
#include <QCoreApplication>
#include <QPair>
#include <QVector>

#include <QDebug>

using namespace std;

mdtAbstractSqlTableController::mdtAbstractSqlTableController(QObject* parent)
 : QObject(parent),
   pvProxyModel(new mdtSortFilterProxyModel)
{
  buildStateMachine();
  pvCurrentRow = -1;
  pvCanWriteToDatabase = true;
}

mdtAbstractSqlTableController::~mdtAbstractSqlTableController()
{
}

void mdtAbstractSqlTableController::setMessageHandler(shared_ptr< mdtUiMessageHandler > handler)
{
  Q_ASSERT(handler);

  int i;

  pvMessageHandler = handler;
  for(i = 0; i < pvDataValidators.size(); ++i){
    Q_ASSERT(pvDataValidators.at(i));
    pvDataValidators.at(i)->setMessageHandler(pvMessageHandler);
  }
}

bool mdtAbstractSqlTableController::hasMessageHandler() const
{
  if(pvMessageHandler){
    return true;
  }else{
    return false;
  }
}

void mdtAbstractSqlTableController::setTableName(const QString& tableName, QSqlDatabase db, const QString& userFriendlyTableName)
{
  shared_ptr<QSqlTableModel> m(new QSqlTableModel(0, db));
  m->setTable(tableName);
  setModel(m, userFriendlyTableName);
}

void mdtAbstractSqlTableController::setModel(shared_ptr< QSqlTableModel > m, const QString& userFriendlyTableName)
{
  Q_ASSERT(m);

  pvModel = m;
  pvProxyModel->setSourceModel(pvModel.get());
  if(userFriendlyTableName.isEmpty()){
    pvUserFriendlyTableName = m->tableName();
  }else{
    pvUserFriendlyTableName = userFriendlyTableName;
  }
  modelSetEvent();
}

void mdtAbstractSqlTableController::setCanWriteToDatabase(bool canWrite)
{
  pvCanWriteToDatabase = canWrite;
}

int mdtAbstractSqlTableController::fieldIndex(const QString& fieldName) const
{
  if(!pvModel){
    return -1;
  }
  return pvModel->fieldIndex(fieldName);
}

void mdtAbstractSqlTableController::setHeaderData(const QString& fieldName, const QString& data)
{
  Q_ASSERT(pvModel);

  int column;

  column = fieldIndex(fieldName);
  if(column < 0){
    return;
  }
  model()->setHeaderData(column, Qt::Horizontal, data);
}

QVariant mdtAbstractSqlTableController::headerData(const QString& fieldName, int role) const
{
  Q_ASSERT(pvModel);

  return headerData(fieldIndex(fieldName), role);
}

bool mdtAbstractSqlTableController::select()
{
  Q_ASSERT(pvModel);
  Q_ASSERT(pvStateMachine.isRunning());

  bool ok;
  int row;

  if(!allDataAreSaved()){
    return false;
  }

  emit selectTriggered();
  pvStateMachine.waitOnState(Selecting);

  // Call select on model
  ok = pvModel->select();
  if(!ok){
    QSqlError sqlError = pvModel->lastError();
    pvLastError.setError(tr("Select failed for") + "'" + pvUserFriendlyTableName + "' (" + pvModel->tableName() + ")" , mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
  }
  // Go to first row
  if(pvModel->rowCount() > 0){
    row = 0;
  }else{
    row = -1;
  }
  // Set current row
  if(!setCurrentRowPv(row)){
    return false;
  }
  updateChildControllersAfterCurrentRowChanged();
  if(ok){
    emit operationSucceed();
    pvStateMachine.waitOnState(Visualizing);
  }
  // Call sort
  sort();

  return ok;
}

bool mdtAbstractSqlTableController::setFilter(const QString& filter)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(pvStateMachine.isRunning());

  int row;

  if(!allDataAreSaved()){
    return false;
  }

  emit selectTriggered();
  pvStateMachine.waitOnState(Selecting);

  // Call select on model
  pvModel->setFilter(filter);
  // Go to first row
  if(pvModel->rowCount() > 0){
    row = 0;
  }else{
    row = -1;
  }
  // Set current row
  if(!setCurrentRowPv(row)){
    return false;
  }
  updateChildControllersAfterCurrentRowChanged();
  emit operationSucceed();
  pvStateMachine.waitOnState(Visualizing);

  return true;
}

bool mdtAbstractSqlTableController::setFilter(const QString& fieldName, const QVariant& matchData)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(pvStateMachine.isRunning());

  QString filter;
  bool ok;

  filter = fieldName + " = " + formatValue(fieldName, matchData, ok);
  if(!ok){
    return false;
  }

  return setFilter(filter);
}

bool mdtAbstractSqlTableController::setFilter(const QString& fieldName, const QList< QVariant >& matchDataList)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(pvStateMachine.isRunning());

  QString filter;
  QSqlField field;
  int i;

  // Get requested field
  field = pvModel->record().field(fieldName);
  if(!field.isValid()){
    pvLastError.setError(tr("Requested field") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Build filter
  filter = fieldName + " IN(";
  for(i = 0; i < matchDataList.size(); ++i){
    field.setValue(matchDataList.at(i));
    filter += formatValue(field);
    if(i < (matchDataList.size()-1)){
      filter += ",";
    }
  }
  filter += ")";

  return setFilter(filter);
}

void mdtAbstractSqlTableController::clearFilter()
{
  Q_ASSERT(pvModel);
  Q_ASSERT(pvStateMachine.isRunning());

  setFilter("");
}

QString mdtAbstractSqlTableController::tableName() const
{
  if(!pvModel){
    return QString();
  }
  return pvModel->tableName();
}

void mdtAbstractSqlTableController::start()
{
  Q_ASSERT(pvModel);

  int i;
  shared_ptr<mdtAbstractSqlTableController> c;

  // Start this controller
  if(pvStateMachine.isRunning()){
    stop();
  }
  pvStateMachine.start(true);
  // Start child controllers
  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    c = pvChildControllerContainers.at(i).controller;
    Q_ASSERT(c);
    c->start();
    if(c->model()){
      c->model()->setFilter("-1");
      if(!c->select()){
        pvLastError = c->lastError();
        if(messageHandler()){
          messageHandler()->setError(pvLastError);
          messageHandler()->displayToUser();
        }
      }
    }
  }
  emit mainWidgetEnableStateChanged(true);
}

void mdtAbstractSqlTableController::stop()
{
  Q_ASSERT(pvModel);

  int i;

  if(!pvStateMachine.isRunning()){
    return;
  }
  emit mainWidgetEnableStateChanged(false);
  // Stop child controllers
  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    Q_ASSERT(pvChildControllerContainers.at(i).controller);
    pvChildControllerContainers.at(i).controller->stop();
  }
  // Stop this controller
  pvStateMachine.stop(true);
}

void mdtAbstractSqlTableController::addDataValidator(shared_ptr< mdtSqlDataValidator > validator, bool putAtTopPriority, bool setInternalMessageHandler)
{
  Q_ASSERT(validator);

  if(setInternalMessageHandler && pvMessageHandler){
    validator->setMessageHandler(pvMessageHandler);
  }
  if(putAtTopPriority){
    pvDataValidators.prepend(validator);
  }else{
    pvDataValidators.append(validator);
  }
}

void mdtAbstractSqlTableController::addColumnToSortOrder(const QString& fieldName, Qt::SortOrder order)
{
  pvProxyModel->addColumnToSortOrder(fieldName, order);
}

void mdtAbstractSqlTableController::clearColumnsSortOrder()
{
  pvProxyModel->clearColumnsSortOrder();
}

void mdtAbstractSqlTableController::sort()
{
  Q_ASSERT(pvModel);

  qDebug() << "mdtAbstractSqlTableController::sort() - table: " << tableName() << " - called ...";
  if(!proxyModel()->hasColumnToSort()){
    return;
  }
  // If model is empty, canFetchMore will allways return true, prevent this
  if(model()->rowCount() < 1){
    return;
  }
  while(model()->canFetchMore()){
    model()->fetchMore();
  }
  // Sort
  qDebug() << "mdtAbstractSqlTableController::sort() - table: " << tableName() << " - sort ...";
  pvProxyModel->sort();
}

bool mdtAbstractSqlTableController::setCurrentData(const QString& fieldName, const QVariant& data, bool submit)
{
  Q_ASSERT(pvModel);

  return setData(currentRow(), fieldName, data, submit);
}

bool mdtAbstractSqlTableController::setData(int row, const QString& fieldName, const QVariant& data, bool submit)
{
  Q_ASSERT(pvModel);

  int column;
  QModelIndex index;

  // Get column of requested field name
  column = pvModel->record().indexOf(fieldName);
  if(column < 0){
    pvLastError.setError(tr("Requested field name") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Get index
  index = pvProxyModel->index(row, column);
  if(!index.isValid()){
    pvLastError.setError(tr("Index with row") + " " + QString::number(row)\
               + " " + tr(", and column") + " " + QString::number(column) + " " + tr("is invalid, table:") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Update model
  pvProxyModel->setData(index, data);
  // Submit if requested
  if(submit && pvCanWriteToDatabase){
    ///return submitAndWait();
    return submitToDatabase();
  }

  return true;
}

bool mdtAbstractSqlTableController::setData(const QString & matchFieldName, const QVariant & matchData, const QString & dataFieldName, const QVariant & data, bool submit)
{
  Q_ASSERT(pvModel);

  int row;
  int matchColumn;

  // Get column for match field
  matchColumn = pvModel->record().indexOf(matchFieldName);
  if(matchColumn < 0){
    pvLastError.setError(tr("Requested field name for matching data") + " '" + matchFieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Get row that matches
  row = firstMatchingRow(matchColumn, matchData);
  if(row < 0){
    return false;
  }

  return setData(row, dataFieldName, data, submit);
}

bool mdtAbstractSqlTableController::setData(const QString& matchFieldName1, const QVariant& key1, const QString& matchFieldName2, const QVariant& key2,
                                            const QString& dataFieldName, const QVariant& data, bool submit)
{
  Q_ASSERT(pvModel);

  int row;
  int matchColumn1;
  int matchColumn2;

  // Get columns for match fields
  matchColumn1 = pvModel->record().indexOf(matchFieldName1);
  if(matchColumn1 < 0){
    pvLastError.setError(tr("Requested field name for key1") + " '" + matchFieldName1 + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  matchColumn2 = pvModel->record().indexOf(matchFieldName2);
  if(matchColumn2 < 0){
    pvLastError.setError(tr("Requested field name for key2") + " '" + matchFieldName2 + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Get row that matches
  row = firstMatchingRow(matchColumn1, key1, matchColumn2, key2);
  if(row < 0){
    return false;
  }

  return setData(row, dataFieldName, data, submit);
}

QVariant mdtAbstractSqlTableController::data(const QModelIndex& index, bool& ok)
{
  Q_ASSERT(pvModel);

  if(!index.isValid()){
    ok = false;
    pvLastError.setError(tr("Index with row") + " " + QString::number(index.row())\
               + " " + tr(", and column") + " " + QString::number(index.column()) + " " + tr("is invalid, table:") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  ok = true;

  return pvProxyModel->data(index);
}

QVariant mdtAbstractSqlTableController::data(int row, const QString& fieldName, bool & ok)
{
  Q_ASSERT(pvModel);

  int column;

  column = pvModel->record().indexOf(fieldName);
  if(column < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }

  return data(row, column, ok);
}

QVariant mdtAbstractSqlTableController::data(const QString & matchFieldName, const QVariant & matchData, const QString & dataFieldName, bool & ok)
{
  Q_ASSERT(pvModel);

  int row;
  int matchColumn;
  int dataColumn;

  // Get column for match field
  matchColumn = pvModel->record().indexOf(matchFieldName);
  if(matchColumn < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name for matching data") + " '" + matchFieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  // Get column for data field
  dataColumn = pvModel->record().indexOf(dataFieldName);
  if(dataColumn < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name") + " '" + dataFieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  // Get row that matches
  row = firstMatchingRow(matchColumn, matchData);
  if(row < 0){
    ok = false;
    return QVariant();
  }

  return data(row, dataColumn, ok);
}

QVariant mdtAbstractSqlTableController::data(const QString& matchFieldName1, const QVariant& key1, const QString& matchFieldName2, const QVariant& key2, const QString& dataFieldName, bool& ok)
{
  Q_ASSERT(pvModel);

  int row;
  int matchColumn1, matchColumn2;
  int dataColumn;

  // Get columns for match fields
  matchColumn1 = pvModel->record().indexOf(matchFieldName1);
  if(matchColumn1 < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name for key1") + " '" + matchFieldName1 + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  matchColumn2 = pvModel->record().indexOf(matchFieldName2);
  if(matchColumn2 < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name for key2") + " '" + matchFieldName2 + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  // Get column for data field
  dataColumn = pvModel->record().indexOf(dataFieldName);
  if(dataColumn < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name") + " '" + dataFieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QVariant();
  }
  // Get row that matches
  row = firstMatchingRow(matchColumn1, key1, matchColumn2, key2);
  if(row < 0){
    ok = false;
    return QVariant();
  }

  return data(row, dataColumn, ok);
}

mdtSqlRecord mdtAbstractSqlTableController::record(int row, const QStringList & fieldNames, bool & ok)
{
  Q_ASSERT(pvModel);

  mdtSqlRecord rec;
  int i;

  for(i = 0; i < fieldNames.size(); ++i){
    if(!rec.addField(fieldNames.at(i), pvModel->tableName(), pvModel->database())){
      ok = false;
      pvLastError = rec.lastError();
      return mdtSqlRecord();
    }
    rec.setValue(fieldNames.at(i), data(row, fieldNames.at(i), ok));
    if(!ok){
      return mdtSqlRecord();
    }
  }
  ok = true;

  return rec;
}

mdtSqlRecord mdtAbstractSqlTableController::record(const QString & matchFieldName, const QVariant & matchData, const QStringList & fieldNames, bool& ok)
{
  Q_ASSERT(pvModel);

  int row;
  int matchColumn;

  // Get column for match field
  matchColumn = pvModel->record().indexOf(matchFieldName);
  if(matchColumn < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name for matching data") + " '" + matchFieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return mdtSqlRecord();
  }
  // Get row that matches
  row = firstMatchingRow(matchColumn, matchData);
  if(row < 0){
    ok = false;
    return mdtSqlRecord();
  }

  return record(row, fieldNames, ok);
}

QList< QVariant > mdtAbstractSqlTableController::dataList(const QString& fieldName, bool& ok, bool fetchAll)
{
  Q_ASSERT(pvModel);

  QList<QVariant> lst;
  QModelIndex index;
  int row, col;

  if(pvModel->rowCount() < 1){
    ok = true;
    return lst;
  }
  // Get column index of reuqested field name
  col = pvModel->record().indexOf(fieldName);
  if(col < 0){
    ok = false;
    pvLastError.setError(tr("Requested field name") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return lst;
  }
  // Fetch all data if requested
  if(fetchAll){
    while(pvModel->canFetchMore()){
      pvModel->fetchMore();
    }
  }
  // Build data list
  for(row = 0; row < proxyModel()->rowCount(); ++row){
    index = proxyModel()->index(row, col);
    lst.append(proxyModel()->data(index));
  }
  ok = true;

  return lst;
}

QString mdtAbstractSqlTableController::formatedValue(int row, const QString& fieldName, bool& ok)
{
  Q_ASSERT(pvModel);

  QSqlField field;

  // Check row
  if((row < 0)||(row >= pvModel->rowCount())){
    ok = false;
    pvLastError.setError(tr("Requested row is out of bound. Row:") + " " + QString::number(row) + " . " + tr("Table:") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QString();
  }
  // Get requested field
  field = pvModel->record(row).field(fieldName);
  if(!field.isValid()){
    ok = false;
    pvLastError.setError(tr("Requested field") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QString();
  }

  return formatValue(field);
}

QString mdtAbstractSqlTableController::formatValue(const QString& fieldName, const QVariant& value, bool& ok)
{
  Q_ASSERT(pvModel);

  QSqlField field;

  // Get requested field and set value to it
  field = pvModel->record().field(fieldName);
  if(!field.isValid()){
    ok = false;
    pvLastError.setError(tr("Requested field") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return QString();
  }
  field.setValue(value);
  ok = true;

  return formatValue(field);
}

QString mdtAbstractSqlTableController::formatValue(const QSqlField& field)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(field.isValid());

  QSqlDriver *sqlDriver;

  // Use SQL driver if available
  sqlDriver = pvModel->database().driver();
  if(sqlDriver != 0){
    return sqlDriver->formatValue(field);
  }
  // Driver not available, try to format value
  QString delimiter;
  switch(field.type()){
    case QVariant::String:
    case QVariant::Char:
      delimiter = "'";
      break;
    case QVariant::Bool:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Double:
      break;

  }
  return delimiter + field.value().toString() + delimiter;
}

int mdtAbstractSqlTableController::rowCount(bool fetchAll) const
{
  if(!pvModel){
    return 0;
  }
  if(pvModel->rowCount() < 1){
    return 0;
  }
  if(fetchAll){
    while(pvModel->canFetchMore()){
      pvModel->fetchMore();
    }
  }
  return pvModel->rowCount();
}

bool mdtAbstractSqlTableController::setCurrentRow(int row)
{
  Q_ASSERT(pvModel);

  // Check that state machine runs
  if(!pvStateMachine.isRunning()){
    pvLastError.setError(tr("Cannot change current row because state machine is stopped. Table:") + " '" + userFriendlyTableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Check about unsaved data
  if(!allDataAreSaved()){
    return false;
  }
  // Update current row
  if(!setCurrentRowPv(row)){
    return false;
  }
  updateChildControllersAfterCurrentRowChanged();

  return true;
}

bool mdtAbstractSqlTableController::setCurrentRow(const QString& fieldName, const QVariant& matchData)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(!matchData.isNull());

  int row;
  int col;

  // Get column of requested field name
  col = pvModel->fieldIndex(fieldName);
  if(col < 0){
    pvLastError.setError(tr("Requested field") + " '" + fieldName + "' " + tr("was not found in table") + " '" + pvModel->tableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  // Get matching row and set current if found
  row = firstMatchingRow(col, matchData);
  if(row < 0){
    return false;
  }

  return setCurrentRow(row);
}

void mdtAbstractSqlTableController::update()
{
  Q_ASSERT(pvModel);

  currentRowChangedEvent(pvCurrentRow);
  updateChildControllersAfterCurrentRowChanged();
}

bool mdtAbstractSqlTableController::allDataAreSaved(bool checkAboutDirtyIndex)
{
  Q_ASSERT(pvModel);

  int i;

  // If current row is < 0 , we have no data to check
  if(currentRow() < 0){
    return true;
  }
  // Check this controller
  if((currentState() != Visualizing)&&(currentState() != Selecting)&&(currentState() != Stopped)){
    if(pvMessageHandler){
      QString msg;
      pvMessageHandler->setType(mdtUiMessageHandler::Warning);
      msg = tr("For table");
      msg += " '" + userFriendlyTableName() + "' ";
      msg += tr(": some data are not saved.");
      pvMessageHandler->setText(msg);
      msg = tr("Please save or cancel modifications and try again.");
      pvMessageHandler->setInformativeText(msg);
      pvMessageHandler->setType(mdtUiMessageHandler::Warning);
      pvMessageHandler->displayToUser();
    }
    return false;
  }
  /*
   * If requested, check about dirty indexes.
   * We take only cached rows, because it's not possible to edit one
   *  that is not cached.
   */
  if(checkAboutDirtyIndex){
    int row, col;
    QModelIndex index;
    for(row = 0; row < pvModel->rowCount(); ++row){
      for(col = 0; col < pvModel->columnCount(); ++col){
        index = pvModel->index(row, col);
        if(pvModel->isDirty(index)){
          // We generate no message, because this situation should only happen for programmed edition with setData()
          return false;
        }
      }
    }
  }
  // Check child controllers
  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    Q_ASSERT(pvChildControllerContainers.at(i).controller);
    if(!pvChildControllerContainers.at(i).controller->allDataAreSaved(checkAboutDirtyIndex)){
      return false;
    }
  }

  return true;
}

bool mdtAbstractSqlTableController::submitAndWait()
{
  Q_ASSERT(pvModel);

  pvLastError.clear();
  pvOperationComplete = false;
  emit submitTriggered();
  waitOperationComplete();
  if(pvLastError.level() != mdtError::NoError){
    return false;
  }

  return true;
}

bool mdtAbstractSqlTableController::removeAndWait()
{
  Q_ASSERT(pvModel);

  pvLastError.clear();
  pvOperationComplete = false;
  emit removeTriggered();
  waitOperationComplete();
  if(pvLastError.level() != mdtError::NoError){
    return false;
  }

  return true;
}

void mdtAbstractSqlTableController::submit()
{
  emit submitTriggered();
}

void mdtAbstractSqlTableController::revert()
{
  emit revertTriggered();
}

void mdtAbstractSqlTableController::insert()
{
  if(!allDataAreSaved()){
    return;
  }
  emit insertTriggered();
}

void mdtAbstractSqlTableController::remove()
{
  emit removeTriggered();
}

bool mdtAbstractSqlTableController::beginTransaction()
{
  Q_ASSERT(pvModel);

  if(!pvModel->database().transaction()){
    QSqlError sqlError = pvModel->database().lastError();
    pvLastError.setError(tr("Cannot beginn transaction (database: '") + pvModel->database().databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtAbstractSqlTableController::rollbackTransaction()
{
  Q_ASSERT(pvModel);

  if(!pvModel->database().rollback()){
    QSqlError sqlError = pvModel->database().lastError();
    pvLastError.setError(tr("Cannot rollback transaction (database: '") + pvModel->database().databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtAbstractSqlTableController::commitTransaction()
{
  Q_ASSERT(pvModel);

  if(!pvModel->database().commit()){
    QSqlError sqlError = pvModel->database().lastError();
    pvLastError.setError(tr("Cannot commit transaction (database: '") + pvModel->database().databaseName() + tr("')."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    return false;
  }
  return true;
}

bool mdtAbstractSqlTableController::setCurrentRowPv(int row)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(pvStateMachine.isRunning());

  // With empty model, we have a special case, baucause fetchMore will allways return true (infinite loop)
  if(pvModel->rowCount() < 1){
    // Send event
    pvCurrentRow = -1;
    currentRowChangedEvent(pvCurrentRow);
    return true;
  }
  // Fetch rows in database if needed
  Q_ASSERT(pvModel->rowCount() > 0);
  while((row >= pvModel->rowCount())&&(pvModel->canFetchMore())){
    pvModel->fetchMore();
  }
  // Check range and send event if OK
  if(row >= pvModel->rowCount()){
    return false;
  }
  pvCurrentRow = row;
  currentRowChangedEvent(pvCurrentRow);

  return true;
}

bool mdtAbstractSqlTableController::submitToDatabase()
{
  Q_ASSERT(model());

  QSqlError sqlError;

  if(!model()->submitAll()){
    sqlError = model()->lastError();
    pvLastError.setError(tr("Submitting data to database failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }

  return true;
}

void mdtAbstractSqlTableController::onStateSelectingEntered()
{
}

void mdtAbstractSqlTableController::onStateVisualizingEntered()
{
  Q_ASSERT(pvModel);

  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit childWidgetEnableStateChanged(true);
  emit insertEnabledStateChanged(true);
  if(currentRow() > -1){
    emit removeEnabledStateChanged(true);
  }
  pvOperationComplete = true;
}

void mdtAbstractSqlTableController::onStateVisualizingExited()
{
  emit childWidgetEnableStateChanged(false);
  emit insertEnabledStateChanged(false);
  emit removeEnabledStateChanged(false);
}

void mdtAbstractSqlTableController::onStateEditingEntered()
{
  emit submitEnabledStateChanged(true);
  emit revertEnabledStateChanged(true);
}

void mdtAbstractSqlTableController::onStateEditingExited()
{
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit stateEditingExited();
}

void mdtAbstractSqlTableController::onStateSubmittingEntered()
{
  if(!checkBeforeSubmit()){
    emit errorOccured();
    return;
  }
  if(!submitToModel()){
    emit errorOccured();
    return;
  }
  if(pvCanWriteToDatabase){
    if(!submitToDatabase()){
      emit errorOccured();
      return;
    }
  }
  /**
  if(!doSubmit()){
    emit errorOccured();
    return;
  }
  */
  /*
   * Go back to row.
   * Calling submitAll() will repopulate the model.
   * Because of this, we must be shure to fetch all data until we find our row
   */
  if(!setCurrentRowPv(pvCurrentRow)){
    emit errorOccured();
    return;
  }
  if(!updateChildControllersAfterSubmit()){
    emit errorOccured();
    return;
  }
  emit operationSucceed();
}

void mdtAbstractSqlTableController::onStateRevertingEntered()
{
  bool doIt;

  // We ask confirmation to the user
  if(pvMessageHandler){
    pvMessageHandler->setText(tr("You choosed to not save modification. This will restore data from database."));
    pvMessageHandler->setInformativeText(tr("Do you really want to loose your modification ?"));
    pvMessageHandler->setType(mdtUiMessageHandler::Warning);
    doIt = pvMessageHandler->askUser();
  }else{
    doIt = true;
  }
  // Do revert
  if(doIt){
    if(doRevert()){
      updateChildControllersAfterRevert();
      emit operationSucceed();
    }else{
      emit errorOccured();
    }
  }else{
    emit errorOccured();  // To exit reverting state
  }
}

void mdtAbstractSqlTableController::onStateInsertingEntered()
{
  // Remember current row, on case of reverting new row
  pvBeforeInsertCurrentRow = pvCurrentRow;
  // Insert new row at end
  pvCurrentRow = rowCount(true);
  if(!pvProxyModel->insertRow(pvCurrentRow)){
    pvLastError.setError(tr("Cannot insert row in table:") + " '" + userFriendlyTableName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
    pvLastError.commit();
    if(pvMessageHandler){
      pvMessageHandler->setError(pvLastError);
      pvMessageHandler->displayToUser();
    }
    pvCurrentRow = pvBeforeInsertCurrentRow;
    emit errorOccured();
  }
  insertDoneEvent(pvCurrentRow);
  updateChildControllersAfterInsert();
  emit operationSucceed();
}

void mdtAbstractSqlTableController::onStateEditingNewRowEntered()
{
  emit submitEnabledStateChanged(true);
  emit revertEnabledStateChanged(true);
}

void mdtAbstractSqlTableController::onStateEditingNewRowExited()
{
  emit submitEnabledStateChanged(false);
  emit revertEnabledStateChanged(false);
  emit stateEditingNewRowExited();
}

void mdtAbstractSqlTableController::onStateSubmittingNewRowEntered()
{
  if(!checkBeforeSubmit()){
    emit errorOccured();
    return;
  }
  /*
   * We temporary disable sorting,
   *  else it is not possible to find
   *  current row after a submit, because model is repopulated,
   *  and also re-sorted
   */
  pvProxyModel->disableSorting();
  if(!submitToModel()){
    emit errorOccured();
    return;
  }
  if(pvCanWriteToDatabase){
    if(!submitToDatabase()){
      emit errorOccured();
      return;
    }
  }
  if(!updateChildControllersAfterSubmitNewRow()){
    emit errorOccured();
    return;
  }
  /*
   * Go back to row.
   * Calling submitAll() will repopulate the model.
   * Because of this, we must be shure to fetch all data until we find our row
   */
  if(!setCurrentRowPv(pvCurrentRow)){
    emit errorOccured();
    return;
  }
  updateChildControllersAfterCurrentRowChanged();
  pvProxyModel->enableSorting();
  emit operationSucceed();
}

void mdtAbstractSqlTableController::onStateRevertingNewRowEntered()
{
  Q_ASSERT(pvModel);

  bool doIt;

  // We ask confirmation to the user
  if(pvMessageHandler){
    pvMessageHandler->setText(tr("You choosed to not save data."));
    pvMessageHandler->setInformativeText(tr("Do you really want to loose new record ?"));
    pvMessageHandler->setType(mdtUiMessageHandler::Warning);
    doIt = pvMessageHandler->askUser();
  }else{
    doIt = true;
  }
  // Do revert
  if(doIt){
    if(!pvProxyModel->removeRow(pvCurrentRow)){
      pvLastError.setError(tr("Cannot remove row in table:") + " '" + userFriendlyTableName() + "'", mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
      pvLastError.commit();
      if(pvMessageHandler){
        pvMessageHandler->setError(pvLastError);
        pvMessageHandler->displayToUser();
      }
      emit errorOccured();
      return;
    }
    pvCurrentRow = pvBeforeInsertCurrentRow;
    updateChildControllersAfterRevert();
    setCurrentRowPv(pvCurrentRow);
    updateChildControllersAfterCurrentRowChanged();
    emit operationSucceed();
  }else{
    emit errorOccured();  // To exit reverting state
  }
}

void mdtAbstractSqlTableController::onStateRemovingEntered()
{
  Q_ASSERT(pvModel);

  bool doIt;
  int row;

  // If no row exists, we do nothing
  if(rowCount() < 1){
    emit operationSucceed();
    return;
  }
  // We ask confirmation to the user
  if(pvMessageHandler){
    pvMessageHandler->setText(tr("You are about to delete some data."));
    pvMessageHandler->setInformativeText(tr("Do you want to continue ?"));
    pvMessageHandler->setType(mdtUiMessageHandler::Warning);
    doIt = pvMessageHandler->askUser();
  }else{
    doIt = true;
  }
  // Remove - Note: we ignore return value, because we return allways to Visualizing state
  if(doIt){
    row = pvCurrentRow;
    if(!updateChildControllersBeforeRemoveRow()){
      return;
    }
    if(!doRemove()){
      return;
    }
    /*
    * Go back to row.
    * Calling submitAll() will repopulate the model.
    * Because of this, we must be shure to fetch all data until we find our row
    */
    row = qMin(row, rowCount(true) - 1);
    setCurrentRowPv(row);
    updateChildControllersAfterCurrentRowChanged();
  }
  emit operationSucceed();
}

int mdtAbstractSqlTableController::firstMatchingRow(int column, const QVariant& matchData)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(!matchData.isNull());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < proxyModel()->columnCount());

  int row;
  QModelIndex index;

  if(pvModel->rowCount() < 1){
    return -1;
  }
  // Search row
  row = 0;
  while(true){
    index = proxyModel()->index(row, column);
    if(proxyModel()->data(index) == matchData){
      return row;
    }
    if(row == (pvModel->rowCount()-1)){
      if(!pvModel->canFetchMore()){
        break;
      }
      pvModel->fetchMore();
    }
    ++row;
  }
  // matchData not found
  QString msg;
  msg = tr("Could not find value") + " '" + matchData.toString() + "' ";
  msg += tr("for field") + " '" + pvModel->record().fieldName(column) + "' ";
  msg += tr("in table") + " '" + pvModel->tableName() + "'";
  pvLastError.setError(msg, mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
  pvLastError.commit();

  return -1;
}

int mdtAbstractSqlTableController::firstMatchingRow(int column1, const QVariant& key1, int column2, const QVariant& key2)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(!key1.isNull());
  Q_ASSERT(!key2.isNull());
  Q_ASSERT(column1 >= 0);
  Q_ASSERT(column1 < proxyModel()->columnCount());
  Q_ASSERT(column2 >= 0);
  Q_ASSERT(column2 < proxyModel()->columnCount());

  int row;
  QModelIndex index;

  if(pvModel->rowCount() < 1){
    return -1;
  }
  // Search row
  row = 0;
  while(true){
    index = proxyModel()->index(row, column1);
    if(proxyModel()->data(index) == key1){
      index = proxyModel()->index(row, column2);
      if(proxyModel()->data(index) == key2){
        return row;
      }
    }
    if(row == (pvModel->rowCount()-1)){
      if(!pvModel->canFetchMore()){
        break;
      }
      pvModel->fetchMore();
    }
    ++row;
  }
  // No matching row found
  QString msg;
  msg = tr("Could not find record with key") + " '" + key1.toString() + "' ";
  msg += tr("for field") + " '" + pvModel->record().fieldName(column1) + "' ";
  msg += tr("and key") + " '" + key2.toString() + "' ";
  msg += tr("for field") + " '" + pvModel->record().fieldName(column2) + "' ";
  msg += tr("in table") + " '" + pvModel->tableName() + "'";
  pvLastError.setError(msg, mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtAbstractSqlTableController");
  pvLastError.commit();

  return -1;
}

bool mdtAbstractSqlTableController::setupAndAddChildController(shared_ptr< mdtAbstractSqlTableController > controller, const mdtSqlRelationInfo& relationInfo, QSqlDatabase db, const QString& userFriendlyChildTableName)
{
  Q_ASSERT(pvModel);
  Q_ASSERT(controller);
  Q_ASSERT(!relationInfo.childTableName().isEmpty());
  Q_ASSERT(!relationInfo.items().isEmpty());

  mdtAbstractSqlTableControllerContainer container;
  shared_ptr<mdtSqlRelation> relation(new mdtSqlRelation);
  mdtSqlRelationInfoItem item;
  int i;

  // Setup child controller
  controller->setTableName(relationInfo.childTableName(), db, userFriendlyChildTableName);
  Q_ASSERT(controller->pvModel);
  // Setup relation
  relation->setParentModel(pvModel.get());
  relation->setChildModel(controller->pvModel.get());
  for(i = 0; i < relationInfo.items().size(); ++i){
    item = relationInfo.items().at(i);
    if(!relation->addRelation(item.parentFieldName, item.childFieldName, item.copyParentToChildOnInsertion, item.relationOperatorWithPreviousItem)){
      pvLastError = relation->lastError();
      return false;
    }
  }
  if(relationInfo.relationType() == mdtSqlRelationInfo::OneToOne){
    connect(controller.get(), SIGNAL(dataEdited()), this, SIGNAL(dataEdited()));
  }
  // Add to container
  container.controller = controller;
  container.relation = relation;
  container.relationType = relationInfo.relationType();
  pvChildControllerContainers.append(container);
  // Start controller if we are running
  if(pvStateMachine.isRunning()){
    controller->start();
    controller->model()->setFilter("-1");
    if(!controller->select()){
      pvLastError = controller->lastError();
      return false;
    }
  }

  return true;
}

void mdtAbstractSqlTableController::updateChildControllersAfterCurrentRowChanged()
{
  int i;
  QModelIndex proxyIndex, sourceIndex;

  proxyIndex = pvProxyModel->index(pvCurrentRow, 0);
  sourceIndex = pvProxyModel->mapToSource(proxyIndex);
  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    Q_ASSERT(pvChildControllerContainers.at(i).relation);
    Q_ASSERT(pvChildControllerContainers.at(i).controller);
    pvChildControllerContainers.at(i).relation->setParentCurrentRow(sourceIndex.row());
    ///pvChildControllerContainers.at(i).controller->setCurrentRowPv(0);
    pvChildControllerContainers.at(i).controller->setCurrentRow(0);
    pvChildControllerContainers.at(i).controller->sort();
  }
}

void mdtAbstractSqlTableController::updateChildControllersAfterInsert()
{
  int i;
  QModelIndex proxyIndex, sourceIndex;

  proxyIndex = pvProxyModel->index(pvCurrentRow, 0);
  sourceIndex = pvProxyModel->mapToSource(proxyIndex);
  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    Q_ASSERT(pvChildControllerContainers.at(i).relation);
    pvChildControllerContainers.at(i).relation->setParentCurrentRow(sourceIndex.row());
    if(pvChildControllerContainers.at(i).relationType == mdtSqlRelationInfo::OneToOne){
      Q_ASSERT(pvChildControllerContainers.at(i).controller);
      pvChildControllerContainers.at(i).controller->insert();
    }
  }
}

void mdtAbstractSqlTableController::updateChildControllersAfterRevert()
{
  int i;

  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    if(pvChildControllerContainers.at(i).relationType == mdtSqlRelationInfo::OneToOne){
      Q_ASSERT(pvChildControllerContainers.at(i).controller);
      pvChildControllerContainers.at(i).controller->revert();
    }
  }
}

bool mdtAbstractSqlTableController::updateChildControllersAfterSubmit()
{
  int i;

  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    if(pvChildControllerContainers.at(i).relationType == mdtSqlRelationInfo::OneToOne){
      Q_ASSERT(pvChildControllerContainers.at(i).controller);
      if(!pvChildControllerContainers.at(i).controller->submitAndWait()){
        return false;
      }
    }
  }

  return true;
}

bool mdtAbstractSqlTableController::updateChildControllersAfterSubmitNewRow()
{
  int i;
  QModelIndex proxyIndex, sourceIndex;

  proxyIndex = pvProxyModel->index(pvCurrentRow, 0);
  sourceIndex = pvProxyModel->mapToSource(proxyIndex);
  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    if(pvChildControllerContainers.at(i).relationType == mdtSqlRelationInfo::OneToOne){
      Q_ASSERT(pvChildControllerContainers.at(i).controller);
      if(!pvChildControllerContainers.at(i).controller->submitAndWait()){
        return false;
      }
      // Here, child controller has updated his FKs, we must update relation filter now
      pvChildControllerContainers.at(i).relation->setParentCurrentRow(sourceIndex.row());
    }
  }

  return true;
}

bool mdtAbstractSqlTableController::updateChildControllersBeforeRemoveRow()
{
  int i;

  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    if(pvChildControllerContainers.at(i).relationType == mdtSqlRelationInfo::OneToOne){
      Q_ASSERT(pvChildControllerContainers.at(i).controller);
      if(!pvChildControllerContainers.at(i).controller->removeAndWait()){
        return false;
      }
    }
  }

  return true;
}

bool mdtAbstractSqlTableController::checkBeforeSubmit()
{
  int i;

  for(i = 0; i < pvDataValidators.size(); ++i){
    Q_ASSERT(pvDataValidators.at(i));
    if(!pvDataValidators.at(i)->checkBeforeSubmit()){
      return false;
    }
  }
  for(i = 0; i < pvChildControllerContainers.size(); ++i){
    Q_ASSERT(pvChildControllerContainers.at(i).controller);
    if(!pvChildControllerContainers.at(i).controller->checkBeforeSubmit()){
      return false;
    }
  }

  return true;
}

void mdtAbstractSqlTableController::waitOperationComplete()
{
  while(!pvOperationComplete){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
}

void mdtAbstractSqlTableController::buildStateMachine()
{
  pvStateSelecting = new mdtState(Selecting);
  pvStateVisualizing = new mdtState(Visualizing);
  pvStateReverting = new mdtState(Reverting);
  pvStateEditing = new mdtState(Editing);
  pvStateSubmitting = new mdtState(Submitting);
  pvStateInserting = new mdtState(Inserting);
  pvStateEditingNewRow = new mdtState(EditingNewRow);
  pvStateRevertingNewRow = new mdtState(RevertingNewRow);
  pvStateSubmittingNewRow = new mdtState(SubmittingNewRow);
  pvStateRemoving = new mdtState(Removing);
  // Setup transitions
  connect(pvStateSelecting, SIGNAL(entered()), this, SLOT(onStateSelectingEntered()));
  pvStateSelecting->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  connect(pvStateVisualizing, SIGNAL(entered()), this, SLOT(onStateVisualizingEntered()));
  connect(pvStateVisualizing, SIGNAL(exited()), this, SLOT(onStateVisualizingExited()));
  pvStateVisualizing->addTransition(this, SIGNAL(selectTriggered()), pvStateSelecting);
  pvStateVisualizing->addTransition(this, SIGNAL(dataEdited()), pvStateEditing);
  pvStateVisualizing->addTransition(this, SIGNAL(submitTriggered()), pvStateSubmitting);
  pvStateVisualizing->addTransition(this, SIGNAL(insertTriggered()), pvStateInserting);
  pvStateVisualizing->addTransition(this, SIGNAL(removeTriggered()), pvStateRemoving);
  connect(pvStateReverting, SIGNAL(entered()), this, SLOT(onStateRevertingEntered()));
  pvStateReverting->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateReverting->addTransition(this, SIGNAL(errorOccured()), pvStateEditing);
  connect(pvStateEditing, SIGNAL(entered()), this, SLOT(onStateEditingEntered()));
  connect(pvStateEditing, SIGNAL(exited()), this, SLOT(onStateEditingExited()));
  pvStateEditing->addTransition(this, SIGNAL(revertTriggered()), pvStateReverting);
  pvStateEditing->addTransition(this, SIGNAL(submitTriggered()), pvStateSubmitting);
  connect(pvStateSubmitting, SIGNAL(entered()), this, SLOT(onStateSubmittingEntered()));
  pvStateSubmitting->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateSubmitting->addTransition(this, SIGNAL(errorOccured()), pvStateEditing);
  connect(pvStateInserting, SIGNAL(entered()), this, SLOT(onStateInsertingEntered()));
  pvStateInserting->addTransition(this, SIGNAL(operationSucceed()), pvStateEditingNewRow);
  pvStateInserting->addTransition(this, SIGNAL(errorOccured()), pvStateVisualizing);
  connect(pvStateEditingNewRow, SIGNAL(entered()), this, SLOT(onStateEditingNewRowEntered()));
  connect(pvStateEditingNewRow, SIGNAL(exited()), this, SLOT(onStateEditingNewRowExited()));
  pvStateEditingNewRow->addTransition(this, SIGNAL(revertTriggered()), pvStateRevertingNewRow);
  pvStateEditingNewRow->addTransition(this, SIGNAL(submitTriggered()), pvStateSubmittingNewRow);
  connect(pvStateRevertingNewRow, SIGNAL(entered()), this, SLOT(onStateRevertingNewRowEntered()));
  pvStateRevertingNewRow->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateRevertingNewRow->addTransition(this, SIGNAL(errorOccured()), pvStateEditingNewRow);
  connect(pvStateSubmittingNewRow, SIGNAL(entered()), this, SLOT(onStateSubmittingNewRowEntered()));
  pvStateSubmittingNewRow->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  pvStateSubmittingNewRow->addTransition(this, SIGNAL(errorOccured()), pvStateEditingNewRow);
  connect(pvStateRemoving, SIGNAL(entered()), this, SLOT(onStateRemovingEntered()));
  pvStateRemoving->addTransition(this, SIGNAL(operationSucceed()), pvStateVisualizing);
  // Add states to state machine and start
  pvStateMachine.addState(pvStateSelecting);
  pvStateMachine.addState(pvStateVisualizing);
  pvStateMachine.addState(pvStateReverting);
  pvStateMachine.addState(pvStateEditing);
  pvStateMachine.addState(pvStateSubmitting);
  pvStateMachine.addState(pvStateInserting);
  pvStateMachine.addState(pvStateEditingNewRow);
  pvStateMachine.addState(pvStateRevertingNewRow);
  pvStateMachine.addState(pvStateSubmittingNewRow);
  pvStateMachine.addState(pvStateRemoving);
  pvStateMachine.setInitialState(pvStateVisualizing);
}
