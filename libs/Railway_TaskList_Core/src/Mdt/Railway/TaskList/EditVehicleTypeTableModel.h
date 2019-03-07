/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_RAILWAY_TASK_LIST_EDIT_VEHICLE_TYPE_TABLE_MODEL_H
#define MDT_RAILWAY_TASK_LIST_EDIT_VEHICLE_TYPE_TABLE_MODEL_H

#include "Mdt/ItemModel/AbstractReadOnlyCachedTableModel.h"
#include "MdtRailway_TaskList_CoreExport.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief Table model to edit vehicle types
   */
  class MDT_RAILWAY_TASKLIST_CORE_EXPORT EditVehicleTypeTableModel : public Mdt::ItemModel::AbstractReadOnlyCachedTableModel
  {
    Q_OBJECT

    using BaseClass = AbstractReadOnlyCachedTableModel;

   public:

    /*! \brief Constructor
     */
    explicit EditVehicleTypeTableModel(QObject *parent = nullptr);

    /*! \brief Get count of columns
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get vehicle type name column
     */
    constexpr int nameColumn() const noexcept
    {
      return 0;
    }

    /*! \brief Get vehicle type alias column
     */
    constexpr int aliasColumn() const noexcept
    {
      return 1;
    }

    /*! \brief Get vehicle type manufacturer serie column
     */
    constexpr int manufacturerSerieColumn() const noexcept
    {
      return 2;
    }

   private:

    QVariant horizontalHeaderDisplayRoleData(int column) const override;

    bool fetchRecords(int count) override;

  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_EDIT_VEHICLE_TYPE_TABLE_MODEL_H
