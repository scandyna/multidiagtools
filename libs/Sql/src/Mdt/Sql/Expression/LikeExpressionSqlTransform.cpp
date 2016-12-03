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
#include "LikeExpressionSqlTransform.h"
#include "Mdt/FilterExpression/LikeExpression.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QSqlField>
#include <QVariant>

#include <QDebug>

namespace Mdt{ namespace Sql{ namespace Expression{

QString LikeExpressionSqlTransform::getSql(const Mdt::FilterExpression::LikeExpression & expr, const QSqlDatabase & db)
{
  QString sql;
  const auto *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  QSqlField field("", QVariant::String);
  field.setValue(expr.expression());

  qDebug() << "expr: " << expr.expression() << " , formatted: " << driver->formatValue(field);

  return sql;
}

void LikeExpressionSqlTransform::escapeMetacharacters(QString & str)
{

}

}}} // namespace Mdt{ namespace Sql{ namespace Expression{
