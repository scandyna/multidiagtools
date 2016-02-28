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
#include "TableMappingDialog.h"
// #include "mdtSqlDatabaseCopierTableMappingModel.h"
#include "mdtComboBoxItemDelegate.h"
#include "RelatedTableInsertExpressionDialog.h"
#include "UniqueInsertCriteriaDialog.h"
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>
#include <QTableView>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QModelIndex>

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

TableMappingDialog::TableMappingDialog(QWidget* parent)
 : QDialog(parent),
//    pvMappingModel(new mdtSqlDatabaseCopierTableMappingModel(this)),
   pvSourceFieldSelectionDelegate(new mdtComboBoxItemDelegate(this))
{
  setupUi(this);
  tvMapping->setSelectionBehavior(QAbstractItemView::SelectItems);
  tvMapping->setSelectionMode(QAbstractItemView::SingleSelection);
//   tvMapping->setModel(pvMappingModel);
  auto *sourceTypeDelegate = new mdtComboBoxItemDelegate(this);
//   pvMappingModel->setupItemTypeDelegate(sourceTypeDelegate);
  tvMapping->setItemDelegateForColumn(0, sourceTypeDelegate);
  tvMapping->setItemDelegateForColumn(2, pvSourceFieldSelectionDelegate);
  Q_ASSERT(tvMapping->selectionModel() != nullptr);
  connect(tvMapping->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TableMappingDialog::setSelectedRowFromSelection);
  connect(tbResetMapping, &QToolButton::clicked, this, &TableMappingDialog::resetFieldMapping);
  connect(tbMapByName, &QToolButton::clicked, this, &TableMappingDialog::mapByFieldName);
//   connect(cbSourceTable, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
//           this, &TableMappingDialog::setSourceTable);
  connect(tbRelatedTableExpression, &QToolButton::clicked, this, &TableMappingDialog::editRelatedTableExpression);
  connect(cbUniqueInsertCriteria, &QCheckBox::toggled, this, &TableMappingDialog::setUniqueInsertCriteriaEnabled);
  connect(tbUniqueInsertCriteria, &QToolButton::clicked, this, &TableMappingDialog::editUniqueInsertCriteria);
  cbUniqueInsertCriteria->setChecked(false);
  setUniqueInsertCriteriaEnabled(false);
  setSelectedRow(-1);
}

// void TableMappingDialog::setSourceTables(const QSqlDatabase & db, const QStringList & tables)
// {
//   pvSourceDatabase = db;
//   cbSourceTable->clear();
//   cbSourceTable->addItems(tables);
// }

// void TableMappingDialog::setMapping(const std::shared_ptr<mdtSqlDatabaseCopierTableMapping> & m)
// {
//   Q_ASSERT(m);
// 
//   int previousCbIndex;
//   int newCbIndex;
// 
//   // Setup source field selection delegate
//   pvSourceFieldSelectionDelegate->clear();
//   pvSourceFieldSelectionDelegate->addItem("");
//   pvSourceFieldSelectionDelegate->addItems(m->getSourceTableFieldNameList());
//   // Update source and destination table names
//   previousCbIndex = cbSourceTable->currentIndex();
//   newCbIndex = cbSourceTable->findText(m->sourceTableName());
//   cbSourceTable->setCurrentIndex(newCbIndex);
//   if(newCbIndex == previousCbIndex){
//     setSourceTable(newCbIndex);
//   }
//   lbDestinationTable->setText(m->destinationTableName());
//   // Update mapping model
//   pvMappingModel->setMapping(m);
// 
//   resizeTableViewToContents();
// }

// std::shared_ptr<mdtSqlDatabaseCopierTableMapping> TableMappingDialog::mapping() const
// {
//   return pvMappingModel->mapping();
// }

// mdtSqlDatabaseCopierTableMapping TableMappingDialog::mapping() const
// {
//   return pvMappingModel->mapping();
// }

// void TableMappingDialog::setSourceTable(int cbIndex)
// {
//   QString tableName = cbSourceTable->itemText(cbIndex);
// 
//   if(cbIndex < 0){
//     return;
//   }
//   ///pvMappingModel->setSourceTable(tableName, pvSourceDatabase, pvSourceFieldSelectionDelegate);
// }

void TableMappingDialog::resetFieldMapping()
{
  ///pvMappingModel->resetFieldMapping();
  setSelectedRow(-1);
}

void TableMappingDialog::mapByFieldName()
{
  ///pvMappingModel->generateFieldMappingByName();
  setSelectedRow(-1);
  resizeTableViewToContents();
}

void TableMappingDialog::editRelatedTableExpression()
{
  using mdt::sql::copier::RelatedTableInsertExpressionDialog;
  using mdt::sql::copier::TableMappingItem;

  if(pvSelectedRow < 0){
    return;
  }
  /**
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
  */
}

void TableMappingDialog::setUniqueInsertCriteriaEnabled(bool enable)
{
  using mdt::sql::copier::UniqueInsertCriteria;

  tbUniqueInsertCriteria->setEnabled(enable);
  if(enable){
    ///pvMappingModel->setUniqueInsertCriteria(pvEditingUniqueInsertCriteria);
  }else{
    ///pvMappingModel->setUniqueInsertCriteria(UniqueInsertCriteria());
  }
}

void TableMappingDialog::editUniqueInsertCriteria()
{
//   using mdt::sql::copier::UniqueInsertCriteria;
  using mdt::sql::copier::UniqueInsertCriteriaDialog;

  /**
  UniqueInsertCriteriaDialog dialog(pvMappingModel->mapping(), this);
  dialog.setCriteria(pvEditingUniqueInsertCriteria);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvEditingUniqueInsertCriteria = dialog.criteria();
  Q_ASSERT(!pvEditingUniqueInsertCriteria.isNull());
  pvMappingModel->setUniqueInsertCriteria(pvEditingUniqueInsertCriteria);
  */
}

void TableMappingDialog::resizeTableViewToContents()
{
  tvMapping->resizeColumnsToContents();
  tvMapping->resizeRowsToContents();
}

void TableMappingDialog::setSelectedRowFromSelection(const QItemSelection & selected, const QItemSelection &/* deselected*/)
{
  const auto indexes = selected.indexes();

  if(indexes.isEmpty()){
    pvSelectedRow = -1;
  }
  Q_ASSERT(indexes.size() == 1);
  setSelectedRow(indexes.at(0).row());
}

void TableMappingDialog::setSelectedRow(int row)
{
  pvSelectedRow = row;
  tbRelatedTableExpression->setEnabled(row >= 0);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
