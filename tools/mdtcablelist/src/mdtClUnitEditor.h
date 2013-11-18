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
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include "mdtSqlFormWindow.h"
#include "mdtClEditor.h"
#include "mdtClUnitConnectionData.h"

/*! \brief Cable list's device editor
 */
class mdtClUnitEditor : public mdtClEditor
{
 Q_OBJECT

 public:

  /*! \brief Contruct a cable editor
   */
  mdtClUnitEditor(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClUnitEditor();

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

  /*! \brief Add component
   */
  void addComponent();

  /*! \brief Remove components
   */
  void removeComponents();

  /*! \brief Add a connector to unit connector table that is related to a article connector
   */
  void addConnector();

  /*! \brief Add a connector to unit connector table
   */
  void addFreeConnector();

  /*! \brief Add a connection to unit connection table that is related to a article connection
   */
  void addConnection();

  /*! \brief Add a connection to unit connection table
   */
  void addFreeConnection();

  /*! \brief Edit a connection
   */
  void editConnection();

  /*! \brief Remove connection from unit connection table
   */
  void removeConnections();

  /*! \brief Add a link
   */
  void addLink();

  /*! \brief Edit selected link
   */
  void editLink();

  /*! \brief Remove  links
   */
  void removeLinks();

  /*! \brief View a path of links
   */
  void viewPath();

 signals:

  /*! \brief Used to tell unit widget that we have edited some data
   */
  void unitEdited();

 private:

  /*! \brief Get current Unit ID
   */
  QVariant currentUnitId();

  /*! \brief Let user choose a connector (from Connector table)
   */
  QVariant selectConnector();

  /*! \brief Select a connector assigned to current unit
   */
  QVariant selectUnitConnector();

  /*! \brief Let user select a article connection related to a unit connector
   */
  QVariant selectArticleConnectionLinkedToUnitConnector(const QVariant & unitConnectorId, const QVariant & unitId);
  
  
  /*! \brief Let user choose a connector from those that are related to current article
   */
  QVariant selectArticleConnector();

  /*! \brief Let user choose connections from those that are related to given article connector
   */
  QList<QVariant> selectArticleConnections(const QVariant & articleConnectorId, bool allowMultipleSelection = true);

  /*! \brief Let user choose a connection from those that are related to given article connector
   */
  QVariant selectArticleConnection(const QVariant & articleConnectorId);

  /*! \brief Let user choose a connection from those that are related to current article
   *
   * \todo Obselete ?
   */
  ///mdtClUnitConnectionData selectArticleConnection(const QVariant & articleId);

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Setup Unit table and widget
   */
  bool setupUnitTable();

  /*! \brief Setup unit component table and widget
   */
  bool setupUnitComponentTable();

  /*! \brief Setup unit connection table and widget
   */
  bool setupUnitConnectionTable();

  /*! \brief Setup unit link table
   */
  bool setupUnitLinkTable();

  /*! \brief Setup VehicleUnit table and widget
   */
  bool setupVehicleTable();

  Q_DISABLE_COPY(mdtClUnitEditor);
};

#endif  // #ifndef
