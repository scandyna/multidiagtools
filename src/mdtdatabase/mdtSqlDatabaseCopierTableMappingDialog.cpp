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

mdtSqlDatabaseCopierTableMappingDialog::mdtSqlDatabaseCopierTableMappingDialog(QWidget* parent)
 : QDialog(parent),
   pvMappingModel(new mdtSqlDatabaseCopierTableMappingModel(this)),
   pvDestinationFieldSelectionDelegate(new mdtComboBoxItemDelegate(this))
{
  setupUi(this);
  tvMapping->setModel(pvMappingModel);
  tvMapping->setItemDelegateForColumn(2, pvDestinationFieldSelectionDelegate);
  connect(tbResetMapping, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::resetFieldMapping);
  connect(tbMapByName, &QToolButton::clicked, this, &mdtSqlDatabaseCopierTableMappingDialog::mapByFieldName);
}

void mdtSqlDatabaseCopierTableMappingDialog::setDestinationTables(const QSqlDatabase& db, const QStringList& tables)
{
  pvDestinationDatabase = db;
  cbDestinationTable->clear();
  cbDestinationTable->addItems(tables);
}

void mdtSqlDatabaseCopierTableMappingDialog::setMapping(const mdtSqlDatabaseCopierTableMapping& m)
{
  // Setup destination field selection delegate
  pvDestinationFieldSelectionDelegate->clear();
  pvDestinationFieldSelectionDelegate->addItem("");
  pvDestinationFieldSelectionDelegate->addItems(m.getDestinationFieldNameList());
  // Update mapping model
  pvMappingModel->setMapping(m);
  // Update source and destination table names
  lbSourceTable->setText(m.sourceTableName());
  cbDestinationTable->setCurrentText(m.destinationTableName());

  resizeTableViewToContents();
}

mdtSqlDatabaseCopierTableMapping mdtSqlDatabaseCopierTableMappingDialog::mapping() const
{
  return pvMappingModel->mapping();
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
