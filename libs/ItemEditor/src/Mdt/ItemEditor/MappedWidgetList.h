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
#ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_LIST_H
#define MDT_ITEM_EDITOR_MAPPED_WIDGET_LIST_H

#include "MappedWidget.h"
#include <QPointer>
#include <vector>

namespace Mdt{ namespace ItemEditor{

  /*! \brief MappedWidgetList is used by DataWidgetMapper
   */
  class MappedWidgetList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef std::vector<MappedWidget>::const_iterator const_iterator;

    /*! \brief Add widget
     *
     * \note widget will not be owned by MappedWidgetList
     *        (it will not be deleted)
     */
    void addWidget(QWidget *widget, int column);

    /*! \brief Get count of mapped widgets
     */
    int size() const
    {
      return pvWidgetList.size();
    }

    /*! \brief Check if some widget is mapped
     */
    bool isEmpty() const
    {
      return pvWidgetList.empty();
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return pvWidgetList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return pvWidgetList.cend();
    }

    /*! \brief Remove all mapped widgets
     */
    void clear();

   private:

    std::vector<MappedWidget> pvWidgetList;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_LIST_H
