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
#include "RecordFormatSetupWidgetItem.h"
#include "FieldType.h"
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <algorithm>

#include <QDebug>

namespace mdt{ namespace csv{

RecordFormatSetupWidgetItem::RecordFormatSetupWidgetItem(QWidget *parent)
 : QWidget(parent),
   lbFieldName(new QLabel),
   cbFieldType(new QComboBox)
{
  // Setup UI
  lbFieldName->setAlignment(Qt::AlignHCenter);
  auto *l = new QVBoxLayout;
  l->addWidget(lbFieldName);
  l->addWidget(cbFieldType);
  setLayout(l);
  // Populate type combobox
  auto ftList = FieldType::getAvailableFieldTypeList();
  std::sort(ftList.begin(), ftList.end(), [](const FieldType & a, const FieldType & b){ return a.name() < b.name(); } );
  for(const auto & ft : ftList){
    cbFieldType->addItem(ft.name(), static_cast<int>(ft.type()) );
  }
  connect(cbFieldType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &RecordFormatSetupWidgetItem::onFieldTypeChanged);
}

void RecordFormatSetupWidgetItem::setFieldName(const QString & name)
{
  lbFieldName->setText(name);
}

void RecordFormatSetupWidgetItem::setFieldType(QMetaType::Type type)
{
  int index = cbFieldType->findData(static_cast<int>(type));
  cbFieldType->setCurrentIndex(index);
}

QMetaType::Type RecordFormatSetupWidgetItem::fieldType() const
{
  return static_cast<QMetaType::Type>(cbFieldType->currentData().toInt());
}

void RecordFormatSetupWidgetItem::onFieldTypeChanged()
{
//   qDebug() << "Field " << lbFieldName->text() << ": type changed, signal..";
  emit fieldTypeChanged();
}

}} // namespace mdt{ namespace csv{
