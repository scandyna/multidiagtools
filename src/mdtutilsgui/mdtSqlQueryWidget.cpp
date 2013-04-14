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
#include "mdtSqlQueryWidget.h"
#include "mdtItemsSelectorDialog.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QVariant>

#include <QDebug>

mdtSqlQueryWidget::mdtSqlQueryWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
  // Setup data part
  pvModel = new QSqlQueryModel(this);
  twData->setModel(pvModel);
  
  connect(pbRefresh, SIGNAL(clicked()), this, SLOT(select()));
  connect(pbTables, SIGNAL(clicked()), this, SLOT(chooseTables()));
  connect(pbFields, SIGNAL(clicked()), this, SLOT(chooseFields()));
  connect(pbSort, SIGNAL(clicked()), this, SLOT(chooseFieldsSort()));
}

mdtSqlQueryWidget::~mdtSqlQueryWidget()
{
}

void mdtSqlQueryWidget::setDatabase(QSqlDatabase db)
{
  pvDb = db;
}

void mdtSqlQueryWidget::setHeaderTextsByFieldNames(const QHash<QString, QString> &mapping)
{
  pvHeaderTextsByFieldNames = mapping;
}

void mdtSqlQueryWidget::select()
{
  QSqlQuery query(pvDb);
  QString sql;
  QPair<QString, int> sortItem;
  int i;

  qDebug() << "Tables: " << pvDb.tables();
  
  sql = "SELECT * FROM example_dwn_tbl;";
  
  sql = "SELECT ";
  for(i=0; i<pvFields.size(); ++i){
    sql += " \"" + pvFields.at(i) + "\" ";
    if(i<(pvFields.size()-1)){
      sql += ",";
    }
  }
  sql += " FROM ";
  for(i=0; i<pvTables.size(); ++i){
    sql += " \"" + pvTables.at(i) + "\" ";
    if(i<(pvTables.size()-1)){
      sql += ",";
    }
  }
  if(!pvSortedFieldsWithOrder.isEmpty()){
    sql += " ORDER BY ";
    for(i=0; i<pvSortedFieldsWithOrder.size(); ++i){
      sortItem = pvSortedFieldsWithOrder.at(i);
      sql += " \"" + sortItem.first + "\" ";
      if(sortItem.second == Qt::AscendingOrder){
        sql += " ASC ";
      }else{
        sql += " DESC ";
      }
      if(i < (pvSortedFieldsWithOrder.size()-1)){
        sql += ",";
      }
    }
  }
  
  qDebug() << "SQL: " << sql;
  if(!query.exec(sql)){
    qDebug() << "Query failed, error: " << query.lastError();
    return;
  }
  pvModel->setQuery(query);
  updateHeaderTexts();
  twData->resizeColumnsToContents();
}

void mdtSqlQueryWidget::chooseTables()
{
  mdtItemsSelectorDialog *dialog;
  QStringList availableTables, tmpTables;
  int retval;
  int i;

  // Build available tables list
  tmpTables = pvDb.tables();
  for(i=0; i<tmpTables.size(); ++i){
    if(!pvTables.contains(tmpTables.at(i))){
      availableTables.append(tmpTables.at(i));
    }
  }
  // Setup and show dialog
  dialog = new mdtItemsSelectorDialog(this);
  dialog->setWindowTitle(tr("Choose tables"));
  dialog->setSortOptionEnabled(false);
  dialog->setAvailableItemsLabelText(tr("Available tables"));
  dialog->setAvailableItems(availableTables);
  dialog->setSelectedItemsLabelText(tr("Selected tables"));
  dialog->setSelectedItems(pvTables);
  retval = dialog->exec();
  if(retval == QDialog::Accepted){
    pvTables = dialog->selectedItems();
    qDebug() << "Selected tables: " << pvTables;
  }

  delete dialog;
}

