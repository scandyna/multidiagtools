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
#include "PrimaryKeyContainer.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

class FieldNameVisitor : public boost::static_visitor<QString>
{
 public:

  QString operator()(const AutoIncrementPrimaryKey & pk) const
  {
    return pk.fieldName();
  }

  QString operator()(const PrimaryKey &) const
  {
    return QString();
  }
};

class FieldNameListVisitor : public boost::static_visitor<QStringList>
{
 public:

  QStringList operator()(const AutoIncrementPrimaryKey & pk) const
  {
    return QStringList{pk.fieldName()};
  }

  QStringList operator()(const PrimaryKey & pk) const
  {
    return pk.fieldNameList();
  }

};

class FieldTypeVisitor : public boost::static_visitor<FieldType>
{
 public:

  FieldType operator()(const AutoIncrementPrimaryKey & pk) const
  {
    return pk.fieldType();
  }

  FieldType operator()(const PrimaryKey &) const
  {
    return FieldType::UnknownType;
  }
};

class FieldLengthVisitor : public boost::static_visitor<int>
{
 public:

  int operator()(const AutoIncrementPrimaryKey &) const
  {
    return -1;
  }

  int operator()(const PrimaryKey &) const
  {
    return -1;
  }
};

/*
 * PrimaryKeyContainer implementation
 */

QString PrimaryKeyContainer::fieldName() const
{
  return boost::apply_visitor( FieldNameVisitor() , mPrimaryKey );
}

QStringList PrimaryKeyContainer::fieldNameList() const
{
  return boost::apply_visitor( FieldNameListVisitor() , mPrimaryKey );
}

FieldType PrimaryKeyContainer::fieldType() const
{
  return boost::apply_visitor(FieldTypeVisitor(), mPrimaryKey);
}

int PrimaryKeyContainer::fieldLength() const
{
  return boost::apply_visitor( FieldLengthVisitor() , mPrimaryKey );
}

AutoIncrementPrimaryKey PrimaryKeyContainer::autoIncrementPrimaryKey() const
{
  Q_ASSERT(mType == AutoIncrementPrimaryKeyType);
  return boost::get<AutoIncrementPrimaryKey>(mPrimaryKey);
}

PrimaryKey PrimaryKeyContainer::primaryKey() const
{
  Q_ASSERT(mType == PrimaryKeyType);
  return boost::get<PrimaryKey>(mPrimaryKey);
}

void PrimaryKeyContainer::clear()
{
  mPrimaryKey = PrimaryKey();
  mType = PrimaryKeyType;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
