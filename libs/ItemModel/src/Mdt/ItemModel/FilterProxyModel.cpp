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
#include "FilterProxyModel.h"
#include <QModelIndex>

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

FilterProxyModel::FilterProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
}

bool FilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  if(source_parent.isValid()){
//     auto error = mdtErrorNewQ(tr("Only list and table models/views are supported. Filter can return wrong results with a hierarchical model."), Mdt::Error::Warning, this );
//     error.commit();
    return false;
  }
  if(mFilterExpression.isNull()){
    return true;
  }
  return mFilterExpression.eval(sourceModel(), source_row, filterCaseSensitivity());
}

}} // namespace Mdt{ namespace ItemModel{
