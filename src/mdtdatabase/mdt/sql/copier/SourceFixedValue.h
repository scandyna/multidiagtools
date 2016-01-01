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
#ifndef MDT_SQL_COPIER_SOURCE_FIXED_VALUE_H
#define MDT_SQL_COPIER_SOURCE_FIXED_VALUE_H

#include "AbstractSourceField.h"
#include <QVariant>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Fixed value source
   */
  class SourceFixedValue : public AbstractSourceField
  {
   public:

    /*! \brief Construct a invalid value
     */
    SourceFixedValue()
     : AbstractSourceField()
    {
    }

    ~SourceFixedValue()
    {
    }

    /*! \brief Polymorphic copy
     */
    SourceFixedValue* clone() const override
    {
      return new SourceFixedValue(pvValue);
    }

    /*! \brief Check if value is null
     */
    bool isNull() const override
    {
      return pvValue.isNull();
    }

    /*! \brief Clear
     */
    void clear() override
    {
      pvValue.clear();
    }

    /*! \brief Set fixed value
     */
    void setFixedValue(const QVariant & value) override
    {
      pvValue = value;
    }

    /*! \brief Set fixed value
     */
    QVariant fixedValue() const override
    {
      return pvValue;
    }

   private:

    /*! \brief Construct a initalized value (used by clone())
     */
    SourceFixedValue(const QVariant & value)
     : AbstractSourceField(),
       pvValue(value)
    {
    }

    QVariant pvValue;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_SOURCE_FIXED_VALUE_H

