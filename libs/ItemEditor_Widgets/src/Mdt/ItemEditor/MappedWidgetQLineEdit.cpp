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
#include "MappedWidgetQLineEdit.h"

namespace Mdt{ namespace ItemEditor{

MappedWidgetQLineEdit::MappedWidgetQLineEdit(QLineEdit* edit, int column)
 : MappedWidget(edit, column)
{
  Q_ASSERT(edit != nullptr);

  connect(edit, &QLineEdit::textEdited, this, &MappedWidgetQLineEdit::editionStarted);
}

void MappedWidgetQLineEdit::setWidgetValue(const QVariant& value)
{
  lineEdtit()->setText( value.toString() );
}

QVariant MappedWidgetQLineEdit::widgetValue() const
{
  return lineEdtit()->text();
}

void MappedWidgetQLineEdit::setWidgetEditable(bool editable)
{
  lineEdtit()->setReadOnly(!editable);
}

QLineEdit* MappedWidgetQLineEdit::lineEdtit() const
{
  Q_ASSERT(qobject_cast<QLineEdit*>(widget()) != nullptr);

  return reinterpret_cast<QLineEdit*>( widget() );
}

}} // namespace Mdt{ namespace ItemEditor{
