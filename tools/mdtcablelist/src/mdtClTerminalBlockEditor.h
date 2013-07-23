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
#ifndef MDT_CL_TERMINAL_BLOCK_EDITOR_H
#define MDT_CL_TERMINAL_BLOCK_EDITOR_H

#include <QSqlDatabase>
#include <QObject>

class mdtSqlWindow;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QPushButton;

/*! \brief Cable list's device editor
 */
class mdtClTerminalBlockEditor : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Contruct a cable editor
   */
  mdtClTerminalBlockEditor(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClTerminalBlockEditor();

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Setup GUI
   *
   * Will dispose internal widgets into window
   *
   * \pre window must be a valid pointer
   */
  void setupUi(mdtSqlWindow *window);

 private slots:

  /*! \brief Assign a vehicle to current unit
   */
  void assignVehicle();

  /*! \brief Remove a assigned vehicle to current unit
   */
  void removeVehicleAssignation();

  /*! \brief Add a connection to unit connection table
   */
  void addConnection();

  /*! \brief Remove connection from unit connection table
   */
  void removeConnection();

 signals:

  /*! \brief Used to tell unit widget that we have edited some data
   */
  void unitEdited();

 private:

  /*! \brief Get current Unit ID
   *
   * Will return current ID from Unit table.
   *  Returns a value < 0 on error (no row, ...)
   */
  int currentUnitId();

  /*! \brief Setup Unit table and widget
   */
  bool setupUnitTable();

  /*! \brief Setup terminal ( = unit connection) edit table and widget
   */
  bool setupTerminalEditTable();

  /*! \brief Setup VehicleUnit table and widget
   */
  bool setupVehicleTable();

  Q_DISABLE_COPY(mdtClTerminalBlockEditor);

  QSqlDatabase pvDatabase;
  // Unit objects
  mdtSqlFormWidget *pvUnitWidget;
  QSqlTableModel *pvUnitModel;
  // Unit connection view objects
  // Terminal ( = Unit connection) view objects
  mdtSqlTableWidget *pvTerminalEditWidget;
  QSqlTableModel *pvTerminalEditModel;
  mdtSqlRelation *pvTerminalEditRelation;
  // Vehicle objects
  mdtSqlTableWidget *pvVehicleTypeWidget;
  QSqlTableModel *pvVehicleTypeModel;
  mdtSqlRelation *pvVehicleTypeRelation;
};

#endif  // #ifndef MDT_CL_TERMINAL_BLOCK_EDITOR_H
