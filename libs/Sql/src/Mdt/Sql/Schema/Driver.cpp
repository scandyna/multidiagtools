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
#include "Driver.h"
#include "DriverImplementationInterface.h"
#include "DriverSQLite.h"
#include "DriverMySql.h"
#include <QObject>

namespace Mdt{ namespace Sql{ namespace Schema{

Driver::Driver(const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.driver() != nullptr);

  auto type = typeFromDbmsType(db.driver()->dbmsType());
  switch(type){
    case DriverType::SQLite:
      pvImpl = std::make_unique<DriverSQLite>(db);
      break;
    case DriverType::MySQL:
      pvImpl = std::make_unique<DriverMySql>(db);
      break;
    case DriverType::MariaDB:
    case DriverType::Unknown:
      break;
  }
  if(!pvImpl){
    QString msg = tr("Database engine with Qt driver name '") + db.driverName() + tr("' is not supported. ") \
                  + tr("Database connection name: '") + db.connectionName() + tr("'.");
    pvLastError = mdtErrorNew(msg, Mdt::Error::Critical, "Mdt::Sql::Schema::Driver");
    pvLastError.commit();
  }
}

Driver::~Driver()
{
}

DriverType Driver::type() const
{
  if(!pvImpl){
    return DriverType::Unknown;
  }
  return pvImpl->type();
}

FieldTypeList Driver::getAvailableFieldTypeList() const
{
  Q_ASSERT(isValid());
  return pvImpl->getAvailableFieldTypeList();
}

FieldTypeInfoList Driver::getAvailableFieldTypeInfoList() const
{
  Q_ASSERT(isValid());
  ///return pvImpl->getAvailableFieldTypeInfoList();
}

FieldType Driver::fieldTypeFromQMetaType(QMetaType::Type qmt) const
{
  Q_ASSERT(isValid());
  return pvImpl->fieldTypeFromQMetaType(qmt);
}

QMetaType::Type Driver::fieldTypeToQMetaType(FieldType ft) const
{
  Q_ASSERT(isValid());
  return pvImpl->fieldTypeToQMetaType(ft);
}

QString Driver::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{