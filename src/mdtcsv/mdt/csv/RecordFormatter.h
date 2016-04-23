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
#ifndef MDT_CSV_RECORD_FORMATTER_H
#define MDT_CSV_RECORD_FORMATTER_H

#include "RecordFormat.h"
#include "mdtError.h"
#include <QVariant>

namespace mdt{ namespace csv{

  /*! \brief Format record
   */
  class RecordFormatter
  {
   public:

    /*! \brief Set field count
     *
     * Will also assign given type to each field.
     *
     * \todo Should be called resetFormat() ?
     */
    void setFieldCount(int fieldCount, QMetaType::Type type);

    /*! \brief Get field count
     */
    int fieldCount() const
    {
      return pvFormat.fieldCount();
    }

    /*! \brief Set field type
     *
     * \pre fieldIndex must be in valid range
     */
    void setFieldType(int fieldIndex, QMetaType::Type type);

    /*! \brief Get field type
     *
     * \pre fieldIndex must be in valid range
     */
    QMetaType::Type fieldType(int fieldIndex) const
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < pvFormat.fieldCount());
      return pvFormat.fieldType(fieldIndex);
    }

    /*! \brief Get record format
     */
    RecordFormat format() const
    {
      return pvFormat;
    }

    /*! \brief Format value with type defined at fieldIndex
     *
     * \pre fieldIndex must be in valid range
     */
    bool formatValue(int fieldIndex, QVariant & value) const;

    /*! \brief Get last error
     */
    mdtError lastError() const
    {
      return pvLastError;
    }

   private:

    /*! \brief Convert value to given type
     */
    bool convert(QVariant & value, QMetaType::Type type) const;

    /*! \brief Translate (calls QObject::tr() )
     */
    QString tr(const char *sourceText) const;

    RecordFormat pvFormat;
    mutable mdtError pvLastError;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_RECORD_FORMATTER_H

