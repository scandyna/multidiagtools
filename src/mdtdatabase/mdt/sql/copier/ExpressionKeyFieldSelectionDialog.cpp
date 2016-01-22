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
#include "ExpressionKeyFieldSelectionDialog.h"
#include "TableMapping.h"
#include <QVariant>
#include <QComboBox>
#include <QMessageBox>
#include <QChar>
#include <numeric>
#include <algorithm>
#include <iterator>

namespace mdt{ namespace sql{ namespace copier{

ExpressionKeyFieldSelectionDialog::ExpressionKeyFieldSelectionDialog(const std::shared_ptr<const TableMapping> & tm, QWidget* parent)
 : QDialog(parent),
   pvTableMapping(tm)
{
  Q_ASSERT(pvTableMapping);

  setupUi(this);
}

void ExpressionKeyFieldSelectionDialog::setKey(const FieldIndexList & key)
{
  // Store key
  pvKey = key;
  std::sort(pvKey.begin(), pvKey.end());  // set_difference() exepts sorted ranges
  // List all available fields in destination table
  std::vector<int> allAvailableFieldIndexList(pvTableMapping->destinationTableFieldCount());
  std::iota(allAvailableFieldIndexList.begin(), allAvailableFieldIndexList.end(), 0);
  // List unused fields
  std::vector<int> unusedFieldIndexList;
  std::set_difference(allAvailableFieldIndexList.cbegin(), allAvailableFieldIndexList.cend(),
                      pvKey.cbegin(), pvKey.cend(),
                      std::back_inserter(unusedFieldIndexList) );
  if(unusedFieldIndexList.empty()){
    return;
  }
  // Build available field name list
  cbAvailableFields->clear();
  for(const int fi : unusedFieldIndexList){
    cbAvailableFields->addItem(pvTableMapping->destinationTableFieldNameAt(fi), fi);
  }
}

void ExpressionKeyFieldSelectionDialog::setInitialKey(const FieldIndexList & key)
{
  pvInitialKey = key;
  std::sort(pvInitialKey.begin(), pvInitialKey.end());
}

void ExpressionKeyFieldSelectionDialog::accept()
{
  const auto fieldsToRemove = getMappedToRemoveFieldList();

  // If some mapped field will be removed, warn the user
  if(!fieldsToRemove.isEmpty()){
    QString msg;
    QMessageBox msgBox(this);
    msg = tr("Selected field will remove some allready mapped ones.") + "\n" \
        + tr("Mapped fields to remove: ") \
        + fieldsToRemove.join(QChar(','));
    msgBox.setText(msg);
    msgBox.setInformativeText(tr("Do you want to use this field ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  QDialog::accept();
}

int ExpressionKeyFieldSelectionDialog::selectedFieldIndex() const
{
  auto var = cbAvailableFields->currentData();

  if(var.isNull()){
    return -1;
  }
  return var.toInt();
}

QStringList ExpressionKeyFieldSelectionDialog::getMappedToRemoveFieldList() const
{
  QStringList fieldNameList;
  FieldIndexList selectedDfi;
  FieldIndexList dfiList;

  // Build selected key
  selectedDfi.append(selectedFieldIndex());
  // Ignore the initla key of expression we are editing
  std::set_difference( selectedDfi.cbegin(), selectedDfi.cend(),
                       pvInitialKey.cbegin(), pvInitialKey.cend(),
                       std::back_inserter(dfiList) );
  // Build list of allready mapped item indexes
  if(dfiList.isEmpty()){
    return fieldNameList;
  }
  const auto itemIndexList = pvTableMapping->getItemsToRemoveIndexList(dfiList);
  // Build field name list
  for(int itemIndex : itemIndexList){
    const auto itemDfiList = pvTableMapping->itemAt(itemIndex).destinationFieldIndexList();
    for(int dfi : itemDfiList){
      fieldNameList.append(pvTableMapping->destinationTableFieldNameAt(dfi));
    }
  }

  return fieldNameList;
}

}}} // namespace mdt{ namespace sql{ namespace copier{