void mdtSqlQueryWidget::chooseFields()
{
  mdtItemsSelectorDialog *dialog;
  QSqlRecord rec;
  int retval;
  int tableIndex, fieldIndex;
  QString fieldDisplayText;
  QStringList availableFields;
  QStringList selectedFields;
  QHash<QString, QString> tmpFieldNamesBySelectorDisplayTexts;
  QString fieldName;

  // Build available fields list
  if(pvTables.size() < 1){
    return;
  }
  if(pvTables.size() == 1){
    rec = pvDb.record(pvTables.at(0));
    for(fieldIndex=0; fieldIndex<rec.count(); ++fieldIndex){
      fieldDisplayText = pvHeaderTextsByFieldNames.value(rec.fieldName(fieldIndex));
      if(!pvFieldNamesBySelectorDisplayTexts.contains(fieldDisplayText)){
        availableFields.append(fieldDisplayText);
      }
      tmpFieldNamesBySelectorDisplayTexts.insert(fieldDisplayText, rec.fieldName(fieldIndex));
    }
  }else{
    for(tableIndex=0; tableIndex<pvTables.size(); ++tableIndex){
      rec = pvDb.record(pvTables.at(tableIndex));
      for(fieldIndex=0; fieldIndex<rec.count(); ++fieldIndex){
        fieldDisplayText = pvHeaderTextsByFieldNames.value(rec.fieldName(fieldIndex)) + " (Table: " + pvTables.at(tableIndex) + ")";
        if(!pvFieldNamesBySelectorDisplayTexts.contains(fieldDisplayText)){
          availableFields.append(fieldDisplayText);
        }
        tmpFieldNamesBySelectorDisplayTexts.insert(fieldDisplayText, rec.fieldName(fieldIndex));
      }
    }
  }
  // Build selected fields list
  selectedFields = pvFieldNamesBySelectorDisplayTexts.keys();
  // Setup and show dialog
  dialog = new mdtItemsSelectorDialog(this);
  dialog->setWindowTitle(tr("Choose fields"));
  dialog->setSortOptionEnabled(false);
  dialog->setAvailableItemsLabelText(tr("Available fields"));
  dialog->setAvailableItems(availableFields);
  dialog->setSelectedItemsLabelText(tr("Selected fields"));
  dialog->setSelectedItems(selectedFields);
  retval = dialog->exec();
  if(retval == QDialog::Accepted){
    selectedFields = dialog->selectedItems();
    pvFields.clear();
    pvFieldNamesBySelectorDisplayTexts.clear();
    for(fieldIndex=0; fieldIndex<selectedFields.size(); ++fieldIndex){
      fieldDisplayText = selectedFields.at(fieldIndex);
      fieldName = tmpFieldNamesBySelectorDisplayTexts.value(fieldDisplayText);
      pvFieldNamesBySelectorDisplayTexts.insert(fieldDisplayText, fieldName);
      pvFields.append(fieldName);
    }
    ///qDebug() << "Selected fields: " << pvFields;
    ///select();
  }

  delete dialog;
}

void mdtSqlQueryWidget::chooseFieldsSort()
{
  mdtItemsSelectorDialog *dialog;
  int retval;
  int fieldIndex;
  QString fieldDisplayText;
  QString fieldName;
  QStringList existingFields;
  QStringList sortedFields;
  ///QHash<QString, QString> tmpFieldNamesBySelectorDisplayTexts;

  // Build the existing fields list
  for(fieldIndex=0; fieldIndex<pvFields.size(); ++fieldIndex){
    if(!pvSortedFields.contains(pvFields.at(fieldIndex))){
      fieldDisplayText = pvHeaderTextsByFieldNames.value(pvFields.at(fieldIndex));
      existingFields.append(fieldDisplayText);
    }
  }
  if(existingFields.isEmpty()){
    return;
  }
  // Build the allready sorted fields list
  for(fieldIndex=0; fieldIndex<pvSortedFields.size(); ++fieldIndex){
    fieldDisplayText = pvHeaderTextsByFieldNames.value(pvSortedFields.at(fieldIndex));
    sortedFields.append(fieldDisplayText);
  }
  // Setup and show dialog
  dialog = new mdtItemsSelectorDialog(this);
  dialog->setWindowTitle(tr("Fields sort"));
  dialog->setSortOptionEnabled(true);
  dialog->setAvailableItemsLabelText(tr("Existing fields"));
  dialog->setAvailableItems(existingFields);
  dialog->setSelectedItemsLabelText(tr("Sorted fields"));
  dialog->setSelectedItems(sortedFields);
  retval = dialog->exec();
  if(retval == QDialog::Accepted){
    // Store selected items in sort fields (with technical names)
    sortedFields = dialog->selectedItems();
    pvSortedFields.clear();
    pvSortedFieldsWithOrder.clear();
    for(fieldIndex=0; fieldIndex<sortedFields.size(); ++fieldIndex){
      fieldDisplayText = sortedFields.at(fieldIndex);
      fieldName = pvFieldNamesBySelectorDisplayTexts.value(fieldDisplayText);
      /// \todo Add sort order
      pvSortedFields.append(fieldName);
      pvSortedFieldsWithOrder.append(QPair<QString, int>(fieldName, Qt::AscendingOrder));
    }
    qDebug() << "Selected sorting: " << pvSortedFieldsWithOrder;
  }
  delete dialog;
}

void mdtSqlQueryWidget::updateHeaderTexts()
{
  QSqlRecord rec;
  QVariant value;
  int i;

  rec = pvModel->record();
  for(i=0; i<rec.count(); ++i){
    value = pvHeaderTextsByFieldNames.value(rec.fieldName(i));
    if(value.isValid()){
      pvModel->setHeaderData(i, Qt::Horizontal, value);
    }
  }
}
