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

#include <QSqlDatabase>
#include <QObject>

class mdtSqlWindow;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QPushButton;

/*! \brief Cable list's article editor
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
   *
   * \param includeConnections If true, Unit_tbl will be added as child widget.
   */
  bool setupTables(bool includeConnections);

  /*! \brief Setup GUI
   *
   * Will dispose internal widgets into window
   *
   * \pre window must be a valid pointer
   */
  void setupUi(mdtSqlWindow *window);

 private:

  /*! \brief Get current Article ID
   *
   * Will return current ID from Unit table.
   *  Returns a value < 0 on error (no row, ...)
   */
  int currentVehicleTypeId();

  /*! \brief Setup Article table and widget
   */
  bool setupVehicleTypeTable();

  /*! \brief Setup Article-Connection table and widget
   */
  bool setupUnitTable();

  Q_DISABLE_COPY(mdtClVehicleTypeEditor);

  QSqlDatabase pvDatabase;
  mdtSqlFormWidget *pvVehicleTypeWidget;
  mdtSqlTableWidget *pvUnitWidget;
  QSqlTableModel *pvVehicleTypeModel;
  QSqlTableModel *pvUnitModel;
  mdtSqlRelation *pvUnitRelation;
};

#endif  // #ifndef MDT_CL_VEHICLE_TYPE_EDITOR_H
