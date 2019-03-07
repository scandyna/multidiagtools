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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_EDITION_WINDOW_H
#define MDT_RAILWAY_VEHICLE_TYPE_EDITION_WINDOW_H

#include "Mdt/Railway/CreateVehicleType.h"
#include "Mdt/Railway/Error.h"
#include "Mdt/Railway/VehicleTypeClassId.h"
#include "Mdt/Railway/EditVehicleTypeClassTableModel.h"
#include "Mdt/Railway/EditVehicleTypeTableModel.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.h"
#include "Mdt/ItemModel/ProxyModelContainer.h"
#include "Mdt/ItemModel/SelectionModelCurrentRowChanged.h"
#include "Mdt/ItemEditor/DataWidgetMapper.h"
#include "Mdt/Error.h"
#include "MdtRailway_WidgetsExport.h"
#include <QWidget>
#include <QMainWindow>
#include <memory>

namespace Mdt{ namespace Railway{

  namespace Ui{
    class VehicleTypeEditionWindow;
  }

  /*! \brief Vehicle type edition window
   */
  class MDT_RAILWAY_WIDGETS_EXPORT VehicleTypeEditionWindow : public QMainWindow
  {
   Q_OBJECT

    using SelectQueryFactory = Mdt::QueryExpression::AbstractAsyncSelectQueryFactory;

   public:

    /*! \brief Constructor
     */
    VehicleTypeEditionWindow(QWidget *parent = nullptr);

    /*! \brief Destructor
     */
    ~VehicleTypeEditionWindow();

    void setVehicleTypeClassTableModel(const std::shared_ptr<EditVehicleTypeClassTableModel> & model);

    void setEditVehicleTypeTableModel(const std::shared_ptr<EditVehicleTypeTableModel> & model);

   public slots:

    void displayError(const Mdt::Error & error);

//    signals:

//     void createVehicleTypeRequested(const CreateVehicleTypeRequest & request);

   private slots:

    void addNew();
    void save();

   private:

    void setSelectedVehicleTypeClassIdToModel();
    VehicleTypeClassId selectedVehicleTypeClassId() const;
    void setupChooseVehicleTypeClass();
    void setupVehicleTypeModels();
    void setupChooseVehicleType();
    void setupEditVehicleType();

    std::shared_ptr<EditVehicleTypeClassTableModel> mVehicleTypeClassTableModel;
    Mdt::ItemModel::ProxyModelContainer mEditVehicleTypeModelContainer;
    Mdt::ItemModel::SelectionModelCurrentRowChanged mVehicleTypeCurrentRowChanged;
    std::shared_ptr<EditVehicleTypeTableModel> mEditVehicleTypeTableModel;
    Mdt::ItemEditor::DataWidgetMapper *mWidgetMapper = nullptr;
    
    std::unique_ptr<Ui::VehicleTypeEditionWindow> mUi;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_EDITION_WINDOW_H
