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
#include "SourceField.h"
#include "AbstractSourceField.h"
#include "SourceFieldIndex.h"
#include "SourceFixedValue.h"

namespace mdt{ namespace sql{ namespace copier{

SourceField::SourceField(SourceField::Type type)
 : pvType(type)
{
  constructShared();
}

// Must be implemented here because AbstractSourceField is not defined in header
SourceField::SourceField(const SourceField& other)
 : pvType(other.pvType),
   pvShared(other.pvShared)
{
}

// Must be implemented here because AbstractSourceField is not defined in header
SourceField::~SourceField()
{
}

// Must be implemented here because AbstractSourceField is not defined in header
SourceField& SourceField::operator=(const SourceField & other)
{
  if(&other != this){
    pvType = other.pvType;
    pvShared = other.pvShared;
  }
  return *this;
}

bool SourceField::isNull() const
{
  Q_ASSERT(pvShared);
  return pvShared->isNull();
}

void SourceField::clear()
{
  Q_ASSERT(pvShared);
  pvShared->clear();
}

void SourceField::setFieldIndex(int index)
{
  if(pvType != SourceFieldIndexType){
    reset(SourceFieldIndexType);
  }
  Q_ASSERT(pvShared);
  pvShared->setFieldIndex(index);
}

int SourceField::fieldIndex() const
{
  Q_ASSERT(pvShared);
  return pvShared->fieldIndex();
}

void SourceField::setFixedValue(const QVariant& value)
{
  if(pvType != SourceFixedValueType){
    reset(SourceFixedValueType);
  }
  Q_ASSERT(pvShared);
  pvShared->setFixedValue(value);
}

QVariant SourceField::fixedValue() const
{
  Q_ASSERT(pvShared);
  return pvShared->fixedValue();
}

void SourceField::reset(SourceField::Type type)
{
  Q_ASSERT(type != pvType);

  pvType = type;
  if(pvShared){
    pvShared.~QSharedDataPointer();
  }
  constructShared();
}

void SourceField::constructShared()
{
  switch(pvType){
    case SourceFieldIndexType:
      pvShared = new SourceFieldIndex;
      break;
    case SourceFixedValueType:
      pvShared = new SourceFixedValue;
      break;
  }
}

}}} // namespace mdt{ namespace sql{ namespace copier{

/*
 * Clone template specialization:
 * We have a QSharedDataPointer<AbstractSourceField>,
 * but we must return a new object using the right clone() function.
 */
template <>
mdt::sql::copier::AbstractSourceField *QSharedDataPointer<mdt::sql::copier::AbstractSourceField>::clone()
{
  return d->clone();
}

