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
#include "mdtSqlFieldSetupTableModel.h"
#include "mdtSqlSchemaTable.h"

mdtSqlFieldSetupWidget::mdtSqlFieldSetupWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
  // Setup field selection combobox
  pvFieldModel = new mdtSqlFieldSetupTableModel(this);
  cbField->setModel(pvFieldModel);
  cbField->setModelColumn(mdtSqlFieldSetupTableModel::NameIndex);
}

bool mdtSqlFieldSetupWidget::setData(const mdtSqlFieldSetupData & data, QSqlDatabase db)
{
  mdtSqlSchemaTable st;

  pvSetupData = data;
  // Reverse table
  if(!st.setupFromTable(data.tableName, db)){
    pvLastError = st.lastError();
    return false;
  }
  // Update
  setEditionMode(data.editionMode);
  lbTableName->setText(data.tableName);
  pvFieldModel->setFieldList(st.fieldList());

  return true;
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
