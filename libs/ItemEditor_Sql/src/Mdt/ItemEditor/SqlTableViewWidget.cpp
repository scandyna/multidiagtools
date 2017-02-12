/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "SqlTableViewWidget.h"
#include <QTableView>

namespace Mdt{ namespace ItemEditor{

SqlTableViewWidget::SqlTableViewWidget(QWidget* parent, const QSqlDatabase & db)
 : AbstractTableViewWidget(parent),
   mController(new SqlTableViewController(this))
{
  mController->setDefaultModel(db);
  mController->setView(view());
}

SqlTableViewWidget::SqlTableViewWidget(const QSqlDatabase & db)
 : SqlTableViewWidget(nullptr, db)
{
}

void SqlTableViewWidget::setModel(QSqlTableModel* model)
{
  Q_ASSERT(model != nullptr);

  mController->setModel(model);
}

}} // namespace Mdt{ namespace ItemEditor{
