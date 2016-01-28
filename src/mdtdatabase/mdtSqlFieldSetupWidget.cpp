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
#include "mdtSqlTableSchemaModel.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlDriverType.h"
#include "mdtSqlFieldType.h"

#include <QDebug>

mdtSqlFieldSetupWidget::mdtSqlFieldSetupWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
  // Setup field selection combobox
  pvTabledSchemaModel = new mdtSqlTableSchemaModel(this);
  cbField->setModel(pvTabledSchemaModel);
  cbField->setModelColumn(mdtSqlTableSchemaModel::NameIndex);
  connect(cbField, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFieldParameters(int)));
}

void mdtSqlFieldSetupWidget::setEditionMode(mdtSqlFieldSetupEditionMode mode)
{
  switch(mode){
    case mdtSqlFieldSetupEditionMode::Creation:
      setEditionModeCreation();
      break;
    case mdtSqlFieldSetupEditionMode::Selection:
      setEditionModeSelection();
      break;
    case mdtSqlFieldSetupEditionMode::Edition:
      setEditionModeEdition();
      break;
  }
}

bool mdtSqlFieldSetupWidget::setTable(const QString & tableName, QSqlDatabase db)
{
  mdtSqlTableSchema st;

  // Reverse table
  if(!st.setupFromTable(tableName, db)){
    pvLastError = st.lastError();
    return false;
  }

  return setTable(st, db);
}

bool mdtSqlFieldSetupWidget::setTable(const mdtSqlTableSchema & tableSchema, QSqlDatabase db)
{
  // Fill available field types
  if(!populateFieldTypeCombobox(db.driverName())){
    return false;
  }
  // Update
  lbTableName->setText(tableSchema.tableName());
  pvTabledSchemaModel->setTableSchema(tableSchema, mdtSqlDriverType::typeFromName(db.driverName()));
  cbField->setCurrentIndex(0);

  return true;
}

void mdtSqlFieldSetupWidget::setField(int fieldIndex)
{
  cbField->setCurrentIndex(fieldIndex);
}

QString mdtSqlFieldSetupWidget::fieldName() const
{
  return cbField->currentText();
}

int mdtSqlFieldSetupWidget::fieldIndex() const
{
  return cbField->currentIndex();
}

void mdtSqlFieldSetupWidget::updateFieldParameters(int cbFieldIndex)
{
  mdtSqlField field = pvTabledSchemaModel->field(cbFieldIndex);

  leName->setText(field.name());
  setFieldType(field.type());
  sbLength->setValue(field.length());
  leDefaultValue->setText(field.defaultValue().toString());
  cbAutoIncrement->setChecked(field.isAutoValue());
  cbPartOfPk->setChecked(pvTabledSchemaModel->isPartOfPrimaryKey(cbFieldIndex));
  cbRequired->setChecked(field.isRequired());
  cbRequired->setChecked(field.isRequired());
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

void mdtSqlFieldSetupWidget::setFieldType(mdtSqlFieldType::Type type)
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
  cbPartOfPk->setEnabled(true);
  cbRequired->setVisible(true);
  cbRequired->setEnabled(true);
  cbAutoIncrement->setVisible(true);
  cbAutoIncrement->setEnabled(true);
//   cbPartOfPk->setVisible(true);
//   cbPartOfPk->setCheckable(true);
//   cbRequired->setVisible(true);
//   cbRequired->setCheckable(true);
//   cbAutoIncrement->setVisible(true);
//   cbAutoIncrement->setCheckable(true);
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
  cbPartOfPk->setEnabled(false);
  cbRequired->setEnabled(false);
  cbAutoIncrement->setEnabled(false);
//   cbPartOfPk->setCheckable(false);
//   cbRequired->setCheckable(false);
//   cbAutoIncrement->setCheckable(false);
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
  cbPartOfPk->setEnabled(true);
  cbRequired->setVisible(true);
  cbRequired->setEnabled(true);
  cbAutoIncrement->setVisible(true);
  cbAutoIncrement->setEnabled(true);
//   cbPartOfPk->setVisible(true);
//   cbPartOfPk->setCheckable(true);
//   cbRequired->setVisible(true);
//   cbRequired->setCheckable(true);
//   cbAutoIncrement->setVisible(true);
//   cbAutoIncrement->setCheckable(true);
}
