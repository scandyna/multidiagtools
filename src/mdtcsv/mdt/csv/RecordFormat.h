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
#ifndef MDT_CSV_RECORD_FORMAT_H
#define MDT_CSV_RECORD_FORMAT_H

#include <QVariant>
#include <QMetaType>
#include <QVector>

namespace mdt{ namespace csv{

  /*! \brief CSV record format
   *
   * This value class contains CSV record format.
   */
  class RecordFormat
  {
   public:

    /*! \brief Set field count
     *
     * Will also assign given type to each field.
     *
     * \todo Should be called reset() ?
     */
    void setFieldCount(int fieldCount, QMetaType::Type type)
    {
      pvFieldTypes.clear();
      pvFieldTypes.fill(type, fieldCount);
    }

    /*! \brief Get field count
     */
    int fieldCount() const
    {
      return pvFieldTypes.size();
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


   private:

    QVector<QMetaType::Type> pvFieldTypes;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_RECORD_FORMAT_H

