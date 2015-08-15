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
#include "mdtSqlFieldSetupWidget.h"
#include "mdtSqlSchemaTableModel.h"
#include "mdtSqlSchemaTable.h"
#include "mdtSqlDriverType.h"
#include "mdtSqlFieldType.h"

#include <QDebug>

mdtSqlFieldSetupWidget::mdtSqlFieldSetupWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
  // Setup field selection combobox
  pvSchemaTabledModel = new mdtSqlSchemaTableModel(this);
  cbField->setModel(pvSchemaTabledModel);
  cbField->setModelColumn(mdtSqlSchemaTableModel::NameIndex);
  connect(cbField, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFieldParameters(int)));
}

void mdtSqlFieldSetupWidget::setEditionMode(mdtSqlFieldSetupEditionMode_t mode)
{
  switch(mode){
    case mdtSqlFieldSetupEditionMode_t::Creation:
      setEditionModeCreation();
      break;
    case mdtSqlFieldSetupEditionMode_t::Selection:
      setEditionModeSelection();
      break;
    case mdtSqlFieldSetupEditionMode_t::Edition:
      setEditionModeEdition();
      break;
  }
}

bool mdtSqlFieldSetupWidget::setTable(const QString & tableName, QSqlDatabase db)
{
  mdtSqlSchemaTable st;

  // Reverse table
  if(!st.setupFromTable(tableName, db)){
    pvLastError = st.lastError();
    return false;
  }

  return setTable(st, db);
}

bool mdtSqlFieldSetupWidget::setTable(const mdtSqlSchemaTable & tableSchema, QSqlDatabase db)
{
  // Fill available field types
  if(!populateFieldTypeCombobox(db.driverName())){
    return false;
  }
  // Update
  lbTableName->setText(tableSchema.tableName());
  pvSchemaTabledModel->setTableSchema(tableSchema);
  cbField->setCurrentIndex(0);

  return true;
}

void mdtSqlFieldSetupWidget::setField(int fieldIndex)
{
  cbField->setCurrentIndex(fieldIndex);
}

// bool mdtSqlFieldSetupWidget::setData(const mdtSqlFieldSetupData & data, QSqlDatabase db)
// {
//   mdtSqlSchemaTable st;
// 
//   pvSetupData = data;
//   // Reverse table
//   if(!st.setupFromTable(data.tableName, db)){
//     pvLastError = st.lastError();
//     return false;
//   }
//   // Fill available field types
//   if(!populateFieldTypeCombobox(db.driverName())){
//     return false;
//   }
//   // Update
//   setEditionMode(data.editionMode);
//   lbTableName->setText(data.tableName);
//   pvSchemaTabledModel->setTableSchema(st);
//   cbField->setCurrentIndex(0);
// 
//   return true;
// }

void mdtSqlFieldSetupWidget::updateFieldParameters(int cbFieldIndex)
{
  QSqlField field = pvSchemaTabledModel->field(cbFieldIndex);

  leName->setText(field.name());
  setFieldType(field.type());
  sbLength->setValue(field.length());
  leDefaultValue->setText(field.defaultValue().toString());
  cbAutoIncrement->setChecked(field.isAutoValue());
  cbPartOfPk->setChecked(pvSchemaTabledModel->isPartOfPrimaryKey(cbFieldIndex));
  cbRequired->setChecked(field.requiredStatus() == QSqlField::QSqlField::Required);
}

bool mdtSqlFieldSetupWidget::populateFieldTypeCombobox(const QString & driverName)
{
  auto driverType = mdtSqlDriverType::typeFromName(driverName);

  cbType->clear();
  if(driverType == mdtSqlDriverType::Unknown){
    pvLastError.setError(tr("Unsupported driver '") + driverName + tr("'"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlFieldSetupWidget");
    pvLastError.commit();
    return false;
  }
  auto availableFieldTypeList = mdtSqlFieldType::availableFieldTypeList(driverType);
  for(const auto & fieldType : availableFieldTypeList){
    cbType->addItem(fieldType.name(), fieldType.type());
  }

  return true;
}

void mdtSqlFieldSetupWidget::setFieldType(QVariant::Type type)
{
  int index = cbType->findData(type);
  cbType->setCurrentIndex(index);
}

void mdtSqlFieldSetupWidget::setEditionModeCreation()
{
  lbField->setVisible(false);
  cbField->setVisible(false);
  lbName->setVisible(true);
  leName->setVisible(true);
  cbType->setVisible(true);
  cbType->setEnabled(true);
  sbLength->setVisible(true);
  sbLength->setEnabled(true);
  leDefaultValue->setVisible(true);
  leDefaultValue->setEnabled(true);
  cbPartOfPk->setVisible(true);
  cbPartOfPk->setCheckable(true);
  cbRequired->setVisible(true);
  cbRequired->setCheckable(true);
  cbAutoIncrement->setVisible(true);
  cbAutoIncrement->setCheckable(true);
}

void mdtSqlFieldSetupWidget::setEditionModeSelection()
{
  lbField->setVisible(true);
  cbField->setVisible(true);
  lbName->setVisible(false);
  leName->setVisible(false);
  cbType->setEnabled(false);
  sbLength->setEnabled(false);
  leDefaultValue->setEnabled(false);
  cbPartOfPk->setCheckable(false);
  cbRequired->setCheckable(false);
  cbAutoIncrement->setCheckable(false);
}

void mdtSqlFieldSetupWidget::setEditionModeEdition()
{
  lbField->setVisible(true);
  cbField->setVisible(true);
  lbName->setVisible(true);
  leName->setVisible(true);
  cbType->setVisible(true);
  cbType->setEnabled(true);
  sbLength->setVisible(true);
  sbLength->setEnabled(true);
  leDefaultValue->setVisible(true);
  leDefaultValue->setEnabled(true);
  cbPartOfPk->setVisible(true);
  cbPartOfPk->setCheckable(true);
  cbRequired->setVisible(true);
  cbRequired->setCheckable(true);
  cbAutoIncrement->setVisible(true);
  cbAutoIncrement->setCheckable(true);
}
