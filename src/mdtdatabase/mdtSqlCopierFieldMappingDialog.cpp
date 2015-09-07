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
#include "mdtSqlCopierFieldMappingDialog.h"
#include "mdtSqlFieldSetupWidget.h"
#include <QSqlDatabase>

mdtSqlCopierFieldMappingDialog::mdtSqlCopierFieldMappingDialog(QWidget *parent)
 : QDialog(parent),
   pvSourceDatabaseFieldSelectionWidget(nullptr),
   pvDestinationDatabaseFieldSelectionWidget(nullptr)
{
  setupUi(this);
  rbUseSourceSplitting->setChecked(true);
  connect(rbNoSourceSplitting, &QRadioButton::toggled, this, &mdtSqlCopierFieldMappingDialog::setSourceSplittingEditionEnabled);
  rbNoSourceSplitting->setChecked(true);
  rbUseDestinationCat->setChecked(true);
  connect(rbNoDestinationCat, &QRadioButton::toggled, this, &mdtSqlCopierFieldMappingDialog::setDestinationCatEditionEnabled);
  rbNoDestinationCat->setChecked(true);
}

bool mdtSqlCopierFieldMappingDialog::setSource(const mdtSqlCopierCodecSettings & cs)
{
  buildSourceSelectionWidget(cs.type());
  if(cs.type() == mdtSqlCopierCodecSettings::SqliteCodec){
    Q_ASSERT(pvSourceDatabaseFieldSelectionWidget != nullptr);
    QSqlDatabase db = QSqlDatabase::database(cs.connectionName(), false);
    if(!pvSourceDatabaseFieldSelectionWidget->setTable(cs.tableName(), db)){
      pvLastError = pvSourceDatabaseFieldSelectionWidget->lastError();
      return false;
    }
  }
  return true;
}

void mdtSqlCopierFieldMappingDialog::setSourceField(int fieldIndex)
{
  if(pvSourceDatabaseFieldSelectionWidget != nullptr){
    pvSourceDatabaseFieldSelectionWidget->setField(fieldIndex);
  }
}

QString mdtSqlCopierFieldMappingDialog::sourceFieldName() const
{
  if(pvSourceDatabaseFieldSelectionWidget != nullptr){
    return pvSourceDatabaseFieldSelectionWidget->fieldName();
  }
  return QString();
}

bool mdtSqlCopierFieldMappingDialog::setDestination(const mdtSqlCopierCodecSettings& cs)
{
  buildDestinationSelectionWidget(cs.type());
  if(cs.type() == mdtSqlCopierCodecSettings::SqliteCodec){
    Q_ASSERT(pvDestinationDatabaseFieldSelectionWidget != nullptr);
    QSqlDatabase db = QSqlDatabase::database(cs.connectionName(), false);
    if(!pvDestinationDatabaseFieldSelectionWidget->setTable(cs.tableName(), db)){
      pvLastError = pvDestinationDatabaseFieldSelectionWidget->lastError();
      return false;
    }
  }
  return true;
}

void mdtSqlCopierFieldMappingDialog::setDestinationField(int fieldIndex)
{
  if(pvDestinationDatabaseFieldSelectionWidget != nullptr){
    pvDestinationDatabaseFieldSelectionWidget->setField(fieldIndex);
  }
}

QString mdtSqlCopierFieldMappingDialog::destinationFieldName() const
{
  if(pvDestinationDatabaseFieldSelectionWidget != nullptr){
    return pvDestinationDatabaseFieldSelectionWidget->fieldName();
  }
  return QString();
}

void mdtSqlCopierFieldMappingDialog::setMapping(const mdtSqlCopierFieldMapping & mapping)
{
  pvMapping = mapping;
  setSourceField(pvMapping.sourceFieldIndex);
  setDestinationField(pvMapping.destinationFieldIndex);
}

void mdtSqlCopierFieldMappingDialog::setSourceSplittingEditionEnabled(bool rbNoSourceSplittingEnabled)
{
  sbSourceSplittingStartOffset->setEnabled(!rbNoSourceSplittingEnabled);
  sbSourceSplittingSize->setEnabled(!rbNoSourceSplittingEnabled);
}

void mdtSqlCopierFieldMappingDialog::setDestinationCatEditionEnabled(bool rbNoDestinationCatEnabled)
{
  leDestinationCatPrefix->setEnabled(!rbNoDestinationCatEnabled);
}

void mdtSqlCopierFieldMappingDialog::accept()
{
  // Update source part of field mapping
  if(pvSourceDatabaseFieldSelectionWidget != nullptr){
    pvMapping.sourceFieldIndex = pvSourceDatabaseFieldSelectionWidget->fieldIndex();
  }
  // Update destination part of field mapping
  if(pvDestinationDatabaseFieldSelectionWidget != nullptr){
    pvMapping.destinationFieldIndex = pvDestinationDatabaseFieldSelectionWidget->fieldIndex();
  }
  /// \todo Checks
  
  QDialog::accept();
}

void mdtSqlCopierFieldMappingDialog::buildSourceSelectionWidget(mdtSqlCopierCodecSettings::CodecType codecType)
{
  switch(codecType){
    case mdtSqlCopierCodecSettings::SqliteCodec:
      buildSourceDatabaseSelectionWidget();
      break;
  }
}

void mdtSqlCopierFieldMappingDialog::buildSourceDatabaseSelectionWidget()
{
  Q_ASSERT(gbSourceField->layout() != nullptr);

  if(pvSourceDatabaseFieldSelectionWidget != nullptr){
    return;
  }
  pvSourceDatabaseFieldSelectionWidget = new mdtSqlFieldSetupWidget;
  gbSourceField->layout()->addWidget(pvSourceDatabaseFieldSelectionWidget);
  pvSourceDatabaseFieldSelectionWidget->setEditionMode(mdtSqlFieldSetupEditionMode::Selection);
}

void mdtSqlCopierFieldMappingDialog::buildDestinationSelectionWidget(mdtSqlCopierCodecSettings::CodecType codecType)
{
  switch(codecType){
    case mdtSqlCopierCodecSettings::SqliteCodec:
      buildDestinationDatabaseSelectionWidget();
      break;
  }
}

void mdtSqlCopierFieldMappingDialog::buildDestinationDatabaseSelectionWidget()
{
  Q_ASSERT(gbDestinationField->layout() != nullptr);

  if(pvDestinationDatabaseFieldSelectionWidget != nullptr){
    return;
  }
  pvDestinationDatabaseFieldSelectionWidget = new mdtSqlFieldSetupWidget;
  gbDestinationField->layout()->addWidget(pvDestinationDatabaseFieldSelectionWidget);
  pvDestinationDatabaseFieldSelectionWidget->setEditionMode(mdtSqlFieldSetupEditionMode::Selection);
}
