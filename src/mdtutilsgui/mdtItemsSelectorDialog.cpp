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
#include "mdtItemsSelectorDialog.h"
#include <QStringListModel>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QModelIndex>

#include <QDebug>

mdtItemsSelectorDialog::mdtItemsSelectorDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  // Setup items views
  pvAvailableItemsModel = new QStringListModel(this);
  lwAvailableItems->setSelectionMode(QAbstractItemView::MultiSelection);
  lwAvailableItems->setEditTriggers(QAbstractItemView::NoEditTriggers);
  lwAvailableItems->setModel(pvAvailableItemsModel);
  pvSelectedItemsModel = new QStringListModel(this);
  lwSelectedItems->setSelectionMode(QAbstractItemView::MultiSelection);
  lwSelectedItems->setEditTriggers(QAbstractItemView::NoEditTriggers);
  lwSelectedItems->setModel(pvSelectedItemsModel);
  // Setup sort combobox
  cbSortOrder->addItem(tr("Ascending"), Qt::AscendingOrder);
  cbSortOrder->addItem(tr("Descending"), Qt::DescendingOrder);

  connect(pbLeft, SIGNAL(clicked()), this, SLOT(transferSelectedItemsToSelectedItems()));
  connect(pbRight, SIGNAL(clicked()), this, SLOT(transferSelectedItemsToAvailableItems()));
  connect(pbSelectAllAvailableItems, SIGNAL(clicked()), this, SLOT(selectAllAvailableItems()));
  connect(pbUnselectAllAvailableItems, SIGNAL(clicked()), this, SLOT(unselectAllAvailableItems()));
  connect(pbSelectAllSelectedItems, SIGNAL(clicked()), this, SLOT(selectAllSelectedItems()));
  connect(pbUnselectAllSelectedItems, SIGNAL(clicked()), this, SLOT(unselectAllSelectedItems()));
  connect(pbUp, SIGNAL(clicked()), this, SLOT(moveSelectedItemsInSelectedItemsListUp()));
  connect(pbDown, SIGNAL(clicked()), this, SLOT(moveSelectedItemsInSelectedItemsListDown()));
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);
  ///connect(lwSelectedItems->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),\
            this, SLOT(updateCbSortOrderBySelectedItem(const QModelIndex&, const QModelIndex&)));
}

mdtItemsSelectorDialog::~mdtItemsSelectorDialog()
{
}

void mdtItemsSelectorDialog::setSortOptionEnabled(bool enabled)
{
  if(enabled){
    lbSortOrder->show();
    cbSortOrder->show();
  }else{
    lbSortOrder->hide();
    cbSortOrder->hide();
  }
}

void mdtItemsSelectorDialog::setAvailableItemsLabelText(const QString &text)
{
  lbAvailableItems->setText(text);
}

void mdtItemsSelectorDialog::setAvailableItems(const QStringList &items)
{
  pvAvailableItemsModel->setStringList(items);
  pvAvailableItemsModel->sort(0);
}

void mdtItemsSelectorDialog::setSelectedItemsLabelText(const QString &text)
{
  lbSelectedItems->setText(text);
}

void mdtItemsSelectorDialog::setSelectedItems(const QStringList &items)
{
  pvSelectedItemsModel->setStringList(items);
}

QStringList mdtItemsSelectorDialog::selectedItems() const
{
  return pvSelectedItemsModel->stringList();
}

void mdtItemsSelectorDialog::transferSelectedItemsToSelectedItems()
{
  Q_ASSERT(lwAvailableItems->selectionModel() != 0);

  QModelIndexList indexes = lwAvailableItems->selectionModel()->selectedIndexes();
  QModelIndex index;
  QStringList availableItemsList;
  QStringList selectedItemsList;
  int i;
  int itemIndex;

  // Transfer items
  foreach(index, indexes){
    if(!addItemToSelectedItemsModel(pvAvailableItemsModel->data(index, Qt::DisplayRole))){
      qDebug() << "Cannot add item at index " << index;
      return;
    }
  }
  // Remove transfered items
  availableItemsList = pvAvailableItemsModel->stringList();
  selectedItemsList = pvSelectedItemsModel->stringList();
  for(i=0; i<selectedItemsList.size(); ++i){
    itemIndex = availableItemsList.indexOf(selectedItemsList.at(i));
    if(itemIndex > -1){
      availableItemsList.removeAt(itemIndex);
    }
  }
  pvAvailableItemsModel->setStringList(availableItemsList);
  pvAvailableItemsModel->sort(0);
}

