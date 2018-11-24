/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "DataWidgetMapper.h"
#include "MappedWidget.h"
#include "WidgetStyleSheet.h"
#include "DataWidgetMapperItemDelegate.h"
#include "Mdt/ItemModel/RowRange.h"
#include "Mdt/ItemModel/ColumnRange.h"
#include <QAbstractItemModel>
#include <QWidget>
#include <QComboBox>
#include <QAbstractItemModel>
#include <QByteArray>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QScopedValueRollback>
#include <QLatin1String>
#include <QDebug>

// #include "Debug.h"

using namespace Mdt::ItemModel;

namespace Mdt{ namespace ItemEditor{

DataWidgetMapper::DataWidgetMapper(QObject* parent)
 : QObject(parent),
   mCurrentRow(-1),
   mUpdatingMappedWidget(false),
   mEditingState(false)
{
}

void DataWidgetMapper::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  if(model == mModel){
    return;
  }
  if(!mModel.isNull()){
    disconnect(mModel, &QAbstractItemModel::dataChanged, this, &DataWidgetMapper::onModelDataChanged);
    disconnect(mModel, &QAbstractItemModel::modelReset, this, &DataWidgetMapper::onModelReset);
  }
  mModel = model;
  connect(mModel, &QAbstractItemModel::dataChanged, this, &DataWidgetMapper::onModelDataChanged);
  connect(mModel, &QAbstractItemModel::modelReset, this, &DataWidgetMapper::onModelReset);
  setCurrentRow(-1);
}

QAbstractItemModel* DataWidgetMapper::model() const
{
  return mModel;
}

void DataWidgetMapper::addMapping(QWidget* widget, int column)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(column >= 0);
  Q_ASSERT(widget->metaObject() != nullptr);

  removeMapping(widget);
  removeMappingForColumn(column);
  MappedWidget *mappedWidget = mMappedWidgetList.addMapping(widget, column);
  Q_ASSERT(mappedWidget != nullptr);
  connect(mappedWidget, &MappedWidget::editionStarted, this, &DataWidgetMapper::onDataEditionStarted);
  updateMappedWidget(mappedWidget, column);
}

void DataWidgetMapper::addRelationalMapping(QComboBox *comboBox, int column, int comboBoxModelValueColumn)
{
  Q_ASSERT(comboBox != nullptr);
  Q_ASSERT(column >= 0);
  Q_ASSERT(comboBox->metaObject() != nullptr);
  Q_ASSERT(comboBox->model() != nullptr);
  Q_ASSERT(comboBoxModelValueColumn >= 0);
  Q_ASSERT(comboBoxModelValueColumn < comboBox->model()->columnCount());

  removeMapping(comboBox);
  removeMappingForColumn(column);
  MappedWidget *mappedWidget = mMappedWidgetList.addRelationalMapping(comboBox, column, comboBoxModelValueColumn);
  Q_ASSERT(mappedWidget != nullptr);
  connect(mappedWidget, &MappedWidget::editionStarted, this, &DataWidgetMapper::onDataEditionStarted);
  updateMappedWidget(mappedWidget, column);
}

void DataWidgetMapper::removeMapping(QWidget* widget)
{
  Q_ASSERT(widget != nullptr);

  int index = mMappedWidgetList.getIndexForWidget(widget);
  if(index < 0){
    return;
  }
  MappedWidget *mappedWidget = mMappedWidgetList.mappedWidgetAt(index);
  Q_ASSERT(mappedWidget != nullptr);
  disconnect(mappedWidget, &MappedWidget::editionStarted, this, &DataWidgetMapper::onDataEditionStarted);
  unmapWidget(mappedWidget);
  mMappedWidgetList.removeMappingAt(index);
}

void DataWidgetMapper::clearMapping()
{
  for(const auto & mw : mMappedWidgetList){
    auto *widget = mw->widget();
    if(widget != nullptr){
      unmapWidget(mw.get());
    }
  }
  mMappedWidgetList.clear();
}

void DataWidgetMapper::setCurrentRow(int row)
{
  if(mModel.isNull()){
    return;
  }
  bool rowHasChanged = (row != mCurrentRow);
  mCurrentRow = row;
  if(!mEditingState){
    updateAllMappedWidgets();
  }
  if(rowHasChanged){
    emit currentRowChanged(row);
  }
}

void DataWidgetMapper::updateFromModelFlags()
{
  for(const auto & mw : mMappedWidgetList){
    updateMappedWidgetFromModelFlags(mw.get(), mw->column());
  }
}

bool DataWidgetMapper::setDataToModel()
{
  Q_ASSERT(!mModel.isNull());
  Q_ASSERT(!mSubmittingFromMapper);

  QScopedValueRollback<bool> submittingFromMapperGuard(mSubmittingFromMapper);
  mSubmittingFromMapper = true;

  if(!commitAllMappedWidgetsData()){
    return false;
  }

  mEditingState = false;
  emit dataEditionDone();

  return true;
}

void DataWidgetMapper::revertDataFromModel()
{
  updateAllMappedWidgets();
  mEditingState = false;
  emit dataEditionDone();
}

void DataWidgetMapper::onDataEditionStarted()
{
  if(mEditingState || mUpdatingMappedWidget){
    return;
  }
  mEditingState = true;
  emit dataEditionStarted();
}

