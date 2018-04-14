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
#ifndef MDT_SQL_SCHEMA_INDEX_LIST_H
#define MDT_SQL_SCHEMA_INDEX_LIST_H

#include "Index.h"
#include "Mdt/Sql/FieldName.h"
#include "Mdt/Sql/TableName.h"
#include <QVector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief A list of Index
   */
  class IndexList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef QVector<Index>::const_iterator const_iterator;

    /*! \brief Append a index
     */
    void append(const Index & index)
    {
      pvIndexList.append(index);
    }

    /*! \brief Update table name for each index in this list
     */
    void updateTableName(const QString & tableName)
    {
      for(auto & index : pvIndexList){
        index.setTableName(tableName);
      }
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvIndexList.size();
    }

    /*! \brief Get index at index i
     *
     * \pre i must be in valid range
     */
    const Index & at(int i) const
    {
      Q_ASSERT(i >= 0);
      Q_ASSERT(i < size());

      return pvIndexList.at(i);
    }

    /*! \brief Find index for tableName and fieldName
     *
     * If no index matches criteria,
     *  an null Index is returned
     */
    Index findIndex(const TableName & tableName, const FieldName & fieldName) const
    {
      Index idx;

      for(const auto & _idx : pvIndexList){
        if( QString::compare(_idx.tableName(), tableName.toString(), Qt::CaseInsensitive) == 0 ){
          if(_idx.containsFieldName(fieldName.toString())){
            idx = _idx;
            break;
          }
        }
      }

      return idx;
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return pvIndexList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return pvIndexList.cend();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvIndexList.clear();
    }

   private:

    QVector<Index> pvIndexList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_INDEX_LIST_H
