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
#include "mdtSqlCopierCodec.h"
#include <QToolButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QVector>

#include <QDebug>

mdtSqlCopierTableMappingWidget::mdtSqlCopierTableMappingWidget(QWidget* parent)
 : QWidget(parent),
   pvItemsContainerLayout(new QVBoxLayout)
{
  setupUi(this);
  pvItemsContainerLayout->addStretch(1);
  QWidget *itemsContainerWidget = new QWidget;
  itemsContainerWidget->setLayout(pvItemsContainerLayout);
  saItems->setWidget(itemsContainerWidget);
  connect(tbAddFieldMapping, &QToolButton::clicked, this, &mdtSqlCopierTableMappingWidget::addFieldMapping);
  connect(wSourceCodecSettings, &mdtSqlCopierCodecSettingsWidget::settingsChanged, this, &mdtSqlCopierTableMappingWidget::updateSourceCodecSettings);
  connect(wDestinationCodecSettings, &mdtSqlCopierCodecSettingsWidget::settingsChanged, this, &mdtSqlCopierTableMappingWidget::updateDestinationCodecSettings);
}

void mdtSqlCopierTableMappingWidget::updateSourceCodecSettings(const mdtSqlCopierCodecSettings & cs)
{
  Q_ASSERT(pvTableMapping.fieldMappingCount() == 0);

  pvTableMapping.setSource(cs);
}

void mdtSqlCopierTableMappingWidget::updateDestinationCodecSettings(const mdtSqlCopierCodecSettings & cs)
{
  Q_ASSERT(pvTableMapping.fieldMappingCount() == 0);

  pvTableMapping.setDestination(cs);
}

void mdtSqlCopierTableMappingWidget::addFieldMapping()
{
  // Disable source and destination codec settings
  wSourceCodecSettings->setEnabled(false);
  wDestinationCodecSettings->setEnabled(false);
  // Add new item
  addFieldMappingPv();
  editFieldMapping(pvFieldMappingWidgetItems.at(pvFieldMappingWidgetItems.size()-1));
}

void mdtSqlCopierTableMappingWidget::editFieldMapping(mdtSqlCopierTableMappingWidgetItem* widgetItem)
{
  Q_ASSERT(widgetItem != nullptr);

  mdtSqlCopierFieldMappingDialog dialog(this);
  int index = fieldMappingIndex(widgetItem);

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

  dialog.setMapping(pvTableMapping.fieldMappingAt(index));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update
  setFieldMappingAt(index, dialog.mapping());
}

void mdtSqlCopierTableMappingWidget::removeFieldMapping(mdtSqlCopierTableMappingWidgetItem* item)
{
  Q_ASSERT(item != nullptr);

  // Remove item
  removeFieldMappingPv(item);
  // Re-enable source and destination codec settings widgets if no field mapping exists anymore
  if(pvFieldMappingWidgetItems.isEmpty()){
    wSourceCodecSettings->setEnabled(true);
    wDestinationCodecSettings->setEnabled(true);
  }
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

int mdtSqlCopierTableMappingWidget::addFieldMappingPv()
{
  Q_ASSERT(pvFieldMappingWidgetItems.size() == pvTableMapping.fieldMappingCount());

  int index = pvFieldMappingWidgetItems.size();

  addFieldMappingWidgetItems(1);
  pvTableMapping.addFieldMapping(mdtSqlCopierFieldMapping());
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvTableMapping.fieldMappingCount());
  Q_ASSERT(pvFieldMappingWidgetItems.size() == pvTableMapping.fieldMappingCount());

  return index;
}

void mdtSqlCopierTableMappingWidget::removeFieldMappingPv(mdtSqlCopierTableMappingWidgetItem* widgetItem)
{
  Q_ASSERT(widgetItem != nullptr);

  int index = fieldMappingIndex(widgetItem);

  pvItemsContainerLayout->removeWidget(widgetItem);
  pvTableMapping.removeFieldMappingAt(index);
  pvFieldMappingWidgetItems.removeAt(index);
  delete widgetItem;
  Q_ASSERT(pvFieldMappingWidgetItems.size() == pvTableMapping.fieldMappingCount());
}

void mdtSqlCopierTableMappingWidget::setFieldMappingAt(int index, const mdtSqlCopierFieldMapping & fm)
{
  mdtSqlCopierCodec sourceCodec, destinationCodec;

  // Store field mapping
  pvTableMapping.setFieldMapping(index, fm);
  // Update related widget
  auto *widgetItem = pvFieldMappingWidgetItems.at(index);
  Q_ASSERT(widgetItem != nullptr);
  sourceCodec.setSettings(pvTableMapping.source());
  if(!sourceCodec.openTarget()){
    displayError(sourceCodec.lastError());
    return;
  }
  destinationCodec.setSettings(pvTableMapping.destination());
  if(!destinationCodec.openTarget()){
    displayError(destinationCodec.lastError());
    return;
  }
  widgetItem->setSourceFieldName(sourceCodec.fieldNameList().at(fm.sourceFieldIndex));
  widgetItem->setDestinationFieldName(destinationCodec.fieldNameList().at(fm.destinationFieldIndex));
}

int mdtSqlCopierTableMappingWidget::fieldMappingIndex(mdtSqlCopierTableMappingWidgetItem* widgetItem)
{
  Q_ASSERT(widgetItem != nullptr);

  int index = 0;

  for(const auto *item : pvFieldMappingWidgetItems){
    Q_ASSERT(item != nullptr);
    if(item == widgetItem){
      break;
    }
    ++index;
  }
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFieldMappingWidgetItems.size());
  Q_ASSERT(index < pvTableMapping.fieldMappingCount());

  return index;
}

void mdtSqlCopierTableMappingWidget::addFieldMappingWidgetItems(int n)
{
  Q_ASSERT(pvItemsContainerLayout->count() > 0);  // A stretch was added in constructor

  for(int i = 0; i < n; ++i){
    auto *item = new mdtSqlCopierTableMappingWidgetItem(this, this);
    pvItemsContainerLayout->insertWidget(pvItemsContainerLayout->count() - 1, item);
    pvFieldMappingWidgetItems.append(item);
  }
}
