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
#include "mdtFieldMapDialog.h"
#include "mdtFieldMapItem.h"
#include "mdtFieldMapItemDialog.h"
#include "mdtFieldListEditionDialog.h"
#include <QStringList>
#include <QTableWidgetItem>
#include <QList>
#include <QMap>
#include <QMultiMap>
#include <QModelIndex>
#include <QMessageBox>

#include <QDebug>

mdtFieldMapDialog::mdtFieldMapDialog(QWidget *parent) 
 : QDialog(parent)
{
  setupUi(this);
  connect(pbAdd, SIGNAL(clicked()), this, SLOT(addMapItem()));
  connect(pbRemove, SIGNAL(clicked()), this, SLOT(removeMapItem()));
  connect(pbEditDestinationFieldList, SIGNAL(clicked()), this, SLOT(editDestinationFieldList()));
  // Setup mapping table view
  twMapping->setSelectionMode(QTableWidget::SingleSelection);
  twMapping->setSelectionBehavior(QTableWidget::SelectRows);
  twMapping->setEditTriggers(QTableWidget::NoEditTriggers);
}

mdtFieldMapDialog::~mdtFieldMapDialog()
{
}

void mdtFieldMapDialog::setFieldMap(const mdtFieldMap &map)
{
  Q_ASSERT(!map.sourceFields().isEmpty());

  pvFieldMap = map;
  updateMappingTableView();
  updateSourcePreview();
  updateDestinationPreview();
}

mdtFieldMap mdtFieldMapDialog::fieldMap() const
{
  return pvFieldMap;
}

void mdtFieldMapDialog::addMapItem()
{
  mdtFieldMapItemDialog dialog(this);
  mdtFieldMapItem *item;

  // Check that we have destination fields available
  if(pvFieldMap.notMappedDestinationFields(mdtFieldMap::ReferenceByName).isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("There is no more destination field available."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }
  // Setup dialog and show it
  dialog.setSourceFields(pvFieldMap.sourceFields());
  dialog.setDestinationFields(pvFieldMap.notMappedDestinationFields(mdtFieldMap::ReferenceByName));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add map item
  item = new mdtFieldMapItem;
  *item = dialog.mapItem();
  pvFieldMap.addItem(item);
  updateMappingTableView();
  updateSourcePreview();
  updateDestinationPreview();
}

void mdtFieldMapDialog::removeMapItem()
{
  QList<QTableWidgetItem*> selectedItems;

  // Get selected items
  selectedItems = twMapping->selectedItems();
  if(selectedItems.isEmpty()){
    return;
  }
  Q_ASSERT(selectedItems.at(0) != 0);
  Q_ASSERT(selectedItems.at(0)->data(Qt::UserRole).value<mdtFieldMapItem*>() != 0);
  pvFieldMap.removeItem(selectedItems.at(0)->data(Qt::UserRole).value<mdtFieldMapItem*>());
  updateMappingTableView();
  updateSourcePreview();
  updateDestinationPreview();
}

void mdtFieldMapDialog::editDestinationFieldList()
{
  mdtFieldListEditionDialog dialog(this);

  // Setup dialog and show
  dialog.setAvailableFields(pvFieldMap.destinationFields(), 0);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result back and update previews
  pvFieldMap.updateDestinationFields(dialog.fields(), mdtFieldMap::ReferenceByName);
  twMapping->clear();
  updateMappingTableView();
  updateDestinationPreview();
}

void mdtFieldMapDialog::updateMappingTableView()
{
  QList<mdtFieldMapItem*> items;
  QMultiMap<int, mdtFieldMapItem*> sortMap;
  mdtFieldMapItem *fmItem;
  QTableWidgetItem *twSourceItem, *twDestinationItem;
  int row, i;
  QString str;
  QStringList mappingViewHeader;

  // Get map items and sort them by source field indexes
  items = pvFieldMap.items();
  i = items.size() - 1;
  while(i >= 0){
    fmItem = items.at(i);
    Q_ASSERT(fmItem != 0);
    sortMap.insert(fmItem->sourceFieldIndex(), fmItem);
    --i;
  }
  items = sortMap.values();
  // Update view
  twMapping->setRowCount(items.size());
  // Set header if needed
  if(twMapping->horizontalHeaderItem(0) == 0){
    mappingViewHeader << tr("Source field") << " -> " << tr("Destination field");
    twMapping->setColumnCount(mappingViewHeader.size());
    twMapping->setHorizontalHeaderLabels(mappingViewHeader);
  }
  for(row = 0; row < items.size(); ++row){
    fmItem = items.at(row);
    Q_ASSERT(fmItem != 0);
    // Update source field
    twSourceItem = twMapping->item(row, 0);
    if(twSourceItem == 0){
      twSourceItem = new QTableWidgetItem;
      twMapping->setItem(row, 0, twSourceItem);
    }
    if(fmItem->sourceFieldDisplayText().isEmpty()){
      str = fmItem->sourceFieldName();
    }else{
      str = fmItem->sourceFieldDisplayText();
    }
    if(fmItem->sourceFieldDataStartOffset() > -1){
      str += " [" + QString::number(fmItem->sourceFieldDataStartOffset()) + "-";
      if(fmItem->sourceFieldDataEndOffset() > -1){
        str += QString::number(fmItem->sourceFieldDataEndOffset());
      }else{
        str += "*";
      }
      str += "]";
    }
    twSourceItem->setData(Qt::DisplayRole, str);
    // Update destination field
    twDestinationItem = twMapping->item(row, 2);
    if(twDestinationItem == 0){
      twDestinationItem = new QTableWidgetItem;
      twMapping->setItem(row, 2, twDestinationItem);
    }
    if(fmItem->destinationFieldDisplayText().isEmpty()){
      twDestinationItem->setData(Qt::DisplayRole, fmItem->destinationFieldName());
    }else{
      twDestinationItem->setData(Qt::DisplayRole, fmItem->destinationFieldDisplayText());
    }
    // Store field map item pointer
    twSourceItem->setData(Qt::UserRole, QVariant::fromValue(fmItem));
    twDestinationItem->setData(Qt::UserRole, QVariant::fromValue(fmItem));
  }
  twMapping->resizeColumnsToContents();
  twMapping->resizeRowsToContents();
}

void mdtFieldMapDialog::updateSourcePreview() 
{
  QStringList header;

  header = pvFieldMap.sourceHeader();
  twSourcePreview->setColumnCount(header.size());
  twSourcePreview->setHorizontalHeaderLabels(header);
}

void mdtFieldMapDialog::updateDestinationPreview() 
{
  QStringList header;

  header = pvFieldMap.destinationHeader();
  twDestinationPreview->setColumnCount(header.size());
  twDestinationPreview->setHorizontalHeaderLabels(header);
}

