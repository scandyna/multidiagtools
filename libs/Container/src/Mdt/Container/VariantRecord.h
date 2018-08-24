/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_CONTAINER_VARIANT_RECORD_H
#define MDT_CONTAINER_VARIANT_RECORD_H

#include "MdtContainerExport.h"
#include <QVariant>
#include <QMetaType>
#include <vector>

namespace Mdt{ namespace Container{

  /*! \brief Record using QVariant as column data
   */
  class MDT_CONTAINER_EXPORT VariantRecord
  {
   public:

    /*! \brief Construct a empty record
     */
    VariantRecord() noexcept = default;

    /*! \brief Construct a record with \a count columns
     */
    explicit VariantRecord(int count)
     : mRecord(count)
    {
    }

    /*! \brief Copy construct a record from \a other
     */
    VariantRecord(const VariantRecord & other) = default;

    /*! \brief Copy assign \a other to this record
     */
    VariantRecord & operator=(const VariantRecord & other) = default;

    /*! \brief Move construct a record from \a other
     */
    VariantRecord(VariantRecord && other) = default;

    /*! \brief Move assign \a other to this record
     */
    VariantRecord & operator=(VariantRecord && other) = default;

    /*! \brief Get count of columns
     */
    int columnCount() const noexcept
    {
      return mRecord.size();
    }

    /*! \brief Get \a value at \a column
     *
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    QVariant value(int column) const
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      return mRecord[column];
    }

    /*! \brief Set \a value at \a column
     *
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setValue(int column, const QVariant & value)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      mRecord[column] = value;
    }

   private:

    std::vector<QVariant> mRecord;
  };

}} // namespace Mdt{ namespace Container{
Q_DECLARE_METATYPE(Mdt::Container::VariantRecord)

#endif // #ifndef MDT_CONTAINER_VARIANT_RECORD_H