void mdtItemsSelectorDialog::transferSelectedItemsToAvailableItems()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndexList indexes = lwSelectedItems->selectionModel()->selectedIndexes();
  QModelIndex index;
  QStringList availableItemsList;
  QStringList selectedItemsList;
  int i;
  int itemIndex;

  // Transfer items
  foreach(index, indexes){
    if(!addItemToAvailableItemsModel(pvSelectedItemsModel->data(index, Qt::DisplayRole))){
      return;
    }
  }
  // Remove transfered items
  availableItemsList = pvAvailableItemsModel->stringList();
  selectedItemsList = pvSelectedItemsModel->stringList();
  for(i=0; i<availableItemsList.size(); ++i){
    itemIndex = selectedItemsList.indexOf(availableItemsList.at(i));
    if(itemIndex > -1){
      selectedItemsList.removeAt(itemIndex);
    }
  }
  pvSelectedItemsModel->setStringList(selectedItemsList);
  pvAvailableItemsModel->sort(0);
}

void mdtItemsSelectorDialog::selectAllAvailableItems()
{
  Q_ASSERT(lwAvailableItems->selectionModel() != 0);

  QModelIndex topIndex = pvAvailableItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvAvailableItemsModel->index(pvAvailableItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  qDebug() << "Selected " << selection.size() << " items in available list";
  
  lwAvailableItems->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void mdtItemsSelectorDialog::unselectAllAvailableItems()
{
  Q_ASSERT(lwAvailableItems->selectionModel() != 0);

  QModelIndex topIndex = pvAvailableItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvAvailableItemsModel->index(pvAvailableItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  lwAvailableItems->selectionModel()->select(selection, QItemSelectionModel::Deselect);
}

void mdtItemsSelectorDialog::selectAllSelectedItems()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex topIndex = pvSelectedItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvSelectedItemsModel->index(pvSelectedItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  lwSelectedItems->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void mdtItemsSelectorDialog::unselectAllSelectedItems()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex topIndex = pvSelectedItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvSelectedItemsModel->index(pvSelectedItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  lwSelectedItems->selectionModel()->select(selection, QItemSelectionModel::Deselect);
}

void mdtItemsSelectorDialog::moveSelectedItemsInSelectedItemsListUp()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex index = lwSelectedItems->selectionModel()->currentIndex();
  QVariant itemData;
  int row;

  if(!index.isValid()){
    return;
  }
  row = index.row();
  if(row < 1){
    return;
  }
  // Move item up
  itemData = pvSelectedItemsModel->data(index, Qt::DisplayRole);
  if(!pvSelectedItemsModel->removeRow(row)){
    return;
  }
  --row;
  if(!pvSelectedItemsModel->insertRow(row)){
    return;
  }
  index = pvSelectedItemsModel->index(row, 0);
  pvSelectedItemsModel->setData(index, itemData);
  // Update selection
  unselectAllSelectedItems();
  lwSelectedItems->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
}

void mdtItemsSelectorDialog::moveSelectedItemsInSelectedItemsListDown()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex index = lwSelectedItems->selectionModel()->currentIndex();
  QVariant itemData;
  int row;

  if(!index.isValid()){
    return;
  }
  row = index.row();
  if(row >= (pvSelectedItemsModel->rowCount()-1)){
    return;
  }
  // Move item down
  itemData = pvSelectedItemsModel->data(index, Qt::DisplayRole);
  if(!pvSelectedItemsModel->removeRow(row)){
    return;
  }
  ++row;
  if(!pvSelectedItemsModel->insertRow(row)){
    return;
  }
  index = pvSelectedItemsModel->index(row, 0);
  pvSelectedItemsModel->setData(index, itemData);
  // Update selection
  unselectAllSelectedItems();
  lwSelectedItems->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
}

/**
void mdtItemsSelectorDialog::updateCbSortOrderBySelectedItem(const QModelIndex &current, const QModelIndex &previous)
{
  if(!current.isValid()){
    cbSortOrder->setEnabled(false);
    return;
  }
}
*/

bool mdtItemsSelectorDialog::addItemToSelectedItemsModel(const QVariant &data)
{
  int row;

  row = pvSelectedItemsModel->rowCount();
  if(!pvSelectedItemsModel->insertRow(row)){
    return false;
  }
  if(!pvSelectedItemsModel->setData(pvSelectedItemsModel->index(row, 0), data)){
    pvSelectedItemsModel->revert();
    return false;
  }

  return true;
}

bool mdtItemsSelectorDialog::addItemToAvailableItemsModel(const QVariant &data)
{
  int row;

  row = pvAvailableItemsModel->rowCount();
  if(!pvAvailableItemsModel->insertRow(row)){
    return false;
  }
  if(!pvAvailableItemsModel->setData(pvAvailableItemsModel->index(row, 0), data)){
    pvAvailableItemsModel->revert();
    return false;
  }

  return true;
}