void DataWidgetMapper::onModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &)
{
  if(topLeft.parent().isValid()){
    return;
  }
  RowRange rowRange;
  rowRange.setFirstIndex(topLeft);
  rowRange.setLastIndex(bottomRight);
  if(!rowRange.containsRow(mCurrentRow)){
    return;
  }
  ColumnRange columnRange;
  columnRange.setFirstIndex(topLeft);
  columnRange.setLastIndex(bottomRight);
  for(const auto & mw : mMappedWidgetList){
    // Update current widget if its column is in range topLeft, bottomRight
    int column = mw->column();
    if(columnRange.containsColumn(column)){
      updateMappedWidget(mw.get(), column);
    }
  }
  /*
   * If we are submitting from mapper,
   * we only go out from editing mode when all widgets data have been set to model
   * and model's submit successfully returned.
   *
   * Otherwise, data has changed from a other editor,
   * and we have no control, we must simply go out from editing mode
   * and signal this once.
   */
  if(mEditingState && !mSubmittingFromMapper){
    mEditingState = false;
    emit dataEditionDone();
  }
}

void DataWidgetMapper::onModelReset()
{
  setCurrentRow(-1);
}

void DataWidgetMapper::removeMappingForColumn(int column)
{
  Q_ASSERT(column >= 0);

  int index = mMappedWidgetList.getIndexForColumn(column);
  if(index < 0){
    return;
  }
  unmapWidget( mMappedWidgetList.mappedWidgetAt(index) );
  mMappedWidgetList.removeMappingAt(index);
}

void DataWidgetMapper::unmapWidget(MappedWidget * const mappedWidget)
{
  Q_ASSERT(mappedWidget != nullptr);

  updateMappedWidget(mappedWidget, -1);
  mappedWidget->setWidgetEnabled(true);
}

void DataWidgetMapper::updateMappedWidget(MappedWidget * const mappedWidget, int column)
{
  Q_ASSERT(mappedWidget != nullptr);

  mUpdatingMappedWidget = true;
  QModelIndex index;
  if(!mModel.isNull()){
    index = mModel->index(mCurrentRow, column);
    mappedWidget->setWidgetValue(index.data());
  }
  /*
   * On invalid index, widget must allways be disabled.
   */
  if(index.isValid()){
    updateMappedWidgetForItemFlags(mappedWidget, mModel->flags(index));
    updateMappedWidgetForAppearance(mappedWidget->widget(), index);
  }else{
    mappedWidget->setWidgetEnabled(false);
  }
  mUpdatingMappedWidget = false;
}

void DataWidgetMapper::updateMappedWidgetFromModelFlags(MappedWidget * const mappedWidget, int column)
{
  Q_ASSERT(mappedWidget != nullptr);

  QModelIndex index;
  if(!mModel.isNull()){
    index = mModel->index(mCurrentRow, column);
  }
  /*
   * On invalid index, widget must allways be disabled.
   */
  if(index.isValid()){
    Q_ASSERT(!mModel.isNull());
    updateMappedWidgetForItemFlags(mappedWidget, mModel->flags(index));
  }else{
    mappedWidget->setWidgetEnabled(false);
  }
}

void DataWidgetMapper::updateMappedWidgetForItemFlags(MappedWidget * const mappedWidget, Qt::ItemFlags flags)
{
  Q_ASSERT(mappedWidget != nullptr);

  /*
   * If Qt::ItemIsEnabled is not set, widget must allways be disabled.
   * Else, follow Qt::ItemIsEditable if editable property exists for widget
   */

  const bool enabled = flags.testFlag(Qt::ItemIsEnabled);
  const bool editable = flags.testFlag(Qt::ItemIsEditable);

  mappedWidget->setWidgetEnabled(enabled);
  mappedWidget->setWidgetEditable(editable);
}

void DataWidgetMapper::updateMappedWidgetForAppearance(QWidget*const widget, const QModelIndex& index)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(index.isValid());
  Q_ASSERT(!mModel.isNull());

  WidgetStyleSheet ws;
  QVariant var;

  // Text alignment
  var = mModel->data(index, Qt::TextAlignmentRole);
  if(variantIsOfType(var, QMetaType::Int)){
    widget->setProperty("alignment", var);
  }else{
    widget->setProperty("alignment", QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter)));
  }
  // Text font
  var = mModel->data(index, Qt::FontRole);
  if(variantIsOfType(var, QMetaType::QFont)){
    ws.setTextFontVariant(var);
  }
  // Foreground brush
  var = mModel->data(index, Qt::ForegroundRole);
  if(variantIsOfType(var, QMetaType::QBrush)){
    ws.setForegroundBrushVariant(var);
  }
  // Background brush
  var = mModel->data(index, Qt::BackgroundRole);
  if(variantIsOfType(var, QMetaType::QBrush)){
    ws.setBackgroundBrushVariant(var);
  }
  /*
   * NOTE Should compare with previous ws before setting
   *      (could be a performance improuvement)
   */
  widget->setStyleSheet(ws.toCssString());
}

void DataWidgetMapper::updateAllMappedWidgets()
{
  for(const auto & mw : mMappedWidgetList){
    updateMappedWidget(mw.get(), mw->column());
  }
}

bool DataWidgetMapper::commitData(MappedWidget * const mappedWidget, int column)
{
  Q_ASSERT(mappedWidget != nullptr);
  Q_ASSERT(!mModel.isNull());

  auto index = mModel->index(mCurrentRow, column);
  if(index.isValid()){
    const bool ok = mModel->setData(index, mappedWidget->widgetValue());
    if(!ok){
      return false;
    }
  }

  return true;
}

bool DataWidgetMapper::commitAllMappedWidgetsData()
{
  for(const auto & mw : mMappedWidgetList){
    if( !commitData(mw.get(), mw->column()) ){
      return false;
    }
  }
  return true;
}

bool DataWidgetMapper::variantIsOfType(const QVariant& var, QMetaType::Type type)
{
  return static_cast<QMetaType::Type>(var.type()) == type;
}

}} // namespace Mdt{ namespace ItemEditor{
