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

#include "RowState.h"
#include <QObject>
#include <QWidget>
#include <QPointer>
#include <vector>

class QAbstractItemModel;

namespace Mdt{ namespace ItemEditor{

  /*
   * Data atructure used internally
   */
  struct MappedWidgetListItem
  {
    MappedWidgetListItem(QWidget *w, int c)
     : widget(w), column(c) {}

    QPointer<QWidget> widget;
    int column;
  };

  /*! \brief MappedWidgetList is used by AbstractWidgetMapperController
   */
  class MappedWidgetList : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit MappedWidgetList(QObject* parent = nullptr);

    /*! \brief Add widget
     *
     * \note widget will not be owned by MappedWidgetList
     *        (it will not be deleted)
     */
    void addWidget(QWidget *widget, int column);

    /*! \brief Get widget at index i
     *
     * Index i depends on the order the widgets where mapped,
     *  not on column.
     */
    QWidget *at(int i) const
    {
      Q_ASSERT(i >= 0);
      Q_ASSERT(i < size());
      return pvWidgetList[i].widget;
    }

    /*! \brief Set model
     */
    void setModel(QAbstractItemModel *model);

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

    /*! \brief Remove all mapped widgets
     */
    void clear();

   public slots:

    /*! \brief Set row state
     *
     * Will enable/disable widgets regarding row state
     */
    void setRowState(RowState rs);

   private:

    /*! \brief Update enable state widget
     */
    void updateWidgetEnableState(QWidget * const widget, int column);

    /*! \brief Update enable state of all mapped widgets
     */
    void updateAllWidgetsEnableState();

    RowState pvRowState;
    std::vector<MappedWidgetListItem> pvWidgetList;
    QPointer<QAbstractItemModel> pvModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_MAPPED_WIDGET_LIST_H
