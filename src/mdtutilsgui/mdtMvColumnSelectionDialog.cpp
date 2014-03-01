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
#include "mdtMvColumnSelectionDialog.h"
#include "mdtItemsSelectorDialogItem.h"
#include<QWidget>
#include<QAbstractTableModel>
#include<QTableView>
#include <QHeaderView>
#include <QString>
#include <QVariant>
#include <QList>
#include <vector>

//#include <QDebug>

mdtMvColumnSelectionDialog::mdtMvColumnSelectionDialog(QWidget *parent, QAbstractTableModel *model, QTableView *view)
 : mdtItemsSelectorDialog(parent)
{
  Q_ASSERT(model != 0);
  Q_ASSERT(view != 0);

  pvModel = model;
  pvView = view;
  setWindowTitle(tr("Choose fields"));
  setSortOptionEnabled(false);
  setAvailableItemsLabelText(tr("Available fields"));
  setSelectedItemsLabelText(tr("Selected fields"));

  displayAvailableColumns();
  displaySelectedColumns();
}

void mdtMvColumnSelectionDialog::accept() 
{
  applyNotSelectedColumns();
  applySelectedColumns();
  QDialog::accept();
}

void mdtMvColumnSelectionDialog::displayAvailableColumns() 
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvView != 0);

  int i;
  QList<mdtItemsSelectorDialogItem> items;

  for(i = 0; i < pvModel->columnCount(); ++i){
    if(pvView->isColumnHidden(i)){
      mdtItemsSelectorDialogItem item;
      item.setColumnIndex(i);
      item.setText(pvModel->headerData(i, Qt::Horizontal).toString());
      items.append(item);
    }
  }
  setAvailableItems(items);
}

void mdtMvColumnSelectionDialog::displaySelectedColumns() 
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvView != 0);
  Q_ASSERT(pvView->horizontalHeader() != 0);

  int i;
  unsigned int ui;
  QList<mdtItemsSelectorDialogItem> items;
  QHeaderView *hdv = pvView->horizontalHeader();
  std::vector<int> visualIndexes;
  int col;

  // Build list of visual indexes and sort it
  for(i = 0; i < hdv->count(); ++i){
    if(!hdv->isSectionHidden(i)){
      visualIndexes.push_back(hdv->visualIndex(i));
    }
  }
  std::sort(visualIndexes.begin(), visualIndexes.end());
  // Build items list
  for(ui = 0; ui < visualIndexes.size(); ++ui){
    mdtItemsSelectorDialogItem item;
    col = hdv->logicalIndex(visualIndexes[ui]);
    item.setColumnIndex(col);
    item.setText(pvModel->headerData(col, Qt::Horizontal).toString());
    items.append(item);
  }
  setSelectedItems(items);
}

void mdtMvColumnSelectionDialog::applySelectedColumns()
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvView != 0);
  Q_ASSERT(pvView->horizontalHeader() != 0);

  int i;
  QList<mdtItemsSelectorDialogItem> items;
  mdtItemsSelectorDialogItem item;
  QHeaderView *hdv = pvView->horizontalHeader();
  int visualIdx;

  items = selectedItems();
  for(i = 0; i < items.size(); ++i){
    item = items.at(i);
    pvView->setColumnHidden(item.columnIndex(), false);
    visualIdx = hdv->visualIndex(item.columnIndex());
    hdv->moveSection(visualIdx, i);
  }
}

void mdtMvColumnSelectionDialog::applyNotSelectedColumns()
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvView != 0);

  int i;
  QList<mdtItemsSelectorDialogItem> items;
  mdtItemsSelectorDialogItem item;

  items = notSelectedItems();
  for(i = 0; i < items.size(); ++i){
    item = items.at(i);
    pvView->setColumnHidden(item.columnIndex(), true);
  }
}

