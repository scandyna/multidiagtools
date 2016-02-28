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
#include "DatabaseCopierTableMappingDialog.h"
#include "mdtSqlDatabaseCopierTableMappingModel.h"
#include "mdt/sql/Database.h"
#include <QComboBox>
#include <QVBoxLayout>

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

DatabaseCopierTableMappingDialog::DatabaseCopierTableMappingDialog(QWidget *parent)
 : TableMappingDialog(parent),
   pvMappingModel(new mdtSqlDatabaseCopierTableMappingModel(this)),
   cbSourceTable(new QComboBox)
{
  setModel(pvMappingModel);
  // Setup source tables combobox
  auto *l = new QVBoxLayout;
  l->addWidget(cbSourceTable);
  setSourceTableLayout(l);
  connect(cbSourceTable, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &DatabaseCopierTableMappingDialog::setSourceTable);
}

void DatabaseCopierTableMappingDialog::setSourceDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(db.isOpen());

  pvMappingModel->setSourceDatabase(db);
  populateSourceTableCombobox(pvMappingModel->mapping().get());
}

void DatabaseCopierTableMappingDialog::setMapping(const std::shared_ptr<mdtSqlDatabaseCopierTableMapping> & m)
{
  Q_ASSERT(m);
  Q_ASSERT(m->sourceDatabase().isOpen());
  Q_ASSERT(m->destinationDatabase().isOpen());

  int previousCbIndex;
  int newCbIndex;

  populateSourceTableCombobox(m.get());
  /*
   * Update current source table.
   * If we allready have source table selected,
   * we must call setSourceTable() slot manually
   */
  previousCbIndex = cbSourceTable->currentIndex();
  newCbIndex = cbSourceTable->findText(m->sourceTableName());
  cbSourceTable->setCurrentIndex(newCbIndex);
  if(newCbIndex == previousCbIndex){
    setSourceTable(newCbIndex);
  }
  // Update mapping
  pvMappingModel->setMapping(m);
  updateMapping();
}

std::shared_ptr<mdtSqlDatabaseCopierTableMapping> DatabaseCopierTableMappingDialog::mapping() const
{
  return pvMappingModel->mapping();
}

void DatabaseCopierTableMappingDialog::setSourceTable(int cbIndex)
{
  if(cbIndex < 0){
    return;
  }
  if(!pvMappingModel->mapping()->sourceDatabase().isOpen()){
    return;
  }
  QString tableName = cbSourceTable->itemText(cbIndex);
  pvMappingModel->setSourceTable(tableName);
  updateSourceTableFieldSelectionDelegate();
}

const std::shared_ptr<const TableMapping> DatabaseCopierTableMappingDialog::mappingBase() const
{
  return pvMappingModel->mapping();
}

const std::shared_ptr<TableMapping> DatabaseCopierTableMappingDialog::mappingBase()
{
  return pvMappingModel->mapping();
}

TableMappingModel *DatabaseCopierTableMappingDialog::mappingModelBase()
{
  return pvMappingModel;
}

void DatabaseCopierTableMappingDialog::populateSourceTableCombobox(const mdtSqlDatabaseCopierTableMapping * const tm)
{
  Q_ASSERT(tm != nullptr);

  using mdt::sql::Database;

  auto tables = Database::getTables(tm->sourceDatabase(), Database::Tables | Database::Views);
  tables.removeAll(tm->destinationTableName());
  tables.sort();
  cbSourceTable->clear();
  cbSourceTable->addItems(tables);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
