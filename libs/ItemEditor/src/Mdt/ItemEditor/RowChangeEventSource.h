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
#ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_SOURCE_H
#define MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_SOURCE_H

#include <QMetaType>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Enumeration of various sources that can cause a row state change
   *
   * \sa RowChangeEventDispatcher
   */
  enum class RowChangeEventSource
  {
    ModelReset,     /*!< A new model was set, or existing model was repopulated. */
    Controller,     /*!< Source of request come from controller (f.ex. AbstractController::setCurrentRow(int) was called). */
    ItemSelection   /*!< User selected a item in a view (f.e.x in a QTableView). */
  };

}} // namespace Mdt{ namespace ItemEditor{

Q_DECLARE_METATYPE(Mdt::ItemEditor::RowChangeEventSource)

#endif // #ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_SOURCE_H
