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
#include "mdtSqlCopierTableMappingWidget.h"
#include "mdtSqlCopierTableMappingWidgetItem.h"
#include "mdtSqlCopierFieldMapping.h"
#include "mdtSqlCopierFieldMappingDialog.h"
///#include "mdtSqlCopierCodecSettings.h"
#include <QToolButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QVector>

#include <QDebug>

mdtSqlCopierTableMappingWidget::mdtSqlCopierTableMappingWidget(QWidget* parent)
 : QWidget(parent),
   pvItemsContainerWidget(new QWidget),
   pvItemsContainerLayout(new QVBoxLayout)
{
  setupUi(this);
  pvItemsContainerLayout->addStretch(1);
  pvItemsContainerWidget->setLayout(pvItemsContainerLayout);
  saItems->setWidget(pvItemsContainerWidget);
  connect(tbRemoveFieldMapping, &QToolButton::clicked, this, &mdtSqlCopierTableMappingWidget::addFieldMapping);
  connect(wSourceCodecSettings, &mdtSqlCopierCodecSettingsWidget::settingsChanged, this, &mdtSqlCopierTableMappingWidget::updateSourceCodecSettings);
  connect(wDestinationCodecSettings, &mdtSqlCopierCodecSettingsWidget::settingsChanged, this, &mdtSqlCopierTableMappingWidget::updateDestinationCodecSettings);
}

void mdtSqlCopierTableMappingWidget::updateSourceCodecSettings(const mdtSqlCopierCodecSettings & cs)
{
  Q_ASSERT(pvFieldMappingItems.empty());

  pvTableMapping.setSource(cs);
}

void mdtSqlCopierTableMappingWidget::updateDestinationCodecSettings(const mdtSqlCopierCodecSettings & cs)
{
  Q_ASSERT(pvFieldMappingItems.empty());

  pvTableMapping.setDestination(cs);
}

void mdtSqlCopierTableMappingWidget::addFieldMapping()
{
  Q_ASSERT(pvItemsContainerLayout->count() > 0);  // A stretch was added in constructor

  // Disable source and destination codec settings
  wSourceCodecSettings->setEnabled(false);
  wDestinationCodecSettings->setEnabled(false);
  // Add new item
  auto *item = new mdtSqlCopierTableMappingWidgetItem(this, this);
  pvItemsContainerLayout->insertWidget(pvItemsContainerLayout->count() - 1, item);
  pvFieldMappingItems.emplace_back(mdtSqlCopierFieldMapping(), item);
  editFieldMapping(item);
}

void mdtSqlCopierTableMappingWidget::editFieldMapping(mdtSqlCopierTableMappingWidgetItem* item)
{
  Q_ASSERT(item != nullptr);

  mdtSqlCopierFieldMappingDialog dialog(this);
  std::vector<FieldMappingItem>::size_type fmItemIndex;
  FieldMappingItem fmItem;

  // Find field map item to update (and its index in vector)
  for(fmItemIndex = 0; fmItemIndex < pvFieldMappingItems.size(); ++fmItemIndex){
    if(pvFieldMappingItems[fmItemIndex].widget == item){
      fmItem = pvFieldMappingItems[fmItemIndex];
      break;
    }
  }
  Q_ASSERT(fmItem.widget != nullptr);

  // Setup dialog's source part
  if(!dialog.setSource(pvTableMapping.source())){
    displayError(dialog.lastError());
    return;
  }
  /// \todo Find first field index that was not allready mapped
  ///dialog.setSourceField(0);
  // Setup dialog's destination part
  if(!dialog.setDestination(pvTableMapping.destination())){
    displayError(dialog.lastError());
    return;
  }
  /// \todo Find first field index that was not allready mapped
  ///dialog.setDestinationField(0);
  
  dialog.setMapping(fmItem.mapping);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update
  item->setSourceFieldName(dialog.sourceFieldName());
  item->setDestinationFieldName(dialog.destinationFieldName());
  fmItem.mapping = dialog.mapping();
  pvFieldMappingItems[fmItemIndex] = fmItem;
  // Rebuild table mapping
  rebuildFieldMappingList();
}

void mdtSqlCopierTableMappingWidget::removeFieldMapping(mdtSqlCopierTableMappingWidgetItem* item)
{
  Q_ASSERT(item != nullptr);

  // Remove item
  pvItemsContainerLayout->removeWidget(item);
  for(auto it = pvFieldMappingItems.begin(); it != pvFieldMappingItems.end(); ++it){
    if(it->widget == item){
      pvFieldMappingItems.erase(it);
      break;
    }
  }
  delete item;
  // Re-enable source and destination codec settings widgets if no field mapping exists anymore
  if(pvFieldMappingItems.empty()){
    wSourceCodecSettings->setEnabled(true);
    wDestinationCodecSettings->setEnabled(true);
  }
}

void mdtSqlCopierTableMappingWidget::rebuildFieldMappingList()
{
  QVector<mdtSqlCopierFieldMapping> fmList;

  for(const auto & fmItem : pvFieldMappingItems){
    fmList.append(fmItem.mapping);
  }
  pvTableMapping.setFieldMappingList(fmList);
}

void mdtSqlCopierTableMappingWidget::displayError(const mdtError & error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
