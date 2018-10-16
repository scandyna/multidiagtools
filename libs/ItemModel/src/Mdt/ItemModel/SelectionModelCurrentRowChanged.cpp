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
#include "SelectionModelCurrentRowChanged.h"
#include <QItemSelectionModel>
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemModel{

SelectionModelCurrentRowChanged::SelectionModelCurrentRowChanged(QObject* parent)
 : QObject(parent)
{
}

void SelectionModelCurrentRowChanged::setSelectionModel(QItemSelectionModel* selectionModel)
{
  Q_ASSERT(selectionModel != nullptr);

  disconnect(mCurrentRowChangedConnection);

  mCurrentRowChangedConnection =
    connect(selectionModel, &QItemSelectionModel::currentRowChanged, this, &SelectionModelCurrentRowChanged::setCurrentRow);

  emit currentRowChanged( selectionModel->currentIndex().row() );
}

void SelectionModelCurrentRowChanged::setCurrentRow(const QModelIndex& current, const QModelIndex& /*previous*/)
{
  emit currentRowChanged( current.row() );
}

}} // namespace Mdt{ namespace ItemModel{
