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
#include "MappedWidget.h"
#include <QMetaObject>
#include <QMetaProperty>

namespace Mdt{ namespace ItemEditor{

MappedWidget::MappedWidget(QWidget* widget, int column)
 : QObject(),
   mWidget(widget), mColumn(column)
{
  Q_ASSERT(mWidget != nullptr);
  Q_ASSERT(column >= 0);
}

void MappedWidget::setWidgetEnabled(bool enable)
{
  mWidget->setEnabled(enable);
}

}} // namespace Mdt{ namespace ItemEditor{
