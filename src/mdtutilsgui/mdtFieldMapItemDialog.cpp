/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtFieldMapItemDialog.h"
#include "mdtFieldMapItem.h"
#include <QComboBox>
#include <QVariant>
#include <QLabel>

//#include <QDebug>

mdtFieldMapItemDialog::mdtFieldMapItemDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  pvTargetMapItem = 0;
  // Setup edition map item
  pvEditMapItem.setDataType(QVariant::String);
  // Setup start and end comboboxes
  connect(cbSourceField, SIGNAL(currentIndexChanged(int)), this, SLOT(setSourceField(int)));
  connect(cbDestinationField, SIGNAL(currentIndexChanged(int)), this, SLOT(setDestinationField(int)));
  // Setup splitting options
  sbSourceSize->setMinimum(1);
  sbSourceSize->setValue(10);
  setSrcSplitParameterWidgetsEnabled(false);
  connect(rbUseSplitting, SIGNAL(toggled(bool)), this, SLOT(setSrcSplitParameterWidgetsEnabled(bool)));
  connect(sbSourceStartOffset, SIGNAL(valueChanged(int)), this, SLOT(setSourceDataStartOffset(int)));
  connect(sbSourceSize, SIGNAL(valueChanged(int)), this, SLOT(setSourceDataEndOffset(int)));
  // Setup data preview
  setSourceData(tr("Some test data"));
}

mdtFieldMapItemDialog::~mdtFieldMapItemDialog() 
{
}

void mdtFieldMapItemDialog::setSourceFields(const QList<mdtFieldMapField> & fields) 
{
  int i;
  QVariant var;
  mdtFieldMapField field;

  cbSourceField->clear();
  for(i = 0; i < fields.size(); ++i){
    field = fields.at(i);
    var.setValue(field);
    if(field.displayText.isEmpty()){
      cbSourceField->addItem(field.name, var);
    }else{
      cbSourceField->addItem(field.displayText, var);
    }
  }
}

void mdtFieldMapItemDialog::setDestinationFields(const QList<mdtFieldMapField> & fields) 
{
  int i;
  QVariant var;
  mdtFieldMapField field;

  cbDestinationField->clear();
  for(i = 0; i < fields.size(); ++i){
    field = fields.at(i);
    var.setValue(field);
    if(field.displayText.isEmpty()){
      cbDestinationField->addItem(field.name, var);
    }else{
      cbDestinationField->addItem(field.displayText, var);
    }
  }
}

// For preview
void mdtFieldMapItemDialog::setSourceData(const QString & data) 
{
  lbSourceData->setText(data);
  updateDestinationPreview();
}

void mdtFieldMapItemDialog::setMapItem(mdtFieldMapItem *item) 
{
  Q_ASSERT(item != 0);

  pvTargetMapItem = item;
  pvEditMapItem.setFieldIndex(item->fieldIndex());
  pvEditMapItem.setFieldName(item->fieldName());
  pvEditMapItem.setFieldDisplayText(item->fieldDisplayText());
  pvEditMapItem.setSourceFieldIndex(item->sourceFieldIndex());
  pvEditMapItem.setSourceFieldName(item->sourceFieldName());
  pvEditMapItem.setSourceFieldDataStartOffset(item->sourceFieldDataStartOffset());
  pvEditMapItem.setSourceFieldDataEndOffset(item->sourceFieldDataEndOffset());
  pvEditMapItem.setDataType(item->dataType());
}

void mdtFieldMapItemDialog::accept()
{
  if(pvTargetMapItem != 0){
    pvTargetMapItem->setFieldIndex(pvEditMapItem.fieldIndex());
    pvTargetMapItem->setFieldName(pvEditMapItem.fieldName());
    pvTargetMapItem->setFieldDisplayText(pvEditMapItem.fieldDisplayText());
    pvTargetMapItem->setSourceFieldIndex(pvEditMapItem.sourceFieldIndex());
    pvTargetMapItem->setSourceFieldName(pvEditMapItem.sourceFieldName());
    pvTargetMapItem->setSourceFieldDataStartOffset(pvEditMapItem.sourceFieldDataStartOffset());
    pvTargetMapItem->setSourceFieldDataEndOffset(pvEditMapItem.sourceFieldDataEndOffset());
    pvTargetMapItem->setDataType(pvEditMapItem.dataType());
  }
  QDialog::accept();
}

void mdtFieldMapItemDialog::setSourceField(int cbIndex)
{
  mdtFieldMapField field;

  if(cbIndex < 0){
    pvEditMapItem.setSourceFieldIndex(-1);
    pvEditMapItem.setSourceFieldName("");
  }else{
    field = cbSourceField->itemData(cbIndex).value<mdtFieldMapField>();
    pvEditMapItem.setSourceFieldIndex(field.index);
    pvEditMapItem.setSourceFieldName(field.name);
  }
}

void mdtFieldMapItemDialog::setDestinationField(int cbIndex)
{
  mdtFieldMapField field;

  if(cbIndex < 0){
    pvEditMapItem.setFieldIndex(-1);
    pvEditMapItem.setFieldName("");
    pvEditMapItem.setFieldDisplayText("");
  }else{
    field = cbDestinationField->itemData(cbIndex).value<mdtFieldMapField>();
    pvEditMapItem.setFieldIndex(field.index);
    pvEditMapItem.setFieldName(field.name);
    pvEditMapItem.setFieldDisplayText(field.displayText);
  }
}

void mdtFieldMapItemDialog::updateDestinationPreview() 
{
  QString sourceData, destinationData;

  sourceData = lbSourceData->text();
  destinationData = pvEditMapItem.destinationData(sourceData).toString();
  lbDestinationData->setText(destinationData);
}

void mdtFieldMapItemDialog::setSourceDataStartOffset(int value)
{
  pvEditMapItem.setSourceFieldDataStartOffset(sbSourceStartOffset->value());
  pvEditMapItem.setSourceFieldDataEndOffset(sbSourceStartOffset->value() + sbSourceSize->value() - 1);
  updateDestinationPreview();
}

void mdtFieldMapItemDialog::setSourceDataEndOffset(int value)
{
  pvEditMapItem.setSourceFieldDataStartOffset(sbSourceStartOffset->value());
  pvEditMapItem.setSourceFieldDataEndOffset(sbSourceStartOffset->value() + sbSourceSize->value() - 1);
  updateDestinationPreview();
}

void mdtFieldMapItemDialog::setSrcSplitParameterWidgetsEnabled(bool enable) 
{
  sbSourceStartOffset->setEnabled(enable);
  sbSourceSize->setEnabled(enable);
  if(enable){
    pvEditMapItem.setSourceFieldDataStartOffset(sbSourceStartOffset->value());
    pvEditMapItem.setSourceFieldDataEndOffset(sbSourceStartOffset->value() + sbSourceSize->value() - 1);
  }else{
    pvEditMapItem.setSourceFieldDataStartOffset(-1);
    pvEditMapItem.setSourceFieldDataEndOffset(-1);
  }
  updateDestinationPreview();
}

