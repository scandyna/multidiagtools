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
#include "UniqueInsertExpressionDialog.h"
#include "UniqueInsertExpressionModel.h"
#include "mdtComboBoxItemDelegate.h"
#include <QToolButton>
#include <QString>
#include <QModelIndex>
#include <QModelIndexList>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QVector>

namespace mdt{ namespace sql{ namespace copier{

UniqueInsertExpressionDialog::UniqueInsertExpressionDialog(const TableMapping & tm, UniqueInsertExpression & exp, QWidget* parent)
 : QDialog(parent),
   pvModel(new UniqueInsertExpressionModel(tm, exp, this)),
   pvTableMapping(tm)
{
  setupUi(this);
  twMatchItems->setModel(pvModel);
  twMatchItems->setSelectionMode(QAbstractItemView::SingleSelection);
  // Display current source
  lbSourceTable->setText(tm.sourceTableName());
  // Display current destination
  lbDestinationTable->setText(tm.destinationTableName());
  displayDestinationKey(exp);
  // Setup destination field selection combobox
  auto *cb = new mdtComboBoxItemDelegate(this);
  cb->addItems(tm.getDestinationTableFieldNameList());
  twMatchItems->setItemDelegateForColumn(1, cb);
  // Setup source value field selection combobox
  cb = new mdtComboBoxItemDelegate(this);
  cb->addItems(tm.getSourceTableFieldNameList());
  twMatchItems->setItemDelegateForColumn(3, cb);
  // Signal slot connections
  connect(tbAddMatchItem, &QToolButton::clicked, this, &UniqueInsertExpressionDialog::addMatchItem);
  connect(tbRemoveMatchItem, &QToolButton::clicked, this, &UniqueInsertExpressionDialog::removeSelectedMatchItems);
  connect(tbResizeView, &QToolButton::clicked, this, &UniqueInsertExpressionDialog::resizeMatchItemViewToContents);

  resizeMatchItemViewToContents();
}

void UniqueInsertExpressionDialog::addMatchItem()
{
  pvModel->insertRows(pvModel->rowCount()-1, 1);
  resizeMatchItemViewToContents();
}

void UniqueInsertExpressionDialog::removeSelectedMatchItems()
{
  Q_ASSERT(twMatchItems->selectionModel() != nullptr);

  const auto indexList = twMatchItems->selectionModel()->selectedIndexes();

  // Currently, we only support removing 1 row
  if(indexList.isEmpty()){
    return;
  }
  pvModel->removeRows(indexList.at(0).row(), 1);
}

void UniqueInsertExpressionDialog::displayDestinationKey(const UniqueInsertExpression & exp)
{
  QString str;
  auto keyList = exp.destinationFieldIndexList();
  const int lastIndex = keyList.count() - 1;

  leDestinationKey->clear();
  if(lastIndex < 0){
    return;
  }
  for(int i = 0; i < lastIndex; ++i){
    str += pvTableMapping.destinationTableFieldNameAt(keyList.at(i)) + ",";
  }
  str += pvTableMapping.destinationTableFieldNameAt(keyList.at(lastIndex));
  leDestinationKey->setText(str);
}

void UniqueInsertExpressionDialog::resizeMatchItemViewToContents()
{
  twMatchItems->resizeColumnsToContents();
  twMatchItems->resizeRowsToContents();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
