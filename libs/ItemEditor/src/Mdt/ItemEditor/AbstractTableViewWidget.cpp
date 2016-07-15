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
#include "AbstractTableViewWidget.h"
#include "TableViewController.h"
// #include "EventCatchItemDelegate.h"
// #include "ItemSelectionModel.h"
#include <QTableView>
#include <QVBoxLayout>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractTableViewWidget::AbstractTableViewWidget(QWidget* parent)
 : AbstractEditorWidget(parent),
   pvView(new QTableView)
{
  // Layout widgets
  auto *l = new QVBoxLayout;
  l->addWidget(pvView);
  setLayout(l);
}

void AbstractTableViewWidget::setController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);

  auto tableViewController = dynamic_cast<TableViewController*>(controller);
  Q_ASSERT(tableViewController != nullptr);
  tableViewController->setView(pvView);
  AbstractEditorWidget::setController(controller);
}

}} // namespace Mdt{ namespace ItemEditor{
