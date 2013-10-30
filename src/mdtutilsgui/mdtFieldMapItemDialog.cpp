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
  ///pvTargetMapItem = 0;
  // Setup edition map item
  pvMapItem.setDataType(QVariant::String);
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
    if(field.displayText().isEmpty()){
      cbSourceField->addItem(field.name(), var);
    }else{
      cbSourceField->addItem(field.displayText(), var);
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
    if(field.displayText().isEmpty()){
      cbDestinationField->addItem(field.name(), var);
    }else{
      cbDestinationField->addItem(field.displayText(), var);
    }
  }
}

// For preview
void mdtFieldMapItemDialog::setSourceData(const QString & data) 
{
  lbSourceData->setText(data);
  updateDestinationPreview();
}

void mdtFieldMapItemDialog::setMapItem(const mdtFieldMapItem & item) 
{
  pvMapItem = item;
}

mdtFieldMapItem mdtFieldMapItemDialog::mapItem() const
{
  return pvMapItem;
}

void mdtFieldMapItemDialog::accept()
{
  QDialog::accept();
}

void mdtFieldMapItemDialog::setSourceField(int cbIndex)
{
  mdtFieldMapField field;

  if(cbIndex < 0){
    pvMapItem.setSourceFieldIndex(-1);
    pvMapItem.setSourceFieldName("");
    pvMapItem.setSourceFieldDisplayText("");
  }else{
    field = cbSourceField->itemData(cbIndex).value<mdtFieldMapField>();
    pvMapItem.setSourceFieldIndex(field.index());
    pvMapItem.setSourceFieldName(field.name());
    pvMapItem.setSourceFieldDisplayText(field.displayText());
  }
  lbSourceFieldName->setText(pvMapItem.sourceFieldName());
  lbSourceFieldIndex->setText(QString::number(pvMapItem.sourceFieldIndex()));
}

void mdtFieldMapItemDialog::setDestinationField(int cbIndex)
{
  mdtFieldMapField field;

  if(cbIndex < 0){
    pvMapItem.setDestinationFieldIndex(-1);
    pvMapItem.setDestinationFieldName("");
    pvMapItem.setDestinationFieldDisplayText("");
  }else{
    field = cbDestinationField->itemData(cbIndex).value<mdtFieldMapField>();
    pvMapItem.setDestinationFieldIndex(field.index());
    pvMapItem.setDestinationFieldName(field.name());
    pvMapItem.setDestinationFieldDisplayText(field.displayText());
  }
  lbDestinationFieldName->setText(pvMapItem.destinationFieldName());
  lbDestinationFieldIndex->setText(QString::number(pvMapItem.destinationFieldIndex()));
}

void mdtFieldMapItemDialog::updateDestinationPreview() 
{
  QString sourceData, destinationData;

  sourceData = lbSourceData->text();
  destinationData = pvMapItem.destinationData(sourceData).toString();
  lbDestinationData->setText(destinationData);
}

void mdtFieldMapItemDialog::setSourceDataStartOffset(int value)
{
  pvMapItem.setSourceFieldDataStartOffset(sbSourceStartOffset->value());
  pvMapItem.setSourceFieldDataEndOffset(sbSourceStartOffset->value() + sbSourceSize->value() - 1);
  updateDestinationPreview();
}

void mdtFieldMapItemDialog::setSourceDataEndOffset(int value)
{
  pvMapItem.setSourceFieldDataStartOffset(sbSourceStartOffset->value());
  pvMapItem.setSourceFieldDataEndOffset(sbSourceStartOffset->value() + sbSourceSize->value() - 1);
  updateDestinationPreview();
}

void mdtFieldMapItemDialog::setSrcSplitParameterWidgetsEnabled(bool enable) 
{
  sbSourceStartOffset->setEnabled(enable);
  sbSourceSize->setEnabled(enable);
  if(enable){
    pvMapItem.setSourceFieldDataStartOffset(sbSourceStartOffset->value());
    pvMapItem.setSourceFieldDataEndOffset(sbSourceStartOffset->value() + sbSourceSize->value() - 1);
  }else{
    pvMapItem.setSourceFieldDataStartOffset(-1);
    pvMapItem.setSourceFieldDataEndOffset(-1);
  }
  updateDestinationPreview();
}

