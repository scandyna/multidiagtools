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
#ifndef MDT_SQL_SCHEMA_VIEW_LIST_H
#define MDT_SQL_SCHEMA_VIEW_LIST_H

#include "View.h"
#include <QVector>

namespace Mdt{ namespace Sql{ namespace Schema{

  template<typename T>
  class ViewTemplate;

  /*! \brief Holds a list of View
   */
  class ViewList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef QVector<View>::const_iterator const_iterator;

    /*! \brief Append a view
     */
    void append(const View & v)
    {
      pvViewList.append(v);
    }

    /*! \brief Append a view
     */
    template<typename T>
    void append(const ViewTemplate<T> & v)
    {
      pvViewList.append(v.toView());
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvViewList.size();
    }

    /*! \brief Check if view list is empty
     */
    bool isEmpty() const
    {
      return pvViewList.isEmpty();
    }

    /*! \brief Get view at given index
     *
     * \pre index must be valid
     */
    const View & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvViewList.at(index);
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return pvViewList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return pvViewList.cend();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvViewList.clear();
    }

   private:

    QVector<View> pvViewList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_VIEW_LIST_H
