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
#include "SelectFieldListSqlTransform.h"
#include "SelectFieldList.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>

namespace Mdt{ namespace Sql{

QString SelectFieldListSqlTransform::getSql(const SelectFieldList & fieldList, const QSqlDatabase & db)
{

}


}} // namespace Mdt{ namespace Sql{
