/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CSV_RECORD_FORMAT_H
#define MDT_CSV_RECORD_FORMAT_H

#include "mdtError.h"
#include "mdtCsvData.h"
#include <QVariant>
#include <QMetaType>
#include <QVector>

/*! \brief Format of a mdtCsvRecord
 */
class mdtCsvRecordFormat
{
 public:

  /*! \brief Constructor
   *
   * Create a mdtCsvRecordFormat for fieldCount fields.
   *  No format is assigned to the fields.
   */
  mdtCsvRecordFormat(int fieldCount)
   : pvFieldTypes(fieldCount, QMetaType::UnknownType)
  {
  }

  /*! \brief Get field count
   */
  int fieldCount() const
  {
    return pvFieldTypes.size();
  }

  /*! \brief Check if a type was set for given field
   *
   * \pre fieldIndex must be in valid range
   */
  bool hasType(int fieldIndex) const
  {
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < pvFieldTypes.size());
    return (pvFieldTypes.at(fieldIndex) != QMetaType::UnknownType);
  }

  /*! \brief Set field type
   *
   * \pre fieldIndex must be in valid range
   */
  void setFieldType(int fieldIndex, QMetaType::Type type)
  {
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < pvFieldTypes.size());
    pvFieldTypes[fieldIndex] = type;
  }

  /*! \brief Format given record
   *
   * \pre record must contain same count of fields than this format
   */
  bool formatRecord(mdtCsvRecord & record);

  /*! \brief Get field type
   *
   * \pre fieldIndex must be in valid range
   */
  QMetaType::Type fieldType(int fieldIndex) const
  {
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < pvFieldTypes.size());
    return pvFieldTypes.at(fieldIndex);
  }

  /*! \brief Clear formats
   *
   * Reset all fields to no format.
   */
  void clearFormats();

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Convert data to given type
   */
  bool convert(QVariant & data, QMetaType::Type type);

  /*! \brief Translate (calls QObject::tr() )
   */
  QString tr(const char *sourceText);

  QVector<QMetaType::Type> pvFieldTypes;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CSV_RECORD_FORMAT_H
