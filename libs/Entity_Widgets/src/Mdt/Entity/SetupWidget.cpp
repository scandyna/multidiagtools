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
#include "SetupWidget.h"
#include "Mdt/Entity/FieldAttributes.h"
#include <QLineEdit>
#include <QComboBox>
#include <QMetaObject>

namespace Mdt{ namespace Entity{

void setupWidget(QLineEdit*const lineEdit, const FieldAttributes& fieldAttributes)
{
  Q_ASSERT(lineEdit != nullptr);

  const int maxLength = fieldAttributes.maxLength();
  if(maxLength > 0){
    lineEdit->setMaxLength(maxLength);
  }else{
    lineEdit->setMaxLength(32767);
  }
}

void setupWidget(QComboBox*const comboBox, const FieldAttributes& fieldAttributes)
{
  Q_ASSERT(comboBox != nullptr);

  if(comboBox->lineEdit() != nullptr){
    setupWidget(comboBox->lineEdit(), fieldAttributes);
  }
}

void setupWidget(QWidget*const widget, const FieldAttributes & fieldAttributes)
{
  Q_ASSERT(widget != nullptr);

  const int maxLength = fieldAttributes.maxLength();
  if(maxLength > 0){
    setWidgetProperty(widget, "maxLength", maxLength);
  }else{
    setWidgetProperty(widget, "maxLength", 32767);
  }
}

bool setWidgetProperty(QWidget*const widget, const char*const propertyName, const QVariant& propertyValue)
{
  Q_ASSERT(widget != nullptr);
  Q_ASSERT(widget->metaObject() != nullptr);
  Q_ASSERT(propertyName != nullptr);

  if(widget->metaObject()->indexOfProperty(propertyName) < 0){
    return false;
  }

  return widget->setProperty(propertyName, propertyValue);
}

}} // namespace Mdt{ namespace Entity{
