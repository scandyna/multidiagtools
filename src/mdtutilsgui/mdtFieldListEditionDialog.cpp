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
#include "mdtFieldListEditionDialog.h"
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMessageBox>

#include <QDebug>

mdtFieldListEditionDialog::mdtFieldListEditionDialog(QWidget *parent)
 : QDialog(parent)
{
  QStringList header;

  setupUi(this);
  // Setup header
  header << tr("Column index") << tr("Field name") << tr("Header text");
  twFields->setColumnCount(3);
  twFields->setHorizontalHeaderLabels(header);

  twFields->setSelectionMode(QTableWidget::SingleSelection);
  // Connect push buttons
  connect(pbAdd, SIGNAL(clicked()), this, SLOT(addField()));
  connect(pbRemove, SIGNAL(clicked()), this, SLOT(removeField()));
  connect(pbUp, SIGNAL(clicked()), this, SLOT(moveSelectedItemUp()));
  connect(pbDown, SIGNAL(clicked()), this, SLOT(moveSelectedItemDown()));
}

mdtFieldListEditionDialog::~mdtFieldListEditionDialog()
{
}

void mdtFieldListEditionDialog::setAvailableFields(const QList<mdtFieldMapField> &fields, int startFieldIndex)
{
  int i;
  QTableWidgetItem *twItem;
  mdtFieldMapField field;

  pvStartFieldIndex = startFieldIndex;
  twFields->setRowCount(fields.size());
  for(i = 0; i < fields.size(); ++i){
    field = fields.at(i);
    // Column index
    twItem = twFields->item(i, 0);
    if(twItem == 0){
      twItem = new QTableWidgetItem;
    }
    twItem->setFlags(Qt::ItemIsSelectable);
    twItem->setText(QString::number(field.index()));
    twFields->setItem(i, 0, twItem);
    // Field name
    twItem = twFields->item(i, 1);
    if(twItem == 0){
      twItem = new QTableWidgetItem;
    }
    twItem->setText(field.name());
    twFields->setItem(i, 1, twItem);
    // Field display text
    twItem = twFields->item(i, 2);
    if(twItem == 0){
      twItem = new QTableWidgetItem;
    }
    twItem->setText(field.displayText());
    twFields->setItem(i, 2, twItem);
  }
  // Sort by field indexes
  twFields->sortByColumn(0, Qt::AscendingOrder);
  // Re-index fields
  reIndexFields();
}

const QList<mdtFieldMapField> &mdtFieldListEditionDialog::fields() const
{
  return pvResultFields;
}

void mdtFieldListEditionDialog::addField()
{
  int index;
  QTableWidgetItem *twItem;

  // Get index of last field
  twItem = twFields->item(twFields->rowCount() - 1, 0);
  if(twItem == 0){
    index = 0;
  }else{
    index = twItem->data(Qt::DisplayRole).toInt() + 1;
  }
  // Insert a row
  twFields->insertRow(twFields->rowCount());
  // Set index item
  twItem = new QTableWidgetItem;
  twItem->setFlags(Qt::ItemIsSelectable);
  twItem->setData(Qt::DisplayRole, index);
  twFields->setItem(twFields->rowCount() - 1, 0, twItem);
  // Set field name item
  twItem = new QTableWidgetItem;
  twItem->setData(Qt::DisplayRole, QString());
  twFields->setItem(twFields->rowCount() - 1, 1, twItem);
  // Set field display text item
  twItem = new QTableWidgetItem;
  twItem->setData(Qt::DisplayRole, QString());
  twFields->setItem(twFields->rowCount() - 1, 2, twItem);
}

void mdtFieldListEditionDialog::removeField()
{
  QList<QTableWidgetItem*> selectedItems;
  int row;

  // Get selected items
  selectedItems = twFields->selectedItems();
  if(selectedItems.isEmpty()){
    return;
  }
  Q_ASSERT(selectedItems.at(0) != 0);
  row = selectedItems.at(0)->row();
  // QTableWidget will also delete items itself
  twFields->removeRow(row);
  // Re-index fields
  reIndexFields();
}

void mdtFieldListEditionDialog::moveSelectedItemUp()
{
  QTableWidgetItem *twItem;
  int row, column;

  twItem = twFields->currentItem();
  if(twItem == 0){
    return;
  }
  row = twItem->row();
  column = twItem->column();
  swapRows(row - 1, row);
  // Update selection
  twItem = twFields->item(row - 1, column);
  if(twItem != 0){
    twFields->setCurrentItem(twItem);
  }
}

void mdtFieldListEditionDialog::moveSelectedItemDown()
{
  QTableWidgetItem *twItem;
  int row, column;

  twItem = twFields->currentItem();
  if(twItem == 0){
    return;
  }
  row = twItem->row();
  column = twItem->column();
  swapRows(row, row + 1);
  // Update selection
  twItem = twFields->item(row + 1, column);
  if(twItem != 0){
    twFields->setCurrentItem(twItem);
  }
}

void mdtFieldListEditionDialog::accept()
{
  if(!buildResult()){
    return;
  }
  QDialog::accept();
}

