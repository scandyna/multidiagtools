/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "MappedWidgetRelationalQComboBox.h"
#include <QComboBox>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

namespace Mdt{ namespace ItemEditor{

MappedWidgetRelationalQComboBox::MappedWidgetRelationalQComboBox(QComboBox *comboBox, int column, int comboBoxModelValueColumn)
 : MappedWidgetQComboBox(comboBox, column),
   mComboBoxModelValueColumn(comboBoxModelValueColumn)
{
  Q_ASSERT(comboBox != nullptr);
  Q_ASSERT(comboBox->model() != nullptr);
  Q_ASSERT(comboBoxModelValueColumn >= 0);
  Q_ASSERT(comboBoxModelValueColumn < comboBox->model()->columnCount());
}

void MappedWidgetRelationalQComboBox::setWidgetValue(const QVariant & value)
{
  if(value.isNull()){
    comboBox()->setCurrentIndex(-1);
    return;
  }

  const int row = findRowForValue(value);

  comboBox()->setCurrentIndex(row);
}

QVariant MappedWidgetRelationalQComboBox::widgetValue() const
{
  const int row = comboBox()->currentIndex();
  if(row < 0){
    return QVariant();
  }

  const auto *model = comboBox()->model();
  Q_ASSERT(model != nullptr);
  const auto index = model->index(row, mComboBoxModelValueColumn);
  Q_ASSERT(index.isValid());

  return model->data(index);
}

int MappedWidgetRelationalQComboBox::findRowForValue(const QVariant & value) const
{
  Q_ASSERT(!value.isNull());

  const auto *model = comboBox()->model();
  Q_ASSERT(model != nullptr);

  const int n = model->rowCount();
  const int column = mComboBoxModelValueColumn;
  for(int row = 0; row < n; ++row){
    const auto index = model->index(row, column);
    Q_ASSERT(index.isValid());
    if(model->data(index) == value){
      return row;
    }
  }

  return -1;
}

}} // namespace Mdt{ namespace ItemEditor{
