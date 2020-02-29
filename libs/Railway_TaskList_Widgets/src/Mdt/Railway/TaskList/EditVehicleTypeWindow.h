/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_RAILWAY_TASK_LIST_EDIT_VEHICLE_TYPE_WIDNOW_H
#define MDT_RAILWAY_TASK_LIST_EDIT_VEHICLE_TYPE_WIDNOW_H

#include "Mdt/Railway/TaskList/EditVehicleTypeTableModel.h"
#include "MdtRailway_TaskList_WidgetsExport.h"
#include <QWidget>
#include <QMainWindow>
#include <memory>

namespace Mdt{ namespace Railway{ namespace TaskList{

  namespace Ui{
    class EditVehicleTypeWindow;
  }

  /*!\brief
   */
  class MDT_RAILWAY_TASKLIST_WIDGETS_EXPORT EditVehicleTypeWindow : public QMainWindow
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit EditVehicleTypeWindow(QWidget *parent = nullptr);

    /*! \brief Destructor
     */
    ~EditVehicleTypeWindow();

    void setEditVehicleTypeTableModel(const std::shared_ptr<EditVehicleTypeTableModel> & model);

   private:

    std::shared_ptr<EditVehicleTypeTableModel> mEditVehicleTypeTableModel;
    std::unique_ptr<Ui::EditVehicleTypeWindow> mUi;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_EDIT_VEHICLE_TYPE_WIDNOW_H
