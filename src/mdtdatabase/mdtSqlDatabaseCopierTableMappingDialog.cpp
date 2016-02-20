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
#include "mdtSqlDatabaseCopierTableMappingDialog.h"
#include "mdtSqlDatabaseCopierTableMappingModel.h"
#include "mdtComboBoxItemDelegate.h"
#include "mdt/sql/copier/RelatedTableInsertExpressionDialog.h"
#include "mdt/sql/copier/UniqueInsertCriteriaDialog.h"
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>
#include <QTableView>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QModelIndex>

//#include <QDebug>

mdtSqlDatabaseCopierTableMappingDialog::mdtSqlDatabaseCopierTableMappingDialog(QWidget* parent)
 : QDialog(parent),
   pvMappingModel(new mdtSqlDatabaseCopierTableMappingModel(this)),
   pvSourceFieldSelectionDelegate(new mdtComboBoxItemDelegate(this))
{
  setupUi(this);
  tvMapping->setSelectionBehavior(QAbstractItemView::SelectItems);
  tvMapping->setSelectionMode(QAbstractItemView::SingleSelection);
  tvMapping->setModel(pvMappingModel);
  auto *sourceTypeDelegate = new mdtComboBoxItemDelegate(this);
  pvMappingModel->setupItemTypeDelegate(sourceTypeDelegate);
  tvMapping->setItemDelegateForColumn(0, sourceTypeDelegate);
  tvMapping->setItemDelegateForColumn(2, pvSourceFieldSelectionDelegate);
  Q_ASSERT(tvMapping->selectionModel() != nullptr);
  connect(tvMapping->selectionModel(), &QItemSelectionModel::selectionChanged, this, &mdtSqlDatabaseCopierTableMappingDialog::setSelectedRowFromSelection);
  connect(tbResetMapping, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::resetFieldMapping);
  connect(tbMapByName, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::mapByFieldName);
  connect(cbSourceTable, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &mdtSqlDatabaseCopierTableMappingDialog::setSourceTable);
  connect(tbRelatedTableExpression, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::editRelatedTableExpression);
  
  ///connect(pbUniqueInsertExpression, &QPushButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::editUniqueInsertExpression);
  connect(cbUniqueInsertCriteria, &QCheckBox::toggled, this, &mdtSqlDatabaseCopierTableMappingDialog::setUniqueInsertCriteriaEnabled);
  connect(tbUniqueInsertCriteria, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::editUniqueInsertCriteria);
  cbUniqueInsertCriteria->setChecked(false);
  setUniqueInsertCriteriaEnabled(false);
  setSelectedRow(-1);
}

void mdtSqlDatabaseCopierTableMappingDialog::setSourceTables(const QSqlDatabase & db, const QStringList & tables)
{
  pvSourceDatabase = db;
  cbSourceTable->clear();
  cbSourceTable->addItems(tables);
}

void mdtSqlDatabaseCopierTableMappingDialog::setMapping(const std::shared_ptr<mdtSqlDatabaseCopierTableMapping> & m)
{
  Q_ASSERT(m);

  int previousCbIndex;
  int newCbIndex;

  // Setup source field selection delegate
  pvSourceFieldSelectionDelegate->clear();
  pvSourceFieldSelectionDelegate->addItem("");
  pvSourceFieldSelectionDelegate->addItems(m->getSourceTableFieldNameList());
  // Update source and destination table names
  previousCbIndex = cbSourceTable->currentIndex();
  newCbIndex = cbSourceTable->findText(m->sourceTableName());
  cbSourceTable->setCurrentIndex(newCbIndex);
  if(newCbIndex == previousCbIndex){
    setSourceTable(newCbIndex);
  }
  lbDestinationTable->setText(m->destinationTableName());
  // Update mapping model
  pvMappingModel->setMapping(m);

  resizeTableViewToContents();
}

std::shared_ptr<mdtSqlDatabaseCopierTableMapping> mdtSqlDatabaseCopierTableMappingDialog::mapping() const
{
  return pvMappingModel->mapping();
}

// mdtSqlDatabaseCopierTableMapping mdtSqlDatabaseCopierTableMappingDialog::mapping() const
// {
//   return pvMappingModel->mapping();
// }

void mdtSqlDatabaseCopierTableMappingDialog::setSourceTable(int cbIndex)
{
  QString tableName = cbSourceTable->itemText(cbIndex);

  if(cbIndex < 0){
    return;
  }
  pvMappingModel->setSourceTable(tableName, pvSourceDatabase, pvSourceFieldSelectionDelegate);
}

void mdtSqlDatabaseCopierTableMappingDialog::resetFieldMapping()
{
  pvMappingModel->resetFieldMapping();
  setSelectedRow(-1);
}

void mdtSqlDatabaseCopierTableMappingDialog::mapByFieldName()
{
  pvMappingModel->generateFieldMappingByName();
  setSelectedRow(-1);
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierTableMappingDialog::editRelatedTableExpression()
{
  using mdt::sql::copier::RelatedTableInsertExpressionDialog;
  using mdt::sql::copier::TableMappingItem;

  if(pvSelectedRow < 0){
    return;
  }
  RelatedTableInsertExpressionDialog dialog(pvMappingModel->mapping(), this);
  if(!dialog.setTableMappingItemIndex(pvSelectedRow)){
    return;
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(!dialog.expression().isNull());
  TableMappingItem item(TableMappingItem::RelatedTableInsertExpressionType);
  item.setRelatedTableInsertExpression(dialog.expression());
  pvMappingModel->insertItem(item);
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierTableMappingDialog::setUniqueInsertCriteriaEnabled(bool enable)
{
  tbUniqueInsertCriteria->setEnabled(enable);
}

void mdtSqlDatabaseCopierTableMappingDialog::editUniqueInsertCriteria()
{
  using mdt::sql::copier::UniqueInsertCriteriaDialog;

  UniqueInsertCriteriaDialog dialog(pvMappingModel->mapping(), this);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
}

// void mdtSqlDatabaseCopierTableMappingDialog::editUniqueInsertExpression()
// {
//   using mdt::sql::copier::UniqueInsertExpressionDialog;
//   using mdt::sql::copier::TableMappingItem;
// 
//   if(pvSelectedRow < 0){
//     return;
//   }
//   UniqueInsertExpressionDialog dialog(pvMappingModel->mapping(), pvSelectedRow, this);
//   if(dialog.exec() != QDialog::Accepted){
//     return;
//   }
//   TableMappingItem item(TableMappingItem::UniqueInsertExpressionType);
//   item.setUniqueInsertExpression(dialog.expression());
//   pvMappingModel->insertItem(item);
//   resizeTableViewToContents();
// }

void mdtSqlDatabaseCopierTableMappingDialog::resizeTableViewToContents()
{
  tvMapping->resizeColumnsToContents();
  tvMapping->resizeRowsToContents();
}

void mdtSqlDatabaseCopierTableMappingDialog::setSelectedRowFromSelection(const QItemSelection & selected, const QItemSelection &/* deselected*/)
{
  const auto indexes = selected.indexes();

  if(indexes.isEmpty()){
    pvSelectedRow = -1;
  }
  Q_ASSERT(indexes.size() == 1);
  setSelectedRow(indexes.at(0).row());
}

void mdtSqlDatabaseCopierTableMappingDialog::setSelectedRow(int row)
{
  pvSelectedRow = row;
  tbRelatedTableExpression->setEnabled(row >= 0);
  pbUniqueInsertExpression->setEnabled(row >= 0);
}
