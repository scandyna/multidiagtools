/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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

    /*! \brief STL-style iterator
     */
    typedef std::vector<MappedWidget>::iterator iterator;

    /*! \brief Add widget
     *
     * \note widget will not be owned by MappedWidgetList
     *        (it will not be deleted)
     * \pre \a column must be >= 0
     * \pre \a column must not allready exists in this mapping list
     * \pre \a widget must be a valid pointer
     * \pre \a widget must not allready exists in this mapping list
     */
    void addMapping(QWidget *widget, int column);

    /*! \brief Get index for given widget
     *
     * Returns the index of mapping for \a widget if it exists,
     *  otherwise -1 .
     *
     * \pre \a widget muts be a valid pointer
     */
    int getIndexForWidget(const QWidget * const widget) const;

    /*! \brief Get index for given column
     *
     * Returns the index of mapping for \a column if it exists,
     *  otherwise -1 .
     *
     * \pre \a column must be >= 0
     */
    int getIndexForColumn(int column) const;

    /*! \brief Get widget at given index
     *
     * \pre \a index must be in valid range ( 0 <= index < size() )
     */
    QWidget *widgetAt(int index) const;

    /*! \brief Remove mapping at given index
     *
     * \pre \a index must be in valid range ( 0 <= index < size() )
     */
    void removeMappingAt(int index);

    /*! \brief Get count of mapped widgets
     */
    int size() const
    {
      return mWidgetList.size();
    }

    /*! \brief Check if some widget is mapped
     */
    bool isEmpty() const
    {
      return mWidgetList.empty();
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return mWidgetList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return mWidgetList.cend();
    }

    /*! \brief Remove all mapped widgets
     */
    void clear();

   private:

    const_iterator iteratorForWidget(const QWidget * const widget) const;
    const_iterator iteratorForColumn(int column) const;

    std::vector<MappedWidget> mWidgetList;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_LIST_H
