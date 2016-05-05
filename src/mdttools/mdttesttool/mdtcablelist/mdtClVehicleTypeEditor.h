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
#ifndef MDT_CL_VEHICLE_TYPE_EDITOR_H
#define MDT_CL_VEHICLE_TYPE_EDITOR_H

#include "mdtSqlForm.h"
#include <QVariant>

class QWidget;

/*! \brief Vehicle type editor
 */
class mdtClVehicleTypeEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Contruct a vehicle type editor
   */
  mdtClVehicleTypeEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClVehicleTypeEditor();

  /*! \brief Setup tables
   */
  bool setupTables();

/// signals:

  /*! \brief Used to tell vehicle type widget that we have edited some data
   */
  ///void vehicleTypeEdited();

 private slots:

  /*! \brief Edit selected unit
   */
  void editSelectedUnit();

  /*! \brief Edit selected link
   */
  void editLink();

  /*! \brief View a path of links
   */
  void viewLinkPath();

  /*! \brief Export markins to CSV file
   */
//   void exportMarkingsToCsvFile();

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

  /*! \brief Setup LinkList_view table
   */
  bool setupLinkListTable();

  /*! \brief Setup link marking table
   */
  bool setupLinkMarkingTable();

  Q_DISABLE_COPY(mdtClVehicleTypeEditor);
};

#endif  // #ifndef MDT_CL_VEHICLE_TYPE_EDITOR_H
