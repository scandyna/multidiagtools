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
#include "MappedWidgetQComboBox.h"
#include <QLineEdit>

namespace Mdt{ namespace ItemEditor{

MappedWidgetQComboBox::MappedWidgetQComboBox(QComboBox* comboBox, int column)
 : MappedWidget(comboBox, column)
{
  Q_ASSERT(comboBox != nullptr);

  mEnabled = comboBox->isEnabled();
  connect(comboBox, QOverload<int>::of(&QComboBox::activated), this, &MappedWidgetQComboBox::editionStarted);
  auto *lineEdit = comboBox->lineEdit();
  if(lineEdit != nullptr){
    connect(lineEdit, &QLineEdit::textEdited, this, &MappedWidgetQComboBox::editionStarted);
  }
}

void MappedWidgetQComboBox::setWidgetValue(const QVariant& value)
{
  comboBox()->setCurrentText( value.toString() );
}

QVariant MappedWidgetQComboBox::widgetValue() const
{
  return comboBox()->currentText();
}

void MappedWidgetQComboBox::setWidgetEditable(bool editable)
{
  mEditable = editable;
  comboBox()->setEnabled( canEnableWidget() );
}

void MappedWidgetQComboBox::setWidgetEnabled(bool enable)
{
  mEnabled = enable;
  comboBox()->setEnabled( canEnableWidget() );
}

QComboBox* MappedWidgetQComboBox::comboBox() const
{
  Q_ASSERT(qobject_cast<QComboBox*>( widget() ) != nullptr);

  return reinterpret_cast<QComboBox*>( widget() );
}

bool MappedWidgetQComboBox::canEnableWidget() const
{
  return (mEditable && mEnabled);
}

}} // namespace Mdt{ namespace ItemEditor{
