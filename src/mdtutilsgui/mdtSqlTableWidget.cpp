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
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QPushButton>

#include <QDebug>

mdtSqlTableWidget::mdtSqlTableWidget(QWidget *parent)
 : mdtAbstractSqlWidget(parent)
{
  pvTableView = new QTableView;
  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget(pvTableView);
  setLayout(layout);

  pvNavigationLayout = 0;
  pbNavToFirst = 0;
  pbNavToPrevious = 0;
  pbNavToNext = 0;
  pbNavToLast = 0;
  pbInsert = 0;
  pbSubmit = 0;
  pbRevert = 0;
  pbRemove = 0;
}

mdtSqlTableWidget::~mdtSqlTableWidget()
{
}

int mdtSqlTableWidget::currentRow() const
{
  return pvTableView->currentIndex().row();
}

void mdtSqlTableWidget::enableLocalNavigation()
{
  Q_ASSERT(layout() != 0);

  QBoxLayout *boxLayout;

  // Create actions
  pbNavToFirst = new QPushButton("|<<");
  pbNavToPrevious = new QPushButton("<");
  pbNavToNext = new QPushButton(">");
  pbNavToLast = new QPushButton(">>|");
  // Connect actions enable/disable
  connect(this, SIGNAL(toFirstEnabledStateChanged(bool)), pbNavToFirst, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(toPreviousEnabledStateChanged(bool)), pbNavToPrevious, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(toNextEnabledStateChanged(bool)), pbNavToNext, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(toLastEnabledStateChanged(bool)), pbNavToLast, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(pbNavToFirst, SIGNAL(clicked()), this, SLOT(toFirst()));
  connect(pbNavToPrevious, SIGNAL(clicked()), this, SLOT(toPrevious()));
  connect(pbNavToNext, SIGNAL(clicked()), this, SLOT(toNext()));
  connect(pbNavToLast, SIGNAL(clicked()), this, SLOT(toLast()));
  // Add to layout
  if(pvNavigationLayout == 0){
    pvNavigationLayout = new QHBoxLayout;
    pvNavigationLayout->addStretch();
    boxLayout = dynamic_cast<QBoxLayout*>(layout());
    Q_ASSERT(boxLayout != 0);
    boxLayout->addLayout(pvNavigationLayout);
  }
  Q_ASSERT(pvNavigationLayout != 0);
  pvNavigationLayout->addWidget(pbNavToFirst);
  pvNavigationLayout->addWidget(pbNavToPrevious);
  pvNavigationLayout->addWidget(pbNavToNext);
  pvNavigationLayout->addWidget(pbNavToLast);
}

void mdtSqlTableWidget::enableLocalEdition()
{
  Q_ASSERT(layout() != 0);

  QBoxLayout *boxLayout;

  // Create actions
  pbInsert = new QPushButton("New");
  pbSubmit = new QPushButton("Save");
  pbRevert = new QPushButton("Cancel");
  pbRemove = new QPushButton("Delete");
  // As default, functions are disabled
  pbInsert->setEnabled(false);
  pbSubmit->setEnabled(false);
  pbRevert->setEnabled(false);
  pbRemove->setEnabled(false);
  // Connect actions enable/disable
  connect(this, SIGNAL(insertEnabledStateChanged(bool)), pbInsert, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(removeEnabledStateChanged(bool)), pbRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(pbInsert, SIGNAL(clicked()), this, SLOT(insert()));
  connect(pbSubmit, SIGNAL(clicked()), this, SLOT(submit()));
  connect(pbRevert, SIGNAL(clicked()), this, SLOT(revert()));
  connect(pbRemove, SIGNAL(clicked()), this, SLOT(remove()));
  // Add to layout
  if(pvNavigationLayout == 0){
    pvNavigationLayout = new QHBoxLayout;
    pvNavigationLayout->addStretch();
    boxLayout = dynamic_cast<QBoxLayout*>(layout());
    Q_ASSERT(boxLayout != 0);
    boxLayout->addLayout(pvNavigationLayout);
  }
  Q_ASSERT(pvNavigationLayout != 0);
  pvNavigationLayout->addWidget(pbInsert);
  pvNavigationLayout->addWidget(pbSubmit);
  pvNavigationLayout->addWidget(pbRevert);
  pvNavigationLayout->addWidget(pbRemove);
}

void mdtSqlTableWidget::onDataChanged(const QModelIndex &, const QModelIndex &)
{
  emit dataEdited();
}

void mdtSqlTableWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
  if(current.isValid()){
    emit currentRowChanged(current.row());
  }else{
    emit currentRowChanged(-1);
  }
}

void mdtSqlTableWidget::doSetModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTableView->setModel(model);
  connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
  // A selection model is created after setModel() by view itself
  Q_ASSERT(pvTableView->selectionModel() != 0);
  connect(pvTableView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onCurrentRowChanged(const QModelIndex&, const QModelIndex&)));
}

bool mdtSqlTableWidget::doSubmit()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  if(!model()->submitAll()){
    displayDatabaseError(model()->lastError());
    return false;
  }

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
  Q_ASSERT(model() != 0);

  int row;

  // We simply remove the last row
  row = model()->rowCount()-1;
  if(row >= 0){
    if(!pvTableView->model()->removeRow(row)){
      displayDatabaseError(model()->lastError());
      return false;
    }
  }

  return true;
}

bool mdtSqlTableWidget::doRemove()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  int i;
  int ret;
  QMessageBox msgBox;
  QModelIndexList indexes = pvTableView->selectionModel()->selectedIndexes();

  // If nothing was selected, we do nothing
  if(indexes.size() < 1){
    return true;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete selected rows."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return true;
  }
  // Delete selected rows
  for(i=0; i<indexes.size(); ++i){
    if(!model()->removeRow(indexes.at(i).row())){
      displayDatabaseError(model()->lastError());
      model()->revertAll();
      return false;
    }
  }
  if(!model()->submitAll()){
    displayDatabaseError(model()->lastError());
    return false;
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
