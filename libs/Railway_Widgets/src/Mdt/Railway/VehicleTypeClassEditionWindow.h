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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_EDITION_WINDOW_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_EDITION_WINDOW_H

#include "Mdt/Railway/VehicleTypeClass.h"
#include "Mdt/Railway/EditVehicleTypeClassCache.h"
#include "Mdt/Railway/EditVehicleTypeClassCacheAsync.h"
#include "Mdt/Railway/VehicleTypeClassRepository.h"
#include "Mdt/QueryExpression/AbstractSelectQueryFactory.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.h"
#include "MdtRailway_WidgetsExport.h"
#include "Mdt/Entity/AbstractReadOnlyCacheTableModel.h"
#include "Mdt/Error.h"
#include <QWidget>
#include <QMainWindow>
#include <QUndoStack>
#include <QAction>
#include <memory>

namespace Mdt{ namespace Railway{

  namespace Ui{
    class VehicleTypeClassEditionWindow;
  }

  class VehicleTypeClassTableModel;
  class EditVehicleTypeClassTableModel;

  /*! \brief Vehicle type edition window
   */
  class MDT_RAILWAY_WIDGETS_EXPORT VehicleTypeClassEditionWindow : public QMainWindow
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    VehicleTypeClassEditionWindow(QWidget *parent = nullptr);

    /*! \brief Destructor
     */
    ~VehicleTypeClassEditionWindow();

    /*! \brief Set vehicle type class repository
     */
    void setVehicleTypeClassRepository(const VehicleTypeClassRepositoryHandle & repository);

    /*! \brief Set vehicle type class repository
     */
    void setVehicleTypeClassRepository(const std::shared_ptr<VehicleTypeClassRepository> & repository);

    /*! \brief Set the query factory
     *
     * \pre \a factory must be valid
     */
    void setSelectQueryFactory(const std::shared_ptr<Mdt::QueryExpression::AbstractSelectQueryFactory> & factory);

    /*! \brief Set the async query factory
     *
     * \pre \a factory must be valid
     */
    void setAsyncSelectQueryFactory(const std::shared_ptr<Mdt::QueryExpression::AbstractAsyncSelectQueryFactory> & factory);

   private slots:

    void addVehicleTypeClass();
    void removeSelectedVehicleTypeClasses();
    void unremoveSelectedVehicleTypeClasses();
    void save();

   private:

    void displayError(const Mdt::Error & error);
    void createUndoActions();

    std::unique_ptr<Ui::VehicleTypeClassEditionWindow> mUi;
    VehicleTypeClassTableModel *mTableModel;
    EditVehicleTypeClassTableModel *mEditVehicleTypeClassTableModel;
    Mdt::Entity::AbstractReadOnlyCacheTableModel *mEditVehicleTypeClassTableModelAsync;
    EditVehicleTypeClassCache mEditVehicleTypeClassCache;
    EditVehicleTypeClassCacheAsync mEditVehicleTypeClassCacheAsync;
    VehicleTypeClass *mVehicleTypeClass;
    QUndoStack *mUndoStack;
    QAction *mUndoAction;
    QAction *mRedoAction;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_EDITION_WINDOW_H
