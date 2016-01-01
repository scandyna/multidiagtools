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
#ifndef MDT_SQL_COPIER_SOURCE_FIELD_INDEX_H
#define MDT_SQL_COPIER_SOURCE_FIELD_INDEX_H

#include "AbstractSourceField.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Simple field index source field
   */
  class SourceFieldIndex : public AbstractSourceField
  {
   public:

    /*! \brief Construct a invalid field index
     */
    SourceFieldIndex()
     : AbstractSourceField(),
       pvFieldIndex(-1)
    {
    }

    ~SourceFieldIndex()
    {
    }

    /*! \brief Polymorphic copy
     */
    SourceFieldIndex* clone() const override
    {
      return new SourceFieldIndex(pvFieldIndex);
    }

    /*! \brief Check if source field is null
     */
    bool isNull() const override
    {
      return (pvFieldIndex < 0);
    }

    /*! \brief Clear
     */
    void clear() override
    {
      pvFieldIndex = -1;
    }

    /*! \brief Set source field index
     */
    void setFieldIndex(int index) override
    {
      pvFieldIndex = index;
    }

    /*! \brief Get source field index
     */
    virtual int fieldIndex() const override
    {
      return pvFieldIndex;
    }

   private:

    /*! \brief Construct a initalized index (used by clone())
     */
    SourceFieldIndex(int index)
     : AbstractSourceField(),
       pvFieldIndex(index)
    {
    }

    int pvFieldIndex;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_SOURCE_FIELD_INDEX_H

