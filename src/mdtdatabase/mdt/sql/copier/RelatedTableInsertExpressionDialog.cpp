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
#include "TableMapping.h"
#include <QStringList>
#include <QAction>
#include <QSqlDatabase>

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
  // Setup value source
  populateValueSourceCombobox();
  
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
    tableList.sort();
    cbSourceValueTable->addItems(tableList);
  }else{
    cbSourceValueTable->addItem(pvTableMapping->sourceTableName());
  }
}

}}} // namespace mdt{ namespace sql{ namespace copier{
