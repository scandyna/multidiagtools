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
#include <QStringList>
#include <QTableWidgetItem>
#include <QList>
#include <QMap>
#include <QMultiMap>
#include <QModelIndex>

mdtFieldMapDialog::mdtFieldMapDialog(QWidget *parent) 
 : QDialog(parent)
{
  QStringList mappingViewHeader;

  setupUi(this);
  connect(pbAdd, SIGNAL(clicked()), this, SLOT(addMapItem()));
  connect(pbEdit, SIGNAL(clicked()), this, SLOT(editMapItem()));
  connect(pbRemove, SIGNAL(clicked()), this, SLOT(removeMapItem()));
  // Setup mapping table view
  mappingViewHeader << tr("Source field") << " -> " << tr("Destination field");
  twMapping->setColumnCount(mappingViewHeader.size());
  twMapping->setHorizontalHeaderLabels(mappingViewHeader);
  twMapping->setSelectionMode(QTableWidget::SingleSelection);
  twMapping->setSelectionBehavior(QTableWidget::SelectRows);
  twMapping->setEditTriggers(QTableWidget::NoEditTriggers);
}

mdtFieldMapDialog::~mdtFieldMapDialog()
{
}

void mdtFieldMapDialog::setFieldMap(const mdtFieldMap &map)
{
  pvFieldMap = map;
}

mdtFieldMap mdtFieldMapDialog::fieldMap() const
{
  return pvFieldMap;
}

void mdtFieldMapDialog::addMapItem()
{
  mdtFieldMapItemDialog dialog(this);
  mdtFieldMapItem *item;

  // Setup dialog and show it
  /// \todo Check if one field list is empty !!
  ///dialog.setSourceFields(pvFieldMap.notMappedSourceFields(mdtFieldMap::ReferenceByName));
  dialog.setSourceFields(pvFieldMap.sourceFields());
  dialog.setDestinationFields(pvFieldMap.notMappedDestinationFields(mdtFieldMap::ReferenceByName));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  item = new mdtFieldMapItem;
  *item = dialog.mapItem();
  pvFieldMap.addItem(item);
  updateMappingTableView();
  updateSourcePreview();
  updateDestinationPreview();
}

void mdtFieldMapDialog::editMapItem()
{
  mdtFieldMapItemDialog dialog(this);

  dialog.exec();
}

void mdtFieldMapDialog::removeMapItem()
{
  QList<QTableWidgetItem*> selectedItems;

  // Get selected items
  selectedItems = twMapping->selectedItems();
  if(selectedItems.isEmpty()){
    return;
  }
  /// \todo complete: find a reliable relation between QTableWidget item and mdtFieldMap item.
  ///         Best way could be to add a hidden column with source field index ?
  ///         -> Or destination field index, because of splitting ??
}

void mdtFieldMapDialog::updateMappingTableView()
{
  QList<mdtFieldMapItem*> items;
  QMultiMap<int, mdtFieldMapItem*> sortMap;
  mdtFieldMapItem *fmItem;
  QTableWidgetItem *twSourceItem, *twDestinationItem;
  int row, i;

  // Get map items and sort them by source field indexes
  items = pvFieldMap.items();
  i = items.size() - 1;
  while(i >= 0){
    fmItem = items.at(i);
    Q_ASSERT(fmItem != 0);
    sortMap.insert(fmItem->sourceFieldIndex(), fmItem);
    --i;
  }
  /**
  for(i = 0; i < items.size(); ++i){
    fmItem = items.at(i);
    Q_ASSERT(fmItem != 0);
    sortMap.insert(fmItem->sourceFieldIndex(), fmItem);
  }
  */
  items = sortMap.values();
  // Update view
  twMapping->setRowCount(items.size());
  for(row = 0; row < items.size(); ++row){
    fmItem = items.at(row);
    Q_ASSERT(fmItem != 0);
    if(fmItem->sourceFieldDisplayText().isEmpty()){
      twSourceItem = new QTableWidgetItem(fmItem->sourceFieldName());
    }else{
      twSourceItem = new QTableWidgetItem(fmItem->sourceFieldDisplayText());
    }
    twMapping->setItem(row, 0, twSourceItem);
    if(fmItem->destinationFieldDisplayText().isEmpty()){
      twDestinationItem = new QTableWidgetItem(fmItem->destinationFieldName());
    }else{
      twDestinationItem = new QTableWidgetItem(fmItem->destinationFieldDisplayText());
    }
    twMapping->setItem(row, 2, twDestinationItem);
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

