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
#include "mdtSqlSelectionDialog.h"
#include "mdtSortFilterProxyModel.h"
#include "mdtError.h"
#include <QSqlQueryModel>
#include <QSqlField>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSqlRecord>
#include <QItemSelectionModel>
#include <QMessageBox>

//#include <QDebug>

mdtSqlSelectionDialog::mdtSqlSelectionDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  // Set defaults
  pvAllowEmptyResult = false;
  // Setup models
  pvModel = new QSqlQueryModel(this);
  pvProxyModel = new mdtSortFilterProxyModel(this);
  pvProxyModel->setSourceModel(pvModel);
  // Setup view
  pvTableView->setModel(pvProxyModel);
  pvTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

mdtSqlSelectionDialog::~mdtSqlSelectionDialog()
{
}

void mdtSqlSelectionDialog::setMessage(const QString &message)
{
  pvMessageLabel->setText(message);
}

void mdtSqlSelectionDialog::setAllowEmptyResult(bool allow)
{
  pvAllowEmptyResult = allow;
}

void mdtSqlSelectionDialog::setQuery(const QString& sql, QSqlDatabase db, bool allowMultiSelection)
{
  Q_ASSERT(pvModel != 0);

  QSqlError sqlError;

  pvModel->setQuery(sql, db);
  sqlError = pvModel->lastError();
  if(sqlError.isValid()){
    displaySqlError(sqlError);
    return;
  }
  pvTableView->resizeColumnsToContents();
  pvTableView->resizeRowsToContents();
  if(allowMultiSelection){
    pvTableView->setSelectionMode(QAbstractItemView::MultiSelection);
  }else{
    pvTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  }
}

void mdtSqlSelectionDialog::setHeaderData(const QString &fieldName, const QString &data)
{
  Q_ASSERT(pvModel != 0);

  int column;

  column = pvModel->record().indexOf(fieldName);
  if(column < 0){
    return;
  }
  pvModel->setHeaderData(column, Qt::Horizontal, data);
  pvTableView->resizeColumnToContents(column);
}

void mdtSqlSelectionDialog::setColumnHidden(int column, bool hide)
{
  pvTableView->setColumnHidden(column, hide);
}

void mdtSqlSelectionDialog::setColumnHidden(const QString &fieldName, bool hide)
{
  Q_ASSERT(pvModel != 0);

  int column;

  column = pvModel->record().indexOf(fieldName);
  if(column < 0){
    return;
  }
  setColumnHidden(column, hide);
}

void mdtSqlSelectionDialog::clearColumnsSortOrder()
{
  Q_ASSERT(pvProxyModel != 0);

  pvProxyModel->clearColumnsSortOrder();
}

void mdtSqlSelectionDialog::addColumnToSortOrder(int column, Qt::SortOrder order)
{
  Q_ASSERT(pvProxyModel != 0);

  pvProxyModel->addColumnToSortOrder(column, order);
}

void mdtSqlSelectionDialog::addColumnToSortOrder(const QString & fieldName, Qt::SortOrder order)
{
  Q_ASSERT(pvProxyModel != 0);

  pvProxyModel->addColumnToSortOrder(fieldName, order);
}

void mdtSqlSelectionDialog::sort()
{
  // If model is empty, canFetchMore will allways return true, prevent this
  if(pvModel->rowCount() < 1){
    return;
  }
  while(pvModel->canFetchMore()){
    pvModel->fetchMore();
  }
  pvTableView->sortByColumn(0);
}

void mdtSqlSelectionDialog::selectRows(const QString &fieldName, const QVariant &matchData)
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvProxyModel != 0);

  int row, column;
  QModelIndex index;

  column = pvModel->record().indexOf(fieldName);
  if(column < 0){
    return;
  }
  for(row = 0; row < pvProxyModel->rowCount(); ++row){
    index = pvProxyModel->index(row, column);
    if(pvProxyModel->data(index) == matchData){
      pvTableView->selectRow(row);
    }
  }
}

void mdtSqlSelectionDialog::setSelectionResultColumns(const QList<int> &columns)
{
  pvSelectionResultColumns = columns;
}

void mdtSqlSelectionDialog::setSelectionResultColumns(const QStringList &fields)
{
  Q_ASSERT(pvModel != 0);
  int i;

  pvSelectionResultColumns.clear();
  for(i = 0; i < fields.size(); ++i){
    addSelectionResultColumn(fields.at(i));
  }
}

