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
#include "RelatedTableInsertExpressionDialog.h"
#include "RelatedTableInsertMatchItemModel.h"
#include "ExpressionKeyFieldSelectionDialog.h"
#include "mdtComboBoxItemDelegate.h"
#include "TableMapping.h"
#include "mdt/sql/FieldSelectionDialog.h"
#include "mdtError.h"
#include "mdtErrorDialog.h"
#include <QStringList>
#include <QAction>
#include <QSqlDatabase>
#include <QToolButton>
#include <QComboBox>

namespace mdt{ namespace sql{ namespace copier{

RelatedTableInsertExpressionDialog::RelatedTableInsertExpressionDialog(const std::shared_ptr<const TableMapping> & tm, int itemIndex, QWidget* parent)
 : QDialog(parent),
   pvTableMapping(tm)
{
  Q_ASSERT(pvTableMapping);

  QAction *act;

  setupUi(this);
  // Init expression
  if(itemIndex >= 0){
    Q_ASSERT(itemIndex < tm->itemsCount());
///    pvExpression = pvTableMapping->itemAt(itemIndex).uniqueInsertExpression();
  }
  // Setup match item model
  pvMatchItemModel = new RelatedTableInsertMatchItemModel(pvTableMapping, this);
  /// set exp
  // Setup match item view
  tvMatchItems->setModel(pvMatchItemModel);
  tvMatchItems->setSelectionMode(QAbstractItemView::SingleSelection);
  // Display current source
  lbSourceTable->setText(tm->sourceTableName());
  // Display current destination
  lbDestinationTable->setText(tm->destinationTableName());
  // Setup destination fields edition
  act = leDestinationFields->addAction(QIcon::fromTheme("list-add"), QLineEdit::TrailingPosition);
  connect(act, &QAction::triggered, this, &RelatedTableInsertExpressionDialog::addDestinationField);
  act = leDestinationFields->addAction(QIcon::fromTheme("edit-clear"), QLineEdit::TrailingPosition);
  connect(act, &QAction::triggered, this, &RelatedTableInsertExpressionDialog::clearDestinationFields);
  displayDestinationFields();
  // Setup match item edition
  pvRelatedDestinationFieldCombobox = new mdtComboBoxItemDelegate(this);
  tvMatchItems->setItemDelegateForColumn(1, pvRelatedDestinationFieldCombobox);
  pvRelatedSourceFieldCombobox = new mdtComboBoxItemDelegate(this);
  tvMatchItems->setItemDelegateForColumn(3, pvRelatedSourceFieldCombobox);
  connect(tbAddMatchItem, &QToolButton::clicked, this, &RelatedTableInsertExpressionDialog::addMatchItem);
  connect(tbRemoveMatchItem, &QToolButton::clicked, this, &RelatedTableInsertExpressionDialog::removeSelectedMatchItems);
  connect(tbResizeMatchItemView, &QToolButton::clicked, this, &RelatedTableInsertExpressionDialog::resizeMatchItemViewToContents);
  resizeMatchItemViewToContents();
  // Setup value source (table) selection
  connect(cbSourceValueTable, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &RelatedTableInsertExpressionDialog::onValueSourceChanged);
  populateValueSourceCombobox();
  // Setup destination related table selection
  connect(cbDestinationRelatedTable, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &RelatedTableInsertExpressionDialog::onDestinationRelatedTableChanged);
  populateDestinationRelatedTableCombobox();
  // Setup destination related key edition
  act = leDestinationRelatedKey->addAction(QIcon::fromTheme("list-add"), QLineEdit::TrailingPosition);
  connect(act, &QAction::triggered, this, &RelatedTableInsertExpressionDialog::addFieldToDestinationRelatedTableKey);
  act = leDestinationRelatedKey->addAction(QIcon::fromTheme("edit-clear"), QLineEdit::TrailingPosition);
  connect(act, &QAction::triggered, this, &RelatedTableInsertExpressionDialog::clearDestinationRelatedTableKey);
  displayDestinationRelatedTableKey();
}

RelatedTableInsertExpressionDialog::RelatedTableInsertExpressionDialog(const std::shared_ptr<const TableMapping>& tm, QWidget* parent)
 : RelatedTableInsertExpressionDialog(tm, -1, parent)
{
}

void RelatedTableInsertExpressionDialog::addDestinationField()
{
  ExpressionKeyFieldSelectionDialog fieldSelectDialog(pvTableMapping, this);

  // Setup and show field selection dialog
  fieldSelectDialog.setKey(pvExpression.destinationFieldIndexList());
  ///fieldSelectDialog.setInitialKey(pvInitialKey);
  if(fieldSelectDialog.exec() != QDialog::Accepted){
    return;
  }
  // Update expression with selected field
  Q_ASSERT(fieldSelectDialog.selectedFieldIndex() >= 0);
  pvExpression.addDestinationFieldIndex(fieldSelectDialog.selectedFieldIndex());
  displayDestinationFields();
}

void RelatedTableInsertExpressionDialog::clearDestinationFields()
{
  pvExpression.clearDestinationFieldIndexList();
  displayDestinationFields();
}

void RelatedTableInsertExpressionDialog::displayDestinationFields()
{
  const auto dfiList = pvExpression.destinationFieldIndexList();
  QStringList fnList;

  for(const auto dfi : dfiList){
    fnList << pvTableMapping->destinationTableFieldNameAt(dfi);
  }
  leDestinationFields->setText(fnList.join(", "));
}

void RelatedTableInsertExpressionDialog::populateValueSourceCombobox()
{
  auto db = pvTableMapping->sourceDatabase();

  cbSourceValueTable->clear();
  if(db.isValid() && db.isOpen()){
    auto tableList = db.tables(QSql::Tables);
    tableList.removeAll(pvTableMapping->sourceTableName());
    tableList.sort();
    cbSourceValueTable->addItem("");
    cbSourceValueTable->addItems(tableList);
  }else{
    cbSourceValueTable->addItem(pvTableMapping->sourceTableName());
  }
}

void RelatedTableInsertExpressionDialog::onValueSourceChanged(int /*cbIndex*/)
{
  // Clear
  pvMatchItemModel->clearMatchItemList();
  pvRelatedSourceFieldCombobox->clear();
  // Update source related table
  QString tableName = cbSourceValueTable->currentText();
  if(tableName.isEmpty()){
    return;
  }
  auto ret = pvMatchItemModel->setSourceRelatedTable(tableName);
  if(!ret){
    mdtErrorDialog dialog(ret.error());
    dialog.exec();
    return;
  }
  // Populate related source field selection combobox
  pvRelatedSourceFieldCombobox->addItems(pvMatchItemModel->getSourceRelatedTableFieldNameList());
}

void RelatedTableInsertExpressionDialog::populateDestinationRelatedTableCombobox()
{
  auto db = pvTableMapping->destinationDatabase();

  Q_ASSERT(db.isOpen());

  cbDestinationRelatedTable->clear();
  auto tableList = db.tables(QSql::Tables);
  tableList.removeAll(pvTableMapping->destinationTableName());
  tableList.sort();
  cbDestinationRelatedTable->addItem("");
  cbDestinationRelatedTable->addItems(tableList);
}

void RelatedTableInsertExpressionDialog::onDestinationRelatedTableChanged(int /*cbIndex*/)
{
  // Clear
  pvMatchItemModel->clearMatchItemList();
  pvRelatedDestinationFieldCombobox->clear();
  pvExpression.clearDestinationRelatedTableKey();
  displayDestinationRelatedTableKey();
  // Update destination related table
  QString tableName = cbDestinationRelatedTable->currentText();
  if(tableName.isEmpty()){
    return;
  }
  auto ret = pvMatchItemModel->setDestinationRelatedTable(tableName);
  if(!ret){
    mdtErrorDialog dialog(ret.error());
    dialog.exec();
    return;
  }
  // Populate related destination field selection combobox
  pvRelatedDestinationFieldCombobox->addItems(pvMatchItemModel->getDestinationRelatedTableFieldNameList());
}

void RelatedTableInsertExpressionDialog::addFieldToDestinationRelatedTableKey()
{
  using mdt::sql::FieldSelectionDialog;

  auto db = pvTableMapping->destinationDatabase();
  Q_ASSERT(db.isOpen());
  // Get selected table name
  QString tableName = cbDestinationRelatedTable->currentText();
  if(tableName.isEmpty()){
    return;
  }
  // Setup and show field selection dialog
  FieldSelectionDialog fsDialog(this);
  auto ret = fsDialog.setTable(tableName, db);
  if(!ret){
    mdtErrorDialog dialog(ret.error());
    dialog.exec();
    return;
  }
  fsDialog.setFieldSelectionMode(FieldSelectionMode::MultiSelection);
  fsDialog.setFieldIndexesToHide(pvExpression.destinationRelatedTableKey());
//   fsDialog.sort();
  if(fsDialog.exec() != QDialog::Accepted){
    return;
  }
  // Update expression with selected field
  auto fiList = fsDialog.getSelectedFieldIndexList();
  for(int fieldIndex : fiList){
    pvExpression.addDestinationRelatedFieldIndex(fieldIndex);
  }
//   Q_ASSERT(fsDialog.getSelectedFieldIndexList().count() == 1);
//   pvExpression.addDestinationRelatedFieldIndex(fsDialog.getSelectedFieldIndexList().at(0));
  displayDestinationRelatedTableKey();
}

void RelatedTableInsertExpressionDialog::clearDestinationRelatedTableKey()
{
  pvExpression.clearDestinationRelatedTableKey();
  displayDestinationRelatedTableKey();
}

void RelatedTableInsertExpressionDialog::displayDestinationRelatedTableKey()
{
  const auto tableFnList = pvMatchItemModel->getDestinationRelatedTableFieldNameList();
  const auto keyFiList = pvExpression.destinationRelatedTableKey();
  QStringList keyFnList;

  for(const auto fieldIndex : keyFiList){
    Q_ASSERT(fieldIndex < tableFnList.size());
    keyFnList << tableFnList.at(fieldIndex);
  }
  leDestinationRelatedKey->setText(keyFnList.join(", "));
}

void RelatedTableInsertExpressionDialog::addMatchItem()
{
  pvMatchItemModel->insertRows(pvMatchItemModel->rowCount()-1, 1);
  resizeMatchItemViewToContents();
}

void RelatedTableInsertExpressionDialog::removeSelectedMatchItems()
{
  Q_ASSERT(tvMatchItems->selectionModel() != nullptr);

  const auto indexList = tvMatchItems->selectionModel()->selectedIndexes();

  // Currently, we only support removing 1 row
  if(indexList.isEmpty()){
    return;
  }
  pvMatchItemModel->removeRows(indexList.at(0).row(), 1);
}

void RelatedTableInsertExpressionDialog::resizeMatchItemViewToContents()
{
  tvMatchItems->resizeColumnsToContents();
  tvMatchItems->resizeRowsToContents();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
