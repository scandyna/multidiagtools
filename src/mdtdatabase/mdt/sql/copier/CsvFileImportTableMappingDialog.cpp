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
#include "CsvFileImportTableMappingDialog.h"
#include "CsvFileImportTableMappingModel.h"
#include "mdtCsvFileParserSettingsDialog.h"
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>

namespace mdt{ namespace sql{ namespace copier{

CsvFileImportTableMappingDialog::CsvFileImportTableMappingDialog(QWidget *parent)
 : TableMappingDialog(parent),
   pvMappingModel(new CsvFileImportTableMappingModel)
{
  // Setup source table widgets
  QHBoxLayout *l = new QHBoxLayout;
  lbSourceTable = new QLabel;
  QToolButton *tb = new QToolButton;
  tb->setText("...");
  l->addWidget(tb);
  l->addWidget(lbSourceTable);
  setSourceTableLayout(l);
  connect(tb, &QToolButton::clicked, this, &CsvFileImportTableMappingDialog::setupSourceFile);
}

void CsvFileImportTableMappingDialog::setupSourceFile()
{
  mdtCsvFileParserSettingsDialog dialog(this);

  if(dialog.exec() != QDialog::Accepted){
    return;
  }
}

const std::shared_ptr<const TableMapping> CsvFileImportTableMappingDialog::mappingBase() const
{
  return pvMappingModel->mapping();
}

const std::shared_ptr<TableMapping> CsvFileImportTableMappingDialog::mappingBase()
{
  return pvMappingModel->mapping();
}

TableMappingModel *CsvFileImportTableMappingDialog::mappingModelBase()
{
  return pvMappingModel;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
