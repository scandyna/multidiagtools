/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include <QToolButton>

//#include <QDebug>

mdtSqlDatabaseCopierTableMappingDialog::mdtSqlDatabaseCopierTableMappingDialog(QWidget* parent)
 : QDialog(parent),
   pvMappingModel(new mdtSqlDatabaseCopierTableMappingModel(this)),
   pvSourceFieldSelectionDelegate(new mdtComboBoxItemDelegate(this))
{
  setupUi(this);
  tvMapping->setModel(pvMappingModel);
  tvMapping->setItemDelegateForColumn(0, pvSourceFieldSelectionDelegate);
  connect(tbResetMapping, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::resetFieldMapping);
  connect(tbMapByName, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::mapByFieldName);
  connect(cbSourceTable, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &mdtSqlDatabaseCopierTableMappingDialog::setSourceTable);
}

void mdtSqlDatabaseCopierTableMappingDialog::setSourceTables(const QSqlDatabase & db, const QStringList & tables)
{
  pvSourceDatabase = db;
  cbSourceTable->clear();
  cbSourceTable->addItems(tables);
}

void mdtSqlDatabaseCopierTableMappingDialog::setMapping(const mdtSqlDatabaseCopierTableMapping & m)
{
  int previousCbIndex;
  int newCbIndex;

  // Setup source field selection delegate
  pvSourceFieldSelectionDelegate->clear();
  pvSourceFieldSelectionDelegate->addItem("");
  pvSourceFieldSelectionDelegate->addItems(m.getSourceFieldNameList());
  // Update source and destination table names
  previousCbIndex = cbSourceTable->currentIndex();
  newCbIndex = cbSourceTable->findText(m.sourceTableName());
  cbSourceTable->setCurrentIndex(newCbIndex);
  if(newCbIndex == previousCbIndex){
    setSourceTable(newCbIndex);
  }
  lbDestinationTable->setText(m.destinationTableName());
  // Update mapping model
  pvMappingModel->setMapping(m);

  resizeTableViewToContents();
}

mdtSqlDatabaseCopierTableMapping mdtSqlDatabaseCopierTableMappingDialog::mapping() const
{
  return pvMappingModel->mapping();
}

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
}

void mdtSqlDatabaseCopierTableMappingDialog::mapByFieldName()
{
  pvMappingModel->generateFieldMappingByName();
  resizeTableViewToContents();
}

void mdtSqlDatabaseCopierTableMappingDialog::resizeTableViewToContents()
{
  tvMapping->resizeColumnsToContents();
  tvMapping->resizeRowsToContents();
}
