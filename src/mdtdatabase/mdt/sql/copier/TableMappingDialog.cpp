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
#include "TableMapping.h"
#include "TableMappingModel.h"
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
   pvSourceFieldSelectionDelegate(new mdtComboBoxItemDelegate(this))
{
  setupUi(this);
  tvMapping->setSelectionBehavior(QAbstractItemView::SelectItems);
  tvMapping->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(tbResetMapping, &QToolButton::clicked, this, &TableMappingDialog::resetFieldMapping);
  connect(tbMapByName, &QToolButton::clicked, this, &TableMappingDialog::mapByFieldName);
  connect(tbRelatedTableExpression, &QToolButton::clicked, this, &TableMappingDialog::editRelatedTableExpression);
  connect(cbUniqueInsertCriteria, &QCheckBox::toggled, this, &TableMappingDialog::setUniqueInsertCriteriaEnabled);
  connect(tbUniqueInsertCriteria, &QToolButton::clicked, this, &TableMappingDialog::editUniqueInsertCriteria);
}

void TableMappingDialog::setSourceTableLayout(QLayout *l)
{
  Q_ASSERT(l != nullptr);
  Q_ASSERT(gbSourceTable->layout() == nullptr);

  gbSourceTable->setLayout(l);
}

void TableMappingDialog::setDestinationTableLayout(QLayout *l)
{
  Q_ASSERT(l != nullptr);
  Q_ASSERT(gbDestinationTable->layout() == nullptr);

  gbDestinationTable->setLayout(l);
}

void TableMappingDialog::setModel(TableMappingModel *model)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(tvMapping->model() == nullptr);

  tvMapping->setModel(model);
//   auto *sourceTypeDelegate = new mdtComboBoxItemDelegate(this);
//   model->setupItemTypeDelegate(sourceTypeDelegate);
//   tvMapping->setItemDelegateForColumn(0, sourceTypeDelegate);
  tvMapping->setItemDelegateForColumn(2, pvSourceFieldSelectionDelegate);
  Q_ASSERT(tvMapping->selectionModel() != nullptr);
  connect(tvMapping->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TableMappingDialog::setSelectedRowFromSelection);
  cbUniqueInsertCriteria->setChecked(false);
  setUniqueInsertCriteriaEnabled(false);
  setSelectedRow(-1);
}

void TableMappingDialog::updateMapping()
{
  Q_ASSERT(mappingBase());

  updateSourceTableFieldSelectionDelegate();
  resizeTableViewToContents();
}

void TableMappingDialog::updateSourceTableFieldSelectionDelegate()
{
  Q_ASSERT(mappingBase() != nullptr);

  pvSourceFieldSelectionDelegate->clear();
  pvSourceFieldSelectionDelegate->addItem("");
  pvSourceFieldSelectionDelegate->addItems(mappingBase()->getSourceTableFieldNameList());
}

void TableMappingDialog::resizeTableViewToContents()
{
  tvMapping->resizeColumnsToContents();
  tvMapping->resizeRowsToContents();
}

void TableMappingDialog::resetFieldMapping()
{
  Q_ASSERT(mappingModelBase() != nullptr);

  mappingModelBase()->resetFieldMapping();
  setSelectedRow(-1);
}

void TableMappingDialog::mapByFieldName()
{
  Q_ASSERT(mappingModelBase() != nullptr);

  mappingModelBase()->generateFieldMappingByName();
  setSelectedRow(-1);
  resizeTableViewToContents();
}

void TableMappingDialog::editRelatedTableExpression()
{
  Q_ASSERT(mappingBase());
  Q_ASSERT(mappingModelBase() != nullptr);

  if(pvSelectedRow < 0){
    return;
  }
  RelatedTableInsertExpressionDialog dialog(mappingBase(), this);
  if(!dialog.setTableMappingItemIndex(pvSelectedRow)){
    return;
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(!dialog.expression().isNull());
  TableMappingItem item(TableMappingItem::RelatedTableInsertExpressionType);
  item.setRelatedTableInsertExpression(dialog.expression());
  mappingModelBase()->insertItem(item);
  resizeTableViewToContents();
}

void TableMappingDialog::setUniqueInsertCriteriaEnabled(bool enable)
{
  Q_ASSERT(mappingModelBase() != nullptr);

  tbUniqueInsertCriteria->setEnabled(enable);
  if(enable){
    mappingModelBase()->setUniqueInsertCriteria(pvEditingUniqueInsertCriteria);
  }else{
    mappingModelBase()->setUniqueInsertCriteria(UniqueInsertCriteria());
  }
}

void TableMappingDialog::editUniqueInsertCriteria()
{
  Q_ASSERT(mappingBase());
  Q_ASSERT(mappingModelBase() != nullptr);

  UniqueInsertCriteriaDialog dialog(mappingBase(), this);
  dialog.setCriteria(pvEditingUniqueInsertCriteria);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvEditingUniqueInsertCriteria = dialog.criteria();
  Q_ASSERT(!pvEditingUniqueInsertCriteria.isNull());
  mappingModelBase()->setUniqueInsertCriteria(pvEditingUniqueInsertCriteria);
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
