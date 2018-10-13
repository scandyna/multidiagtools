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
#ifndef MDT_QUERY_EXPRESSION_SELECT_FIELD_LIST_H
#define MDT_QUERY_EXPRESSION_SELECT_FIELD_LIST_H

#include "SelectField.h"
#include "SelectAllField.h"
#include "FieldName.h"
#include "EntityName.h"
#include "SelectEntity.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>
#include <vector>

namespace Mdt{ namespace QueryExpression{

  /*! \brief A list of fields in a SelectQuery
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectFieldList
  {
   public:

//     /*! \brief Add a field to this list
//      */
//     void addField(const SelectField & field);

    /*! \brief Add a select all field to this list
     */
    void addField(const SelectAllField & field);

    /*! \brief Add a field to this query
     *
     * \todo Preconditions ?
     */
    void addField(const SelectField & field);

    /*! \brief Add a field to this list
     *
     * \pre \a fieldName must not be null
     */
    void addField(const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Add a field to this list
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be null
     */
    void addField(const SelectEntity & entity, const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Get the index of a field
     *
     * If the requested field was found, -1 is returned.
     */
    int fieldIndex(const SelectField & field) const;

    /*! \brief Get count of fields in this list
     */
    int fieldCount() const noexcept
    {
      return mList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const noexcept
    {
      return mList.empty();
    }

    /*! \brief Get field at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < fieldCount() )
     */
    const SelectField & at(int index) const noexcept
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < fieldCount());

      return mList[index];
    }

    /*! \brief Clear this field list
     */
    void clear();

   private:

    std::vector<SelectField> mList;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_FIELD_LIST_H
