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
#include "ParserFormatSetupProxyModel.h"

#include <QDebug>

namespace mdt{ namespace csv{

ParserFormatSetupProxyModel::ParserFormatSetupProxyModel(QObject *parent)
 : QAbstractProxyModel(parent)
{
}


// int ParserFormatSetupModel::rowCount(const QModelIndex & parent) const
// {
//   Q_ASSERT(sourceModel() != nullptr);
//   qDebug() << "Proxy::rowCount() ..";
//   return sourceModel()->rowCount(parent);
// }
// 
// 
// QVariant ParserFormatSetupModel::headerData(int section, Qt::Orientation orientation, int role) const
// {
//   Q_ASSERT(sourceModel() != nullptr);
//   return sourceModel()->headerData(section, orientation, role);
// }
// 
// QVariant ParserFormatSetupModel::data(const QModelIndex & index, int role) const
// {
//   Q_ASSERT(sourceModel() != nullptr);
//   return sourceModel()->data(index, role);
// }

}} // namespace mdt{ namespace csv{