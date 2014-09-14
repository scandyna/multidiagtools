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
#include "mdtSqlTableViewController.h"
#include "mdtSortFilterProxyModel.h"
#include <QSortFilterProxyModel>
#include <QAbstractItemView>
#include <QTableView>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QSqlError>
#include <QCoreApplication>

#include <QDebug>

/*
 * mdtSqlTableViewControllerItemDelegate implementation
 */

mdtSqlTableViewControllerItemDelegate::mdtSqlTableViewControllerItemDelegate(QObject* parent)
 : QStyledItemDelegate(parent)
{
}

QWidget* mdtSqlTableViewControllerItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  emit dataEditionBegins();
  return QStyledItemDelegate::createEditor(parent, option, index);
}

void mdtSqlTableViewControllerItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  qDebug() << "setModelData() ...";
  QStyledItemDelegate::setModelData(editor, model, index);
  emit dataEditionDone();
}


/*
 * mdtSqlTableViewController implementation
 */

mdtSqlTableViewController::mdtSqlTableViewController(QObject* parent)
 : mdtAbstractSqlTableController(parent)
{
  pvTableView = 0;
  pvDefaultColumnToSelect = 0;
  pvEditionDone = true;
}

mdtSqlTableViewController::~mdtSqlTableViewController()
{
}

void mdtSqlTableViewController::setTableView(QTableView* tv, QAbstractItemDelegate *delegate)
{
  Q_ASSERT(tv != 0);

  pvTableView = tv;
  connect(pvTableView, SIGNAL(destroyed(QObject*)), this, SLOT(onTableViewDestroyed(QObject*)));
  pvTableView->setModel(proxyModel().get());
  if(delegate == 0){
    delegate = new mdtSqlTableViewControllerItemDelegate(pvTableView);
  }
  connect(delegate, SIGNAL(dataEditionBegins()), this, SIGNAL(dataEdited()));
  connect(delegate, SIGNAL(dataEditionDone()), this, SLOT(onEditionDone()));
  pvTableView->setItemDelegate(delegate);
}

int mdtSqlTableViewController::currentRow() const
{
  QModelIndex index;

  if(pvTableView == 0){
    return -1;
  }
  if(!model()){
    return -1;
  }
  index = pvTableView->currentIndex();
  if(index.isValid()){
    return index.row();
  }else{
    return -1;
  }
}

void mdtSqlTableViewController::onTableViewDestroyed(QObject* obj)
{
  pvTableView = 0;
}

void mdtSqlTableViewController::onStateVisualizingEntered()
{
  mdtAbstractSqlTableController::onStateVisualizingEntered();
  pvEditionDone = true;
}

void mdtSqlTableViewController::onStateEditingEntered()
{
  pvEditionDone = false;
  mdtAbstractSqlTableController::onStateEditingEntered();
}

void mdtSqlTableViewController::onStateEditingNewRowEntered()
{
  pvEditionDone = false;
  mdtAbstractSqlTableController::onStateEditingNewRowEntered();
}

void mdtSqlTableViewController::onEditionDone()
{
  pvEditionDone = true;
}

void mdtSqlTableViewController::waitEditionDone()
{
  while(!pvEditionDone){
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
}

void mdtSqlTableViewController::currentRowChangedEvent(int row)
{

}

void mdtSqlTableViewController::modelSetEvent()
{
  Q_ASSERT(model());
  ///Q_ASSERT(pvTableView != 0);

  model()->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ///pvTableView->setModel(model().get());
}

bool mdtSqlTableViewController::doSubmit()
{
  Q_ASSERT(model());
  Q_ASSERT(pvTableView != 0);
  Q_ASSERT(pvTableView->itemDelegate() != 0);

  QSqlError sqlError;
  QModelIndex index;
  int row;

  // Remember current index (will be lost during submit)
  row = currentRow();

  waitEditionDone();
  // Submit to database
  if(!model()->submitAll()){
    sqlError = model()->lastError();
    pvLastError.setError(tr("Submitting data to database failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlTableViewController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }
  /*
   * Go back to row.
   * Calling submitAll() will repopulate the model.
   * Because of this, we must be shure to fetch all data until we find our row
   */
  if(model()->rowCount() > 0){
    while((row >= model()->rowCount())&&(model()->canFetchMore())){
      model()->fetchMore();
    }
  }
  index = proxyModel()->index(row, pvDefaultColumnToSelect);
  pvTableView->setCurrentIndex(index);
  
  qDebug() << "Submit done - go to index " << index;

  return true;
}

bool mdtSqlTableViewController::doRevert()
{
  Q_ASSERT(model());

  model()->revertAll();

  return true;
}

bool mdtSqlTableViewController::doInsert()
{
  Q_ASSERT(model());
  Q_ASSERT(currentState() == Inserting);

  int row;
  QModelIndex index;

  // Insert new row at end
  row = rowCount(true);
  proxyModel()->insertRow(row);
  index = proxyModel()->index(row, pvDefaultColumnToSelect);
  pvTableView->setCurrentIndex(index);

  return true;
}

bool mdtSqlTableViewController::doSubmitNewRow()
{
  return doSubmit();
}

bool mdtSqlTableViewController::doRevertNewRow()
{
  Q_ASSERT(model());
  Q_ASSERT(pvTableView != 0);
  Q_ASSERT(pvTableView->model() != 0);

  int row;
  QSqlError sqlError;

  // We simply remove the last row
  row = pvTableView->model()->rowCount()-1;
  if(row >= 0){
    if(!pvTableView->model()->removeRow(row)){
      sqlError = model()->lastError();
      pvLastError.setError(tr("Reverting new data failed."), mdtError::Error);
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(pvLastError, "mdtSqlTableViewController");
      pvLastError.commit();
      if(messageHandler()){
        messageHandler()->setError(pvLastError);
        messageHandler()->displayToUser();
      }
      return false;
    }
  }

  return true;
}

bool mdtSqlTableViewController::doRemove()
{
  Q_ASSERT(model());
  Q_ASSERT(pvTableView != 0);
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndexList indexes;
  int i;
  int row;
  QVector<int> rows;
  QSqlError sqlError;

  // Build rows list
  indexes = pvTableView->selectionModel()->selectedIndexes();
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    if(!rows.contains(row)){
      rows.append(row);
    }
  }
  // Delete selected rows
  for(i = 0; i < rows.size(); ++i){
    if(!proxyModel()->removeRow(rows.at(i))){
      QSqlError sqlError;
      sqlError = model()->lastError();
      if(sqlError.isValid()){
        pvLastError.setError(tr("Removing data failed."), mdtError::Error);
        pvLastError.setSystemError(sqlError.number(), sqlError.text());
        MDT_ERROR_SET_SRC(pvLastError, "mdtSqlTableViewController");
        pvLastError.commit();
        if(messageHandler()){
          messageHandler()->setError(pvLastError);
          messageHandler()->displayToUser();
        }
      }
      model()->revertAll();
      return false;
    }
  }
  if(!model()->submitAll()){
    model()->revert();
    sqlError = model()->lastError();
    pvLastError.setError(tr("Reverting new data failed."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlTableViewController");
    pvLastError.commit();
    if(messageHandler()){
      messageHandler()->setError(pvLastError);
      messageHandler()->displayToUser();
    }
    return false;
  }

  return true;
}
