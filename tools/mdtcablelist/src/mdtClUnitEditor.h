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
#ifndef MDT_CL_UNIT_EDITOR_H
#define MDT_CL_UNIT_EDITOR_H

#include <QSqlDatabase>
#include <QObject>
#include "mdtSqlFormWindow.h"

class mdtSqlWindow;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QPushButton;

class mdtSqlFormWindow;

/*! \brief Cable list's device editor
 */
class mdtClUnitEditor : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Contruct a cable editor
   */
  mdtClUnitEditor(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClUnitEditor();

  /*! \brief Setup tables
   *
   * \param includeConnections If true, connection_tbl will be added as child widget.
   */
  bool setupTables(bool includeConnections);

  /*! \brief Get form object
   */
  mdtSqlFormWindow *form();

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

  /*! \brief Set the base article of current unit
   */
  void setBaseArticle();

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

  /*! \brief Setup unit connection view table and widget
   */
  bool setupUnitConnectionViewTable();

  /*! \brief Setup unit connection editor table and widget
   */
  bool setupUnitConnectionEditTable();

  /*! \brief Setup VehicleUnit table and widget
   */
  bool setupVehicleTable();

  /*! \brief Setup base Article table and widget
   */
  bool setupArticleTable();

  Q_DISABLE_COPY(mdtClUnitEditor);

  QSqlDatabase pvDatabase;
  // Unit objects
  ///mdtSqlFormWidget *pvUnitWidget;
  ///QSqlTableModel *pvUnitModel;
  // Unit connection view objects
  ///mdtSqlTableWidget *pvUnitConnectionViewWidget;
  ///QSqlTableModel *pvUnitConnectionViewModel;
  ///mdtSqlRelation *pvUnitConnectionViewRelation;
  // Unit connection edit objects
  ///mdtSqlFormWidget *pvUnitConnectionEditWidget;
  ///QSqlTableModel *pvUnitConnectionEditModel;
  ///mdtSqlRelation *pvUnitConnectionEditRelation;
  // Vehicle objects
  ///mdtSqlTableWidget *pvVehicleTypeWidget;
  ///QSqlTableModel *pvVehicleTypeModel;
  ///mdtSqlRelation *pvVehicleTypeRelation;
  
  // Form object
  mdtSqlFormWindow *pvForm;
  // Base article objects
  /**
  mdtSqlTableWidget *pvArticleWidget;
  QSqlTableModel *pvArticleModel;
  mdtSqlRelation *pvArticleRelation;
  */
};

#endif  // #ifndef
