/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_TT_TEST_SYSTEM_UNIT_EDITOR_H
#define MDT_TT_TEST_SYSTEM_UNIT_EDITOR_H

#include "mdtSqlForm.h"
#include <QSqlDatabase>
#include <QList>
#include <QLocale>

class QWidget;

/*! \brief Test system unit editor
 */
class mdtTtTestSystemUnitEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestSystemUnitEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private slots:

  /*! \brief Set the base article of current unit
   */
  void setBaseArticle();

  /*! \brief Set the base article of current unit
   */
  void unsetBaseArticle();

  /*! \brief Add a free connector (based on nothing) to unit connector table
   */
  void addConnector();

  /*! \brief Add a connector based on a one from Connector_tbl to unit connector table
   */
  void addConnectorBasedConnector();

  /*! \brief Add a connector based on article connector to unit connector table
   */
  void addArticleConnectorBasedConnector();

  /*! \brief Edit connector name
   */
  void editConnectorName();

  /*! \brief Remove a connector
   */
  void removeConnectors();

  /*! \brief Add a connection to unit connection table
   */
  void addConnection();

  /*! \brief Add connections based on article connections
   */
  void addArticleConnectionsBasedConnections();

  /*! \brief Edit a connection
   */
  void editConnection();

  /*! \brief Remove connection from unit connection table
   */
  void removeConnections();

 private:

  /*! \brief Get current unit ID
   */
  QVariant currentUnitId()
  {
    return currentData("TestSystemUnit_tbl", "Unit_Id_FK_PK");
  }

  /*! \brief Let user select connections related to a connector from Connector_tbl
   */
  QList<QVariant> selectBaseConnectorContactIdList(const QVariant & connectorId, bool multiSelection = true);

  /*! \brief Let user choose a connector from those that are related to current article
   */
  QVariant selectArticleConnector();

  /*! \brief Select article connections related to given article connector ID
   */
  QList<QVariant> selectByArticleConnectorIdArticleConnectionIdList(const QVariant & articleConnectorId, const QVariant & unitId, bool multiSelection = true);

  /*! \brief Setup Unit_tbl + TestSystemUnit_tbl part
   */
  bool setupTestSystemUnitTables();

  /*! \brief Setup UnitConnector_view part
   */
  bool setupConnectorTable();

  /*! \brief Setup UnitConnection_view part
   */
  bool setupConnectionTable();

  /*! \brief Setup UnitLink_view part
   */
  bool setupLinkTable();

  Q_DISABLE_COPY(mdtTtTestSystemUnitEditor);
};

#endif // #ifndef MDT_TT_TEST_SYSTEM_UNIT_EDITOR_H
