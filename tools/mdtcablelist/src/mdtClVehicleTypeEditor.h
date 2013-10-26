/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CL_VEHICLE_TYPE_EDITOR_H
#define MDT_CL_VEHICLE_TYPE_EDITOR_H

#include "mdtSqlFormWindow.h"
#include <QSqlDatabase>
#include <QObject>
#include <QVariant>

/*! \brief Vehicle type editor
 */
class mdtClVehicleTypeEditor : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Contruct a vehicle type editor
   */
  mdtClVehicleTypeEditor(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClVehicleTypeEditor();

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Get form object
   */
  mdtSqlFormWindow *form();

 signals:

  /*! \brief Used to tell vehicle type widget that we have edited some data
   */
  void vehicleTypeEdited();

 private:

  /*! \brief Get current vehicle type ID
   */
  QVariant currentVehicleTypeId();

  /*! \brief Setup Article table and widget
   */
  bool setupVehicleTypeTable();

  /*! \brief Setup Article-Connection table and widget
   */
  bool setupUnitTable();

  Q_DISABLE_COPY(mdtClVehicleTypeEditor);

  QSqlDatabase pvDatabase;
  // Form object
  mdtSqlFormWindow *pvForm;
};

#endif  // #ifndef MDT_CL_VEHICLE_TYPE_EDITOR_H
