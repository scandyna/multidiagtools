/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "UniqueInsertCriteriaDialog.h"
#include "ExpressionMatchItemModel.h"
#include "mdtComboBoxItemDelegate.h"
#include "ExpressionKeyFieldSelectionDialog.h"
#include <QToolButton>
#include <QString>
#include <QStringList>
#include <QModelIndex>
#include <QModelIndexList>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QVector>
#include <QAction>
#include <QIcon>
#include <QInputDialog>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>

namespace mdt{ namespace sql{ namespace copier{

UniqueInsertCriteriaDialog::UniqueInsertCriteriaDialog(const std::shared_ptr<const TableMapping> & tm, QWidget* parent)
 : QDialog(parent),
   pvTableMapping(tm)
{
  Q_ASSERT(tm);

  setupUi(this);
  // Setup match item model
  pvModel = new ExpressionMatchItemModel(pvTableMapping, this);
  // Setup match item view
  twMatchItems->setModel(pvModel);
  twMatchItems->setSelectionMode(QAbstractItemView::SingleSelection);
  // Display current source
  lbSourceTable->setText(tm->sourceTableName());
  // Display current destination
  lbDestinationTable->setText(tm->destinationTableName());
  // Setup destination field selection combobox
  auto *cb = new mdtComboBoxItemDelegate(this);
  cb->addItems(tm->getDestinationTableFieldNameList());
  twMatchItems->setItemDelegateForColumn(1, cb);
  // Setup source value field selection combobox
  cb = new mdtComboBoxItemDelegate(this);
  cb->addItems(tm->getSourceTableFieldNameList());
  twMatchItems->setItemDelegateForColumn(3, cb);
  // Signal slot connections
  connect(tbAddMatchItem, &QToolButton::clicked, this, &UniqueInsertCriteriaDialog::addMatchItem);
  connect(tbRemoveMatchItem, &QToolButton::clicked, this, &UniqueInsertCriteriaDialog::removeSelectedMatchItems);
  connect(tbResizeView, &QToolButton::clicked, this, &UniqueInsertCriteriaDialog::resizeMatchItemViewToContents);

  resizeMatchItemViewToContents();
}

void UniqueInsertCriteriaDialog::setCriteria(const UniqueInsertCriteria & c)
{
  pvCriteria = c;
  pvModel->setExpressionMatchItemList(pvCriteria.matchItems());
}

void UniqueInsertCriteriaDialog::accept()
{
  pvCriteria.setMatchItems(pvModel->expressionMatchItemList());
  if(pvCriteria.isNull()){
    return;
  }

  QDialog::accept();
}

void UniqueInsertCriteriaDialog::addMatchItem()
{
  pvModel->insertRows(pvModel->rowCount()-1, 1);
  resizeMatchItemViewToContents();
}

void UniqueInsertCriteriaDialog::removeSelectedMatchItems()
{
  Q_ASSERT(twMatchItems->selectionModel() != nullptr);

  const auto indexList = twMatchItems->selectionModel()->selectedIndexes();

  // Currently, we only support removing 1 row
  if(indexList.isEmpty()){
    return;
  }
  pvModel->removeRows(indexList.at(0).row(), 1);
}

void UniqueInsertCriteriaDialog::resizeMatchItemViewToContents()
{
  twMatchItems->resizeColumnsToContents();
  twMatchItems->resizeRowsToContents();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