void mdtSqlSelectionDialog::addSelectionResultColumn(const QString &field)
{
  Q_ASSERT(pvModel != 0);
  int column;

  column = pvModel->record().indexOf(field);
  if(column < 0){
    mdtError e(tr("Field") + " '" + field + "' " + tr("not found"), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSqlSelectionDialog");
    e.commit();
    return;
  }
  pvSelectionResultColumns.append(column);
}

QSqlRecord mdtSqlSelectionDialog::selectedDataRecord()
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QSqlRecord record;
  QSqlRecord fieldInfo;
  int row, i;
  QModelIndex index;

  if(pvSelectionResults.isEmpty()){
    return record;
  }
  // We take only first row of selection results
  index = pvSelectionResults.at(0);
  if(!index.isValid()){
    return record;
  }
  row = index.row();
  fieldInfo = pvModel->record();
  for(i = 0; i < pvSelectionResultColumns.size(); ++i){
    index = pvModel->index(row, pvSelectionResultColumns.at(i));
    record.append(fieldInfo.field(index.column()));
    record.setValue(i, pvModel->data(index));
    ///result.append(pvModel->data(index));
  }

  return record;
}

QList<QVariant> mdtSqlSelectionDialog::selectionResult()
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QList<QVariant> result;
  int row, i;
  QModelIndex index;

  if(pvSelectionResults.isEmpty()){
    return result;
  }
  // We take only first row of selection results
  index = pvSelectionResults.at(0);
  if(!index.isValid()){
    return result;
  }
  row = index.row();
  for(i = 0; i < pvSelectionResultColumns.size(); ++i){
    index = pvModel->index(row, pvSelectionResultColumns.at(i));
    result.append(pvModel->data(index));
  }

  return result;
}

QModelIndexList mdtSqlSelectionDialog::selectionResults()
{
  return pvSelectionResults;
}

QVariant mdtSqlSelectionDialog::selectedData(int row, const QString &fieldName)
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvTableView->selectionModel() != 0);

  int i;
  int column;
  QModelIndex index;
  QModelIndexList rowIndexes;
  QVariant data;

  // Get indexes that maches given row
  for(i = 0; i < pvSelectionResults.size(); ++i){
    index = pvSelectionResults.at(i);
    if(index.row() == row){
      rowIndexes.append(index);
    }
  }
  if(rowIndexes.isEmpty()){
    return data;
  }
  // Get column for given fieldName
  column = pvModel->record().indexOf(fieldName);
  if(column < 0){
    return data;
  }
  // Get data
  for(i = 0; i < rowIndexes.size(); ++i){
    index = rowIndexes.at(i);
    if(index.column() == column){
      index = pvModel->index(index.row(), index.column());
      data = pvModel->data(index);
    }
  }

  return data;
}

void mdtSqlSelectionDialog::reject()
{
  QDialog::reject();
}

void mdtSqlSelectionDialog::accept()
{
  buildSelectionResults();
  if((pvSelectionResults.isEmpty())&&(!pvAllowEmptyResult)){
    return;
  }
  QDialog::accept();
}

void mdtSqlSelectionDialog::buildSelectionResults()
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvProxyModel != 0);
  Q_ASSERT(pvTableView->selectionModel() != 0);

  int i;
  QModelIndex index;
  QModelIndexList selectedIndexes;

  pvSelectionResults.clear();
  selectedIndexes = pvTableView->selectionModel()->selectedIndexes();
  for(i = 0; i < selectedIndexes.size(); ++i){
    index = pvProxyModel->index(selectedIndexes.at(i).row(), selectedIndexes.at(i).column());
    index = pvProxyModel->mapToSource(index);
    if((index.isValid()) && (pvSelectionResultColumns.contains(index.column()))){
      pvSelectionResults.append(index);
    }
  }
}

void mdtSqlSelectionDialog::displaySqlError(const QSqlError & sqlError)
{
  QMessageBox msgBox;

  // Log error
  mdtError e(tr("Unable to get data for given SQL query."), mdtError::Error);
  e.setSystemError(sqlError.number(), sqlError.text());
  MDT_ERROR_SET_SRC(e, "mdtClArticle");
  e.commit();
  // Display error
  msgBox.setText(e.text());
  msgBox.setDetailedText(e.systemText());
  msgBox.setIcon(e.levelIcon());
  msgBox.exec();
}
