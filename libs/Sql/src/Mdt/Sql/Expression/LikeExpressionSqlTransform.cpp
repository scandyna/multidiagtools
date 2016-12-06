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
#include "Mdt/Algorithm.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QLatin1String>
#include <QSqlField>
#include <QVariant>

// #include <QDebug>

namespace Algorithm = Mdt::Algorithm;

namespace Mdt{ namespace Sql{ namespace Expression{

QString LikeExpressionSqlTransform::getSql(const QString & expr, const QSqlDatabase & db)
{
  QString sql;
  QString pattern = expr;
  const auto *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  QSqlField field("", QVariant::String);

  escapeSqlLikeMetacharacters(pattern);
  bool hasEscape = expr != pattern;
  pattern = Algorithm::replaceNonEscapedTokens(pattern, {{'?',"_"},{'*',"%"}}, '\\');
  pattern = Algorithm::unescapeEscapedTokens(pattern, {'?','*'}, '\\');
  field.setValue(pattern);
  sql = QLatin1String("LIKE ") % driver->formatValue(field);
  if(hasEscape){
    field.setValue("\\");
    sql += QLatin1String(" ESCAPE ") % driver->formatValue(field);
  }

  return sql;
}

void LikeExpressionSqlTransform::escapeSqlLikeMetacharacters(QString & str)
{
  str.replace('_', QLatin1String("\\_"));
  str.replace('%', QLatin1String("\\%"));
}

}}} // namespace Mdt{ namespace Sql{ namespace Expression{
