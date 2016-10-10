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
#include "JoinConstraintExpressionTest.h"
#include "Mdt/Application.h"
#include <QDebug>
#include <iostream>
#include <vector>
#include <boost/proto/proto.hpp>
#include <boost/typeof/std/ostream.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/core/demangle.hpp>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <QString>
#include <QVariant>
#include <string>
#include <tuple>

namespace proto = boost::proto;
namespace mpl = boost::mpl;


class TableName
{
 public:

  explicit TableName(const QString & n)
   : pvName(n) {}

  QString toString() const
  {
    return pvName;
  }

 private:

  QString pvName;
};

class FieldName
{
 public:

  explicit FieldName(const QString & n)
   : pvName(n) {}

  QString toString() const
  {
    return pvName;
  }

 private:

  QString pvName;
};



struct TableFieldM
{
  QString tableName;
  QString fieldName;
};

struct TableFieldJ
{
  QString tableName;
  QString fieldName;
};

using MainTableExpressionField = const proto::terminal<TableFieldM>::type;
using TableToJoinExpressionField = const proto::terminal<TableFieldJ>::type;

MainTableExpressionField getMainTableExpressionField(const TableName & tableName, const FieldName & fieldName)
{
  return {{tableName.toString(), fieldName.toString()}};
}

TableToJoinExpressionField getTableToJoinExpressionField(const TableName & tableName, const FieldName & fieldName)
{
  return {{tableName.toString(), fieldName.toString()}};
}

void JoinConstraintExpressionTest::sandbox()
{
  auto clientId = getMainTableExpressionField(TableName("Client_tbl"), FieldName("Id_PK"));
  auto AddressId = getTableToJoinExpressionField(TableName("Address_tbl"), FieldName("Client_Id_FK"));

  std::cout << "Table: " << proto::value(clientId).tableName.toStdString() << " , field: " << proto::value(clientId).fieldName.toStdString() << std::endl;
  std::cout << "Table: " << proto::value(AddressId).tableName.toStdString() << " , field: " << proto::value(AddressId).fieldName.toStdString() << std::endl;

  proto::display_expr(clientId == AddressId);
}

/*
 * Voir:
 * Name == 25 <- Ok
 * 25 == Name <- Nok
 * Name == 25 && Name < 15  <- Ok
 * (Name == 25) && Name < 15  <- Ok
 * Name == (25 && Name) < 15  <- Nok
 * Name == Remarks <- Nok
 * ....
 */

void JoinConstraintExpressionTest::initTestCase()
{
}

void JoinConstraintExpressionTest::cleanupTestCase()
{
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  JoinConstraintExpressionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
