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
#include "mdtSqlFieldMappingDialog.h"
#include "mdtSqlFieldSetupWidget.h"

mdtSqlFieldMappingDialog::mdtSqlFieldMappingDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  fsSourceSetupWidget->setEditionMode(mdtSqlFieldSetupEditionMode::Selection);
  fsDestinationSetupWidget->setEditionMode(mdtSqlFieldSetupEditionMode::Selection);
  rbUseSourceSplitting->setChecked(true);
  connect(rbNoSourceSplitting, &QRadioButton::toggled, this, &mdtSqlFieldMappingDialog::setSourceSplittingEditionEnabled);
  rbNoSourceSplitting->setChecked(true);
  rbUseDestinationCat->setChecked(true);
  connect(rbNoDestinationCat, &QRadioButton::toggled, this, &mdtSqlFieldMappingDialog::setDestinationCatEditionEnabled);
  rbNoDestinationCat->setChecked(true);
}

void mdtSqlFieldMappingDialog::setSourceEditionMode(mdtSqlFieldSetupEditionMode mode)
{
  fsSourceSetupWidget->setEditionMode(mode);
}

bool mdtSqlFieldMappingDialog::setSourceTable(const QString & tableName, QSqlDatabase db)
{
  if(!fsSourceSetupWidget->setTable(tableName, db)){
    pvLastError = fsSourceSetupWidget->lastError();
    return false;
  }
  return true;
}

bool mdtSqlFieldMappingDialog::setSourceTable(const mdtSqlSchemaTable & tableSchema, QSqlDatabase db)
{
  if(!fsSourceSetupWidget->setTable(tableSchema, db)){
    pvLastError = fsSourceSetupWidget->lastError();
    return false;
  }
  return true;
}

void mdtSqlFieldMappingDialog::setDestinationEditionMode(mdtSqlFieldSetupEditionMode mode)
{
  fsDestinationSetupWidget->setEditionMode(mode);
}

bool mdtSqlFieldMappingDialog::setDestinationTable(const QString & tableName, QSqlDatabase db)
{
  if(!fsDestinationSetupWidget->setTable(tableName, db)){
    pvLastError = fsDestinationSetupWidget->lastError();
    return false;
  }
  return true;
}

bool mdtSqlFieldMappingDialog::setDestinationTable(const mdtSqlSchemaTable & tableSchema, QSqlDatabase db)
{
  if(!fsDestinationSetupWidget->setTable(tableSchema, db)){
    pvLastError = fsDestinationSetupWidget->lastError();
    return false;
  }
  return true;
}

void mdtSqlFieldMappingDialog::setMapping(const mdtSqlFieldMappingData & mapping)
{
  pvMapping = mapping;
  fsSourceSetupWidget->setField(mapping.sourceFieldIndex);
  fsDestinationSetupWidget->setField(pvMapping.destinationFieldIndex);
}

void mdtSqlFieldMappingDialog::setSourceSplittingEditionEnabled(bool rbNoSourceSplittingEnabled)
{
  sbSourceSplittingStartOffset->setEnabled(!rbNoSourceSplittingEnabled);
  sbSourceSplittingSize->setEnabled(!rbNoSourceSplittingEnabled);
}

void mdtSqlFieldMappingDialog::setDestinationCatEditionEnabled(bool rbNoDestinationCatEnabled)
{
  leDestinationCatPrefix->setEnabled(!rbNoDestinationCatEnabled);
}
