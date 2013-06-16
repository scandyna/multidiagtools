/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtSqlTableWidget.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QSqlError>
#include <QMessageBox>

#include <QDebug>

mdtSqlTableWidget::mdtSqlTableWidget(QWidget *parent)
 : mdtAbstractSqlWidget(parent)
{
  ///model() = 0;
  pvTableView = new QTableView;
  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget(pvTableView);
  setLayout(layout);
  
  
  ///connect(pvTableView, SIGNAL(entered(const QModelIndex&)), this, SLOT(test01(const QModelIndex&)));
  ///connect(pvTableView, SIGNAL(Hactivated(const QModelIndex&)), this, SLOT(test01(const QModelIndex&)));
  ///connect(pvTableView->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(test02(QWidget*)));
}

mdtSqlTableWidget::~mdtSqlTableWidget()
{
}

int mdtSqlTableWidget::currentRow() const
{
  return pvTableView->currentIndex().row();
}

void mdtSqlTableWidget::onDataChanged(const QModelIndex &, const QModelIndex &)
{
  emit dataEdited();
}

void mdtSqlTableWidget::doSetModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  ///model->setEditStrategy(QSqlTableModel::OnFieldChange);
  ///model->setEditStrategy(QSqlTableModel::OnRowChange);
  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTableView->setModel(model);
  connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
  ///model() = model;
}

bool mdtSqlTableWidget::doSubmit()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  ///Q_ASSERT(pvTableView->model() != 0);
  Q_ASSERT(model() != 0);

  int row;
  ///QSqlError error;

  // Remember current row (will be lost during submit)
  row = currentRow();

  // Do some check before real submit
  /**
  if(!checkBeforeSubmit()){
    QMessageBox msgBox;
    msgBox.setText(tr("There are some errors in edited data."));
    msgBox.setInformativeText( \
      tr("Fields that are not correct should be highlighted.\nMoving cursor over field with error should display the reason\nPlease correct errors, or cancel modifications, and try again."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();    return false;
  }
  */
  // 
  if(!model()->submitAll()){
    displayDatabaseError(model()->lastError());
    ///displayModelFatalError(pvWidgetMapper->model()->lastError());
    return false;
  }
  // Go back to current row
  ///pvWidgetMapper->setCurrentIndex(row);

  return true;
}

bool mdtSqlTableWidget::doRevert()
{
  Q_ASSERT(model() != 0);

  model()->revertAll();

  return true;
}

bool mdtSqlTableWidget::doInsert()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  int row;
  QModelIndex index;

  /**
  if(currentState() != Inserting){
    warnUserAboutUnsavedRow();
    return false;
  }
  */
  // Insert new row at end
  row = model()->rowCount();
  model()->insertRow(row);
  index = model()->index(row, 0);
  pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);

  return true;
}

bool mdtSqlTableWidget::doSubmitNewRow()
{
  return doSubmit();
}

bool mdtSqlTableWidget::doRevertNewRow()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  ///Q_ASSERT(pvTableView->model() != 0);
  Q_ASSERT(model() != 0);

  int row;
  QModelIndex index;

  // We simply remove the last row
  row = model()->rowCount()-1;
  if(row >= 0){
    if(!pvTableView->model()->removeRow(row)){
      qDebug() << "removeRow() failed: " << model()->lastError();
      return false;
    }
  }
  
  // Remeber current row and remove it
  ///row = currentRow();
  // Data was never submit to model, we simply go to last row
  ///clearWidgets();
  ///row = qMin(row, pvTableView->model()->rowCount()-1);
  ///index = pvTableView->model()->index(row, 0);
  ///pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);

  return true;
}

bool mdtSqlTableWidget::doRemove()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  int i;
  QModelIndexList indexes = pvTableView->selectionModel()->selectedIndexes();

  for(i=0; i<indexes.size(); ++i){
    if(!model()->removeRow(indexes.at(i).row())){
      qDebug() << "Remove row failed for index " << indexes.at(i);
      model()->revertAll();
      return false;
    }
  }
  if(!model()->submitAll()){
    qDebug() << "DEL: submitAll() failed, error: " << model()->lastError();
  }

  return true;
}

void mdtSqlTableWidget::toFirst()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;

  if(pvTableView->model() == 0){
    return;
  }
  index = pvTableView->model()->index(0, 0);
  pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
}

void mdtSqlTableWidget::toLast()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;
  int row;

  if(pvTableView->model() == 0){
    return;
  }
  index = pvTableView->selectionModel()->currentIndex();
  row = pvTableView->model()->rowCount()-1;
  if(index.row() < row){
    index = pvTableView->model()->index(row, 0);
    pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
  }
}

void mdtSqlTableWidget::toPrevious()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;
  int row;

  if(pvTableView->model() == 0){
    return;
  }
  // Get current index, check that we are not at the begining and move if Ok
  index = pvTableView->selectionModel()->currentIndex();
  if(index.row() > 0){
    row = index.row()-1;
    index = pvTableView->model()->index(row, 0);
    pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
  }
}

void mdtSqlTableWidget::toNext()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;
  int row;

  if(pvTableView->model() == 0){
    return;
  }
  // Get current index, check that we are not at the end and move if Ok
  index = pvTableView->selectionModel()->currentIndex();
  if(index.row() < (pvTableView->model()->rowCount())-1){
    row = index.row()+1;
    index = pvTableView->model()->index(row, 0);
    pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
  }
}
