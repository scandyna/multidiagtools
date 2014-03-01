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
#include "mdtMvSortingSetupDialog.h"
#include "mdtSortFilterProxyModel.h"
#include<QWidget>
#include<QAbstractTableModel>
#include <QString>
#include <QVariant>
#include <QList>
#include <QVector>

mdtMvSortingSetupDialog::mdtMvSortingSetupDialog(QWidget *parent, QAbstractTableModel *model, mdtSortFilterProxyModel *proxyModel)
 : mdtItemsSelectorDialog(parent)
{
  Q_ASSERT(model != 0);
  Q_ASSERT(proxyModel != 0);

  pvModel = model;
  pvProxyModel = proxyModel;
  setWindowTitle(tr("Manage sorting"));
  setSortOptionEnabled(false);
  setAvailableItemsLabelText(tr("Available fields"));
  setSelectedItemsLabelText(tr("Selected fields"));

  displayAvailableColumns();
  displaySortedColumns();
}

void mdtMvSortingSetupDialog::accept() 
{
  applyNotSelectedColumns();
  applySelectedColumns();
  QDialog::accept();
}

void mdtMvSortingSetupDialog::displayAvailableColumns() 
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvProxyModel != 0);

  QVector<int> sortedColums;
  int i;
  QList<mdtItemsSelectorDialogItem> items;

  sortedColums = pvProxyModel->sortedColumns();
  for(i = 0; i < pvModel->columnCount(); ++i){
    if(!sortedColums.contains(i)){
      mdtItemsSelectorDialogItem item;
      item.setColumnIndex(i);
      item.setText(pvModel->headerData(i, Qt::Horizontal).toString());
      items.append(item);
    }
  }
  setAvailableItems(items);
}

void mdtMvSortingSetupDialog::displaySortedColumns() 
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvProxyModel != 0);

  QVector<int> sortedColums;
  QList<mdtItemsSelectorDialogItem> items;
  int i;
  int col;

  sortedColums = pvProxyModel->sortedColumns();
  for(i = 0; i < sortedColums.size(); ++i){
    mdtItemsSelectorDialogItem item;
    col = sortedColums.at(i);
    item.setColumnIndex(col);
    item.setText(pvModel->headerData(col, Qt::Horizontal).toString());
    items.append(item);
  }
  setSelectedItems(items);
}

void mdtMvSortingSetupDialog::applySelectedColumns() 
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvProxyModel != 0);

  int i;
  QList<mdtItemsSelectorDialogItem> items;
  mdtItemsSelectorDialogItem item;

  items = selectedItems();
  pvProxyModel->clearColumnsSortOrder();
  for(i = 0; i < items.size(); ++i){
    item = items.at(i);
    pvProxyModel->addColumnToSortOrder(item.columnIndex(), item.sortOrder());
  }
}

void mdtMvSortingSetupDialog::applyNotSelectedColumns() 
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvProxyModel != 0);

}

