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
#ifndef MDT_ITEM_EDITOR_TABLE_CONTROLLER_H
#define MDT_ITEM_EDITOR_TABLE_CONTROLLER_H

#include "AbstractController.h"
#include <QPointer>

class QAbstractTableModel;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Controller for QAbstractTableModel
   */
  class TableController : public AbstractController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit TableController(QObject* parent = nullptr);

    // Copy disabled
    TableController(const TableController &) = delete;
    TableController & operator=(const TableController &) = delete;
    // Move disabled
    TableController(TableController &&) = delete;
    TableController & operator=(TableController &&) = delete;

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
//     void setModel(QAbstractTableModel *model) override;

   private:

//     QPointer<QAbstractTableModel> pvModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_TABLE_CONTROLLER_H
