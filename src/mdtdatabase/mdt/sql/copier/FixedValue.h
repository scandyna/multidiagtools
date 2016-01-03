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
#ifndef MDT_SQL_COPIER_FIXED_VALUE_H
#define MDT_SQL_COPIER_FIXED_VALUE_H

#include "AbstractTableMappingItem.h"
#include <QVariant>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Fixed value to copy to destination for SQL copier
   */
  class FixedValue : public AbstractTableMappingItem
  {
   public:

    /*! \brief Create a invalid fixed value
     */
    FixedValue()
     : AbstractTableMappingItem()
    {
      qDebug() << "C  FixedValue::FixedValue() - ref: " << ref.load();
    }

    /*! \brief Destructor
     */
    ~FixedValue()
    {
      qDebug() << "D  FixedValue::~FixedValue() - ref: " << ref.load();
    }

    /*! \brief Polymorphic copy
     */
    FixedValue* clone() const override
    {
      qDebug() << " FixedValue::clone() - ref: " << ref.load();
      return new FixedValue(*this);
    }

    /*! \brief Check if fixed value is null
     *
     * Fixed value is null if value is null or no destination field was set
     */
    bool isNull() const override
    {
      return ( pvValue.isNull() || (destinationFieldIndexCount() < 1) );
    }

    /*! \brief Set fixed value
     */
    void setFixedValue(const QVariant & value, int destinationFieldIndex) override
    {
      pvValue = value;
      setDestinationFieldIndex(destinationFieldIndex);
    }

    /*! \brief Get fixed value
     */
    QVariant fixedValue() const override
    {
      return pvValue;
    }

   private:

    /*! \brief Clear fixed value
     */
    void clearItem() override
    {
      pvValue.clear();
    }

    /*! \brief Copy constructor (used by clone)
     */
    FixedValue(const FixedValue & other)
     : AbstractTableMappingItem(other)
    {
      qDebug() << "CPY  FixedValue::FixedValue(other) - ref: " << ref.load();
    }

    QVariant pvValue;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_FIXED_VALUE_H

