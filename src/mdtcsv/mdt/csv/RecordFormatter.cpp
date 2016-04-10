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
#include "RecordFormatter.h"
#include <QObject>

namespace mdt{ namespace csv{

void RecordFormatter::setFieldCount(int fieldCount, QMetaType::Type type)
{
  pvFormat.setFieldCount(fieldCount, type);
}

void RecordFormatter::setFieldType(int fieldIndex, QMetaType::Type type)
{
  Q_ASSERT(fieldIndex >= 0);
  Q_ASSERT(fieldIndex < pvFormat.fieldCount());

  pvFormat.setFieldType(fieldIndex, type);
}

bool RecordFormatter::formatValue(int fieldIndex, QVariant & value) const
{
  Q_ASSERT(fieldIndex >= 0);
  Q_ASSERT(fieldIndex < pvFormat.fieldCount());

  return convert(value, pvFormat.fieldType(fieldIndex));
}

bool RecordFormatter::convert(QVariant & value, QMetaType::Type type) const
{
  const auto valueCopy = value;

  if(value.type() == QVariant::String){
    /*
     * If string is empty, we want to convert to a null QVariant of requested type.
     * The best way to do this is using QVariant::convert() and ignore return value.
     * See Qt's QVariant documentation about this.
     */
    if(value.toString().isEmpty()){
      value.convert(type);
      return true;
    }
  }
  // Convert
  if(!value.convert(type)){
    QString msg = tr("Convertion of value '") + valueCopy.toString().left(50) \
                  + tr("' to type '") + QMetaType::typeName(type) \
                  + tr("' failed.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdt::csv::RecordFormatter");
    pvLastError.commit();
    return false;
  }

  return true;
}

QString RecordFormatter::tr(const char *sourceText) const
{
  return QObject::tr(sourceText);
}

}} // namespace mdt{ namespace csv{

