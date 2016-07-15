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
#ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_H
#define MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_H

#include "AbstractItemViewController.h"

namespace Mdt{ namespace ItemEditor{

  /*! \brief TableViewController acts on a QAbstractTableModel and a QAbstractItemView
   */
  class TableViewController : public AbstractItemViewController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit TableViewController(QObject* parent = nullptr);

    // Copy disabled
    TableViewController(const TableViewController &) = delete;
    TableViewController & operator=(const TableViewController &) = delete;
    // Move disabled
    TableViewController(TableViewController &&) = delete;
    TableViewController & operator=(TableViewController &&) = delete;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_H