void mdtFieldListEditionDialog::reIndexFields()
{
  int i;
  QTableWidgetItem *twItem;

  for(i = 0; i < twFields->rowCount(); ++i){
    twItem = twFields->item(i, 0);
    Q_ASSERT(twItem != 0);
    twItem->setData(Qt::DisplayRole, pvStartFieldIndex + i);
  }
}

bool mdtFieldListEditionDialog::buildResult()
{
  int i;
  QTableWidgetItem *twItem;
  QString fieldName;
  mdtFieldMapField field;
  int fieldIndex;
  QString fieldDisplayText;

  // Check about dupplicate field names a first time
  if(haveDupplicateFieldName()){
    return false;
  }
  pvResultFields.clear();
  for(i = 0; i < twFields->rowCount(); ++i){
    twItem = twFields->item(i, 1);
    Q_ASSERT(twItem != 0);
    fieldName = twItem->data(Qt::DisplayRole).toString();
    // If field name is empty, we simply fail
    if(fieldName.trimmed().isEmpty()){
      QMessageBox msgBox(this);
      msgBox.setText(tr("There are some fields that have no name."));
      msgBox.setInformativeText(tr("Please correct these fields and try again."));
      msgBox.setIcon(QMessageBox::Information);
      msgBox.exec();
      pvResultFields.clear();
      return false;
    }
    // Check about illegal strings
    fieldName = correctFieldName(fieldName);
    if(fieldName.isEmpty()){
      pvResultFields.clear();
      return false;
    }
    // Store new name
    twItem->setData(Qt::DisplayRole, fieldName);
    // Store to result
    field = mdtFieldMapField();
    twItem = twFields->item(i, 0);
    Q_ASSERT(twItem != 0);
    fieldIndex = twItem->data(Qt::DisplayRole).toInt();
    field.setIndex(fieldIndex);
    field.setName(fieldName);
    twItem = twFields->item(i, 2);
    Q_ASSERT(twItem != 0);
    fieldDisplayText = twItem->data(Qt::DisplayRole).toString();
    field.setDisplayText(fieldDisplayText);
    pvResultFields.append(field);
  }
  // Check about dupplicate field names a after possible string substitution
  if(haveDupplicateFieldName()){
    pvResultFields.clear();
    return false;
  }

  return true;
}

QString mdtFieldListEditionDialog::correctFieldName(const QString &name)
{
  QString newName;
  QString text;

  if(mdtFieldMapField::strContainsIllegalString(name)){
    // Suggesst a new name
    newName = mdtFieldMapField::getFieldName(name, "_");
    QMessageBox msgBox(this);
    text = tr("The field name '") + name + tr("' contains illegal characters.");
    text += "\n" + tr("Suggested name is: '") + newName + tr("'.");
    msgBox.setText(text);
    msgBox.setInformativeText(tr("Do you want to keep this suggested name ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if(msgBox.exec() != QMessageBox::Yes){
      return QString();
    }
  }else{
    newName = name;
  }

  return newName;
}

bool mdtFieldListEditionDialog::haveDupplicateFieldName()
{
  int i;
  QTableWidgetItem *twItem;
  QString fieldName;
  QStringList names;

  for(i = 0; i < twFields->rowCount(); ++i){
    twItem = twFields->item(i, 1);
    Q_ASSERT(twItem != 0);
    fieldName = twItem->data(Qt::DisplayRole).toString();
    // If field allready exists in list (=dupplicate), we fail
    if(names.contains(fieldName)){
      QMessageBox msgBox(this);
      msgBox.setText(tr("There are dupplicate field names."));
      msgBox.setInformativeText(tr("Please choose a distinct name for each field and try again."));
      msgBox.setIcon(QMessageBox::Information);
      msgBox.exec();
      return true;
    }
    names.append(fieldName);
  }

  return false;
}

void mdtFieldListEditionDialog::swapRows(int row1, int row2)
{
  QString row1Data, row2Data;
  QTableWidgetItem *item1, *item2;

  // If we have less than 2 rows, nothing can be swapped..
  if(twFields->rowCount() < 2){
    return;
  }
  // Check row1 range
  if((row1 < 0)||(row1 >= (twFields->rowCount() - 1))){
    return;
  }
  // Check row2 range
  if((row2 < 1)||(row2 >= twFields->rowCount())){
    return;
  }
  // Get row1 field name
  item1 = twFields->item(row1, 1);
  Q_ASSERT(item1 != 0);
  row1Data = item1->data(Qt::DisplayRole).toString();
  // Get row2 field name
  item2 = twFields->item(row2, 1);
  Q_ASSERT(item2 != 0);
  row2Data = item2->data(Qt::DisplayRole).toString();
  // Swap field names
  item1->setData(Qt::DisplayRole, row2Data);
  item2->setData(Qt::DisplayRole, row1Data);
  // Get row1 field display text
  item1 = twFields->item(row1, 2);
  Q_ASSERT(item1 != 0);
  row1Data = item1->data(Qt::DisplayRole).toString();
  // Get row2 field display text
  item2 = twFields->item(row2, 2);
  Q_ASSERT(item2 != 0);
  row2Data = item2->data(Qt::DisplayRole).toString();
  // Swap field display texts
  item1->setData(Qt::DisplayRole, row2Data);
  item2->setData(Qt::DisplayRole, row1Data);
}
