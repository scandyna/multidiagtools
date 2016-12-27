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
#ifndef MDT_ITEM_EDITOR_TABLE_VIEW_WIDGET_H
#define MDT_ITEM_EDITOR_TABLE_VIEW_WIDGET_H

#include "AbstractTableViewWidget.h"
#include "TableViewController.h"

class QAbstractItemModel;

namespace Mdt{ namespace ItemEditor{

  /*! \brief QTableView based editor
   */
  class TableViewWidget : public AbstractTableViewWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit TableViewWidget(QWidget* parent = nullptr);

    /*! \brief Get controller
     */
    TableViewController *controller() const
    {
      return mController;
    }

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the editor does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
    void setModel(QAbstractItemModel *model);

   private:

    TableViewController *mController;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_TABLE_VIEW_WIDGET_H
