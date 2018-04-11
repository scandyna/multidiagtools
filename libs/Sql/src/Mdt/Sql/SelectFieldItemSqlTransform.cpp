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
#include "SelectFieldItemSqlTransform.h"
#include "SelectFieldItem.h"
#include "SelectFieldSqlTransform.h"
#include "SelectField.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

/*
 * SelectFieldItemSqlTransformVisitor
 */
class SelectFieldItemSqlTransformVisitor : boost::static_visitor<QString>
{
 public:

  SelectFieldItemSqlTransformVisitor(const QString & table, const QSqlDatabase & db)
   : mTable(table),
     mDatabase(db)
  {
    Q_ASSERT(mDatabase.isValid());
  }

  QString operator()(const SelectField & field) const
  {
    return getTableNameSql() % SelectFieldSqlTransform::getSql(field, mDatabase);
  }

  QString operator()(const SelectAllField &) const
  {
    return getTableNameSql() % QLatin1Char('*');
  }

  QString operator()(const SelectFieldRawSql & sql) const
  {
    return sql.sql;
  }

 private:

  QString getTableNameSql() const
  {
    QString sql;
    const auto *driver = mDatabase.driver();
    Q_ASSERT(driver != nullptr);

    if(!mTable.isEmpty()){
      sql = driver->escapeIdentifier(mTable, QSqlDriver::TableName) % QLatin1Char('.');
    }

    return sql;
  }

  const QString & mTable;
  const QSqlDatabase & mDatabase;
};

/*
 * SelectFieldItemSqlTransform implementation
 */

QString SelectFieldItemSqlTransform::getSql(const SelectFieldItem & item, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());
  return boost::apply_visitor( SelectFieldItemSqlTransformVisitor(item.tableName(), db) ,item.item() );
}

}} // namespace Mdt{ namespace Sql{
