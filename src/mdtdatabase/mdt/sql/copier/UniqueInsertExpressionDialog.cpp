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

UniqueInsertExpressionDialog::UniqueInsertExpressionDialog(const std::shared_ptr<const TableMapping> & tm, int itemIndex, QWidget* parent)
 : QDialog(parent),
   pvTableMapping(tm)
{
  Q_ASSERT(tm);

  setupUi(this);
  // Init expression
  if(itemIndex >= 0){
    Q_ASSERT(itemIndex < tm->itemsCount());
    pvExpression = pvTableMapping->itemAt(itemIndex).uniqueInsertExpression();
  }
  pvInitialKey = pvExpression.destinationFieldIndexList();
  // Setup match item model
  pvModel = new UniqueInsertExpressionModel(pvTableMapping, pvExpression, this);
  // Setup match item view
  twMatchItems->setModel(pvModel);
  twMatchItems->setSelectionMode(QAbstractItemView::SingleSelection);
  // Display current source
  lbSourceTable->setText(tm->sourceTableName());
  // Display current destination
  lbDestinationTable->setText(tm->destinationTableName());
  // Setup destination key edition
  auto *addFieldAction = leDestinationKey->addAction(QIcon::fromTheme("list-add"), QLineEdit::TrailingPosition);
  connect(addFieldAction, &QAction::triggered, this, &UniqueInsertExpressionDialog::addFieldToDestinationKey);
  auto *clearKeyAction = leDestinationKey->addAction(QIcon::fromTheme("edit-clear"), QLineEdit::TrailingPosition);
  connect(clearKeyAction, &QAction::triggered, this, &UniqueInsertExpressionDialog::clearDestinationKey);
  displayDestinationKey();
  // Setup destination field selection combobox
  auto *cb = new mdtComboBoxItemDelegate(this);
  cb->addItems(tm->getDestinationTableFieldNameList());
  twMatchItems->setItemDelegateForColumn(1, cb);
  // Setup source value field selection combobox
  cb = new mdtComboBoxItemDelegate(this);
  cb->addItems(tm->getSourceTableFieldNameList());
  twMatchItems->setItemDelegateForColumn(3, cb);
  // Signal slot connections
  connect(tbAddMatchItem, &QToolButton::clicked, this, &UniqueInsertExpressionDialog::addMatchItem);
  connect(tbRemoveMatchItem, &QToolButton::clicked, this, &UniqueInsertExpressionDialog::removeSelectedMatchItems);
  connect(tbResizeView, &QToolButton::clicked, this, &UniqueInsertExpressionDialog::resizeMatchItemViewToContents);

  resizeMatchItemViewToContents();
}

UniqueInsertExpressionDialog::UniqueInsertExpressionDialog(const std::shared_ptr<const TableMapping> & tm, QWidget* parent)
 : UniqueInsertExpressionDialog(tm, -1, parent)
{
}

void UniqueInsertExpressionDialog::accept()
{
  if(pvExpression.destinationFieldIndexCount() < 1){
    return;
  }

  QDialog::accept();
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

void UniqueInsertExpressionDialog::clearDestinationKey()
{
  pvExpression.clearDestinationFieldIndexList();
  displayDestinationKey();
}

void UniqueInsertExpressionDialog::addFieldToDestinationKey()
{
  ExpressionKeyFieldSelectionDialog fieldSelectDialog(pvTableMapping, this);

  // Setup and show field selection dialog
  fieldSelectDialog.setKey(pvExpression.destinationFieldIndexList());
  fieldSelectDialog.setInitialKey(pvInitialKey);
  if(fieldSelectDialog.exec() != QDialog::Accepted){
    return;
  }
  // Update expression with selected field
  Q_ASSERT(fieldSelectDialog.selectedFieldIndex() >= 0);
  pvExpression.addDestinationFieldIndex(fieldSelectDialog.selectedFieldIndex());
  displayDestinationKey();
}

void UniqueInsertExpressionDialog::displayDestinationKey()
{
  QString str;
  auto keyList = pvExpression.destinationFieldIndexList();
  const int lastIndex = keyList.count() - 1;

  leDestinationKey->clear();
  if(lastIndex < 0){
    return;
  }
  for(int i = 0; i < lastIndex; ++i){
    str += pvTableMapping->destinationTableFieldNameAt(keyList.at(i)) + ", ";
  }
  str += pvTableMapping->destinationTableFieldNameAt(keyList.at(lastIndex));
  leDestinationKey->setText(str);
}

void UniqueInsertExpressionDialog::resizeMatchItemViewToContents()
{
  twMatchItems->resizeColumnsToContents();
  twMatchItems->resizeRowsToContents();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
